#include "sdk_cfg.h"
#include "audio/audio.h"
#include "audio/dac.h"
#include "audio/dac_api.h"
#include "audio/tone.h"
#include "audio/src.h"
#include "audio/src_api.h"
#include "audio/ladc.h"
#include "audio/digital_vol.h"
#include "audio/audio_stream.h"
#include "board_init.h"
#include "string.h"
#include "flash_api.h"
#include "task_manager.h"
#include "common.h"

struct SOUND_VAR sound;
static u8 sound_cbuffer[OUTPUT_BUF_SIZE] 	AT(.dac_buf_sec);	/* dac cbuffer	*/
static u32 output_buf_limit_val = OUTPUT_BUF_LIMIT;
static volatile u16 audio_visualizer_level;

/* Tracks active DAC sample rate for the dynamic FFT bin mapper */
static volatile u16 audio_current_sample_rate = 44100;

/*
 * Slow-moving baseline: tracks the TRACK's overall loudness (song-to-song
 * mix level), as opposed to audio_visualizer_level above which tracks
 * beat-to-beat transients. Used to pre-normalize signal level before it
 * reaches the spectrum analyzer, so a quietly-mastered track and a loud
 * one both land at a similar analysis level instead of the quiet one
 * starving the FFT bins of signal.
 */
static volatile u16 audio_visualizer_baseline_level;
#define AUDIO_VISUALIZER_BASELINE_TARGET    8000  /* desired analysis level, out of 32767 */
#define AUDIO_VISUALIZER_BASELINE_GAIN_MIN  64    /* Q8 = 0.25x floor: don't over-attenuate loud tracks */
#define AUDIO_VISUALIZER_BASELINE_GAIN_MAX  1024  /* Q8 = 4x ceiling: don't blow up noise during near-silence */

/*
 * Bumped 128 -> 256 for real-time accuracy: doubling N halves the FFT bin
 * width (finer frequency resolution) and doubles the analysis window
 * (more stable magnitude estimate frame-to-frame), at the cost of ~2x the
 * butterflies per call. Still trivial at 192MHz -- see comment above
 * audio_visualizer_fft256() for the cycle budget.
 */
#define AUDIO_VISUALIZER_SAMPLE_COUNT  256
#define AUDIO_VISUALIZER_BAND_COUNT    10
/* Bars are drawn bottom-up from LCD row 61; this leaves headroom for the
 * peak-hold dot (drawn one row above the bar top) to stay on-screen. */
#define AUDIO_VISUALIZER_MAX_BAR_HEIGHT 58
static volatile s16 audio_visualizer_pcm[AUDIO_VISUALIZER_SAMPLE_COUNT];
static volatile u8 audio_visualizer_pcm_pos;

/*
 * Real 256-point radix-2 FFT support tables, Q15.
 * A full FFT with a proper complex magnitude avoids the "phantom energy"
 * that an approximate Goertzel magnitude can show in bins that shouldn't
 * have any, and 256 points gives roughly 2x the frequency resolution of
 * the previous 128-point version for the same sample rate.
 */
#define AUDIO_VISUALIZER_FFT_SIZE       AUDIO_VISUALIZER_SAMPLE_COUNT  /* 256 */
#define AUDIO_VISUALIZER_FFT_STAGES     8                              /* log2(256) */

/* Forward-DFT twiddle factors e^{-j*2*pi*k/256}, Q15, k = 0..127. */
static const s16 audio_visualizer_fft_twiddle_re[AUDIO_VISUALIZER_FFT_SIZE / 2] = {
    32767, 32757, 32728, 32678, 32609, 32521, 32412, 32285,
    32137, 31971, 31785, 31580, 31356, 31113, 30852, 30571,
    30273, 29956, 29621, 29268, 28898, 28510, 28105, 27683,
    27245, 26790, 26319, 25832, 25329, 24811, 24279, 23731,
    23170, 22594, 22005, 21403, 20787, 20159, 19519, 18868,
    18204, 17530, 16846, 16151, 15446, 14732, 14010, 13279,
    12539, 11793, 11039, 10278, 9512, 8739, 7962, 7179,
    6393, 5602, 4808, 4011, 3212, 2410, 1608, 804,
    0, -804, -1608, -2410, -3212, -4011, -4808, -5602,
    -6393, -7179, -7962, -8739, -9512, -10278, -11039, -11793,
    -12539, -13279, -14010, -14732, -15446, -16151, -16846, -17530,
    -18204, -18868, -19519, -20159, -20787, -21403, -22005, -22594,
    -23170, -23731, -24279, -24811, -25329, -25832, -26319, -26790,
    -27245, -27683, -28105, -28510, -28898, -29268, -29621, -29956,
    -30273, -30571, -30852, -31113, -31356, -31580, -31785, -31971,
    -32137, -32285, -32412, -32521, -32609, -32678, -32728, -32757,
};
static const s16 audio_visualizer_fft_twiddle_im[AUDIO_VISUALIZER_FFT_SIZE / 2] = {
    0, -804, -1608, -2410, -3212, -4011, -4808, -5602,
    -6393, -7179, -7962, -8739, -9512, -10278, -11039, -11793,
    -12539, -13279, -14010, -14732, -15446, -16151, -16846, -17530,
    -18204, -18868, -19519, -20159, -20787, -21403, -22005, -22594,
    -23170, -23731, -24279, -24811, -25329, -25832, -26319, -26790,
    -27245, -27683, -28105, -28510, -28898, -29268, -29621, -29956,
    -30273, -30571, -30852, -31113, -31356, -31580, -31785, -31971,
    -32137, -32285, -32412, -32521, -32609, -32678, -32728, -32757,
    -32767, -32757, -32728, -32678, -32609, -32521, -32412, -32285,
    -32137, -31971, -31785, -31580, -31356, -31113, -30852, -30571,
    -30273, -29956, -29621, -29268, -28898, -28510, -28105, -27683,
    -27245, -26790, -26319, -25832, -25329, -24811, -24279, -23731,
    -23170, -22594, -22005, -21403, -20787, -20159, -19519, -18868,
    -18204, -17530, -16846, -16151, -15446, -14732, -14010, -13279,
    -12539, -11793, -11039, -10278, -9512, -8739, -7962, -7179,
    -6393, -5602, -4808, -4011, -3212, -2410, -1608, -804,
};
/* Bit-reversal permutation table for N=256 (8-bit reversal). */
static const u8 audio_visualizer_fft_bitrev[AUDIO_VISUALIZER_FFT_SIZE] = {
    0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
    8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
    4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
    12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
    2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
    10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
    6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246,
    14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254,
    1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241,
    9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249,
    5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245,
    13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 60, 188, 124, 252,
    3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243,
    11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251,
    7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247,
    15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255,
};

/*
 * Logarithmic target center frequencies (Hz) for standard 10-band analysis.
 * Log-spaced from sub-bass kick weight up to high treble air.
 */
static const u16 target_freqs[AUDIO_VISUALIZER_BAND_COUNT] = {
    60,   /* Band 0: Sub-bass (Kick-drum weight) */
    125,  /* Band 1: Mid-bass (Punch) */
    250,  /* Band 2: Low-mids (Warmth) */
    500,  /* Band 3: Mids (Vocal/Instrument body) */
    1000, /* Band 4: Mids (Vocal presence) */
    1600, /* Band 5: High-mids (Clarity) */
    2500, /* Band 6: Presence (Crispness) */
    4000, /* Band 7: Treble (Detail) */
    6000, /* Band 8: High treble (Sparkle) */
    9000  /* Band 9: High treble (Air) */
};

/* Working buffers for the in-place FFT. */
static s16 audio_visualizer_fft_re[AUDIO_VISUALIZER_FFT_SIZE];
static s16 audio_visualizer_fft_im[AUDIO_VISUALIZER_FFT_SIZE];
/* Per-band running peak, used for auto-gain normalization (replaces fixed gain table). */
static u16 audio_visualizer_band_peak[AUDIO_VISUALIZER_BAND_COUNT];

/*
 * Post-AGC gain, in percent (100 = unchanged). AGC already normalizes each
 * band to its own recent peak, so this isn't compensating for loudness --
 * it's a display-only "how tall does this band get to look" knob. Raise the
 * mid/high entries to make those bars visually punchier relative to bass.
 */
static const u8 audio_visualizer_band_gain_pct[AUDIO_VISUALIZER_BAND_COUNT] = {
    100, 100, 100, 100, 100, 110, 120, 130, 140, 150,
};

/*
 * 256-point Hann window, Q14. An unwindowed FFT has wide, high sidelobes,
 * so energy from one band leaks into its neighbors (e.g. bass smearing
 * into the low-mids). Windowing the samples before the FFT suppresses
 * that leakage and gives cleaner band separation.
 */
static const s16 audio_visualizer_window[AUDIO_VISUALIZER_SAMPLE_COUNT] = {
    0, 2, 10, 22, 40, 62, 89, 122,
    159, 201, 247, 299, 355, 417, 483, 553,
    628, 708, 793, 881, 975, 1072, 1174, 1281,
    1391, 1506, 1624, 1747, 1874, 2004, 2138, 2276,
    2417, 2562, 2710, 2862, 3017, 3175, 3336, 3500,
    3667, 3837, 4009, 4184, 4361, 4541, 4722, 4906,
    5092, 5280, 5469, 5661, 5853, 6047, 6243, 6439,
    6637, 6836, 7035, 7235, 7436, 7637, 7839, 8041,
    8242, 8444, 8646, 8847, 9048, 9249, 9449, 9648,
    9846, 10043, 10239, 10434, 10627, 10819, 11010, 11198,
    11385, 11570, 11753, 11934, 12112, 12288, 12462, 12633,
    12801, 12966, 13129, 13288, 13445, 13598, 13748, 13895,
    14038, 14178, 14313, 14446, 14574, 14699, 14819, 14936,
    15049, 15157, 15261, 15361, 15456, 15548, 15634, 15716,
    15794, 15867, 15935, 15999, 16057, 16111, 16161, 16205,
    16245, 16279, 16309, 16334, 16354, 16368, 16378, 16383,
    16383, 16378, 16368, 16354, 16334, 16309, 16279, 16245,
    16205, 16161, 16111, 16057, 15999, 15935, 15867, 15794,
    15716, 15634, 15548, 15456, 15361, 15261, 15157, 15049,
    14936, 14819, 14699, 14574, 14446, 14313, 14178, 14038,
    13895, 13748, 13598, 13445, 13288, 13129, 12966, 12801,
    12633, 12462, 12288, 12112, 11934, 11753, 11570, 11385,
    11198, 11010, 10819, 10627, 10434, 10239, 10043, 9846,
    9648, 9449, 9249, 9048, 8847, 8646, 8444, 8242,
    8041, 7839, 7637, 7436, 7235, 7035, 6836, 6637,
    6439, 6243, 6047, 5853, 5661, 5469, 5280, 5092,
    4906, 4722, 4541, 4361, 4184, 4009, 3837, 3667,
    3500, 3336, 3175, 3017, 2862, 2710, 2562, 2417,
    2276, 2138, 2004, 1874, 1747, 1624, 1506, 1391,
    1281, 1174, 1072, 975, 881, 793, 708, 628,
    553, 483, 417, 355, 299, 247, 201, 159,
    122, 89, 62, 40, 22, 10, 2, 0,
};

extern void music_decoder_loop_resume(void);

void audio_set_output_buf_limit(u32 limit_val)
{
    output_buf_limit_val = limit_val;
}

static u32 audio_stream_write(AUDIO_STREAM_DAC *obj, void *buf, u32 len)
{
    s16 *samples;
    u32 sample_count;
    u32 sample_index;
    s32 sample;
    u16 peak = 0;

    if (obj == NULL) {
        return 0;
    }

    /*
     * Read a sparse set of final PCM samples before they enter the DAC queue.
     * This is intentionally read-only and costs only a few dozen operations per
     * decoded buffer, making it suitable for the OLED VU visualizer.
     */
    samples = (s16 *)buf;
    sample_count = len >> 1;
    for (sample_index = 0; sample_index < sample_count; sample_index += 4) {
        sample = samples[sample_index];
        audio_visualizer_pcm[audio_visualizer_pcm_pos++] = (s16)sample;
        audio_visualizer_pcm_pos &= (AUDIO_VISUALIZER_SAMPLE_COUNT - 1);
        if (sample < 0) {
            sample = -sample;
        }
        if (sample > peak) {
            peak = (u16)sample;
        }
    }
    if (peak > audio_visualizer_level) {
        /* Fast attack so beats are visible. */
        audio_visualizer_level = (audio_visualizer_level + peak) >> 1;
    } else {
        /* Slow release keeps the display smooth between samples. */
        audio_visualizer_level = (audio_visualizer_level * 7 + peak) >> 3;
    }
    if (peak > audio_visualizer_baseline_level) {
        /* Slow rise: a single loud transient shouldn't snap the baseline up. */
        audio_visualizer_baseline_level += (peak - audio_visualizer_baseline_level) >> 6;
    } else {
        /* Even slower fall: this should track the song's overall mix level
         * over a second or two, not react to a brief quiet passage. */
        audio_visualizer_baseline_level -= (audio_visualizer_baseline_level - peak) >> 8;
    }
#if 0
    extern u8 debug_open_dac_flag;
    if (debug_open_dac_flag) {
        debug_open_dac_flag++;
        if (debug_open_dac_flag < 10) {
            putchar('L');
            put_buf(buf, 30);

        } else {

            debug_open_dac_flag = 0;
        }
    }
#endif
    u32 wlen;
    u32 limit = cbuf_get_space(&(obj->cbuf)) / 2;
    /* printf("W"); */
    wlen = cbuf_write(&(obj->cbuf), buf, len);
    if (wlen == len) {
        /* music_decoder_loop_resume(); */
    } else {

    }
    if (obj->flag == 0) {
        if (cbuf_get_data_size(&(obj->cbuf)) >= limit) {
            /* log_printf("data enough !!\n"); */
            obj->flag = 1;
        }
    }

    return wlen;
}

u16 audio_visualizer_get_level(void)
{
    return audio_visualizer_level;
}

/*
 * In-place decimation-in-time radix-2 FFT, N=256, Q15 twiddles.
 * Input must already be in natural (linear) sample order in re[]/im[];
 * this function bit-reverses it internally before the butterfly stages,
 * and leaves the result in natural bin order (re[k]/im[k] = bin k).
 *
 * Each butterfly stage scales down by /2 ("scaled FFT"), so after all 8
 * stages the output is the true DFT divided by 256.
 *
 * Cost: 8 stages x 128 butterflies/stage = 1024 butterflies, each a
 * handful of MACs -- roughly 4k MACs total. Trivial load at 192MHz.
 */
static void audio_visualizer_fft256(s16 *re, s16 *im)
{
    u16 i;
    u16 j;
    u16 k;
    u16 stage;
    u16 m;
    u16 half;
    u16 step;
    u16 idx_a;
    u16 idx_b;
    u16 tw_idx;
    s16 tmp;
    s32 tr;
    s32 ti;
    s16 ar;
    s16 ai;
    s16 br;
    s16 bi;

    /* Bit-reversal permutation (swap-if-greater avoids undoing a swap). */
    for (i = 0; i < AUDIO_VISUALIZER_FFT_SIZE; i++) {
        j = audio_visualizer_fft_bitrev[i];
        if (j > i) {
            tmp = re[i];
            re[i] = re[j];
            re[j] = tmp;
            tmp = im[i];
            im[i] = im[j];
            im[j] = tmp;
        }
    }

    /* Butterfly stages: m doubles each stage (2, 4, 8, ... 256). */
    for (stage = 1; stage <= AUDIO_VISUALIZER_FFT_STAGES; stage++) {
        m = (u16)(1U << stage);
        half = (u16)(m >> 1);
        step = (u16)(AUDIO_VISUALIZER_FFT_SIZE / m);
        for (k = 0; k < AUDIO_VISUALIZER_FFT_SIZE; k += m) {
            for (j = 0; j < half; j++) {
                tw_idx = (u16)(j * step);
                idx_a = (u16)(k + j);
                idx_b = (u16)(idx_a + half);

                /* (re[idx_b] + j*im[idx_b]) * twiddle, Q15 -> back to Q0. */
                tr = ((s32)re[idx_b] * audio_visualizer_fft_twiddle_re[tw_idx] -
                      (s32)im[idx_b] * audio_visualizer_fft_twiddle_im[tw_idx]) >> 15;
                ti = ((s32)re[idx_b] * audio_visualizer_fft_twiddle_im[tw_idx] +
                      (s32)im[idx_b] * audio_visualizer_fft_twiddle_re[tw_idx]) >> 15;

                /* /2 scaling on both halves keeps magnitude bounded every stage. */
                ar = (s16)(re[idx_a] >> 1);
                ai = (s16)(im[idx_a] >> 1);
                br = (s16)(tr >> 1);
                bi = (s16)(ti >> 1);

                re[idx_a] = (s16)(ar + br);
                im[idx_a] = (s16)(ai + bi);
                re[idx_b] = (s16)(ar - br);
                im[idx_b] = (s16)(ai - bi);
            }
        }
    }
}

/*
 * Alpha-max-plus-beta-min magnitude estimate: |z| ~= 0.9604*max + 0.3978*min
 * of (|re|,|im|). Max error ~4% vs a true sqrt(re^2+im^2). Fast, single-cycle MACs.
 */
static u32 audio_visualizer_magnitude(s16 re, s16 im)
{
    u32 a;
    u32 b;
    u32 hi;
    u32 lo;

    a = (re < 0) ? (u32)(-(s32)re) : (u32)re;
    b = (im < 0) ? (u32)(-(s32)im) : (u32)im;
    if (a > b) {
        hi = a;
        lo = b;
    } else {
        hi = b;
        lo = a;
    }
    return ((hi * 246) + (lo * 102)) >> 8;
}

/*
 * A full 256-point FFT of the last 256 PCM samples, with a proper complex
 * magnitude per bin.
 *
 * Incorporates:
 * 1. Thread-Safe atomic snapshotting of PCM data to prevent visual waveform tearing.
 * 2. Dynamic logarithmic frequency bin mapping based on active DAC sample rate.
 */
void audio_visualizer_get_spectrum(u8 *bands, u8 count)
{
    u8 band;
    u16 sample_index;
    u32 magnitude;
    s32 normalized;
    u16 peak;
    u16 baseline;
    u32 baseline_gain_q8;
    u16 fs;

    /* Local stack buffers to hold a stable, untorn snapshot of the PCM data */
    s16 local_pcm[AUDIO_VISUALIZER_SAMPLE_COUNT];
    u8 local_start;

    if (bands == NULL) {
        return;
    }
    if (count > AUDIO_VISUALIZER_BAND_COUNT) {
        count = AUDIO_VISUALIZER_BAND_COUNT;
    }

    /*
     * Thread-Safe Snapshot: Copy the volatile circular buffer to local memory
     * inside an atomic block to prevent DAC interrupts from mutating the data
     * during windowing and FFT execution.
     */
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    local_start = audio_visualizer_pcm_pos;
    memcpy(local_pcm, (void *)audio_visualizer_pcm, sizeof(local_pcm));
    OS_EXIT_CRITICAL();

    /* Derive baseline normalization factor */
    baseline = audio_visualizer_baseline_level;
    if (baseline < 64) {
        baseline = 64;
    }
    baseline_gain_q8 = ((u32)AUDIO_VISUALIZER_BASELINE_TARGET << 8) / baseline;
    if (baseline_gain_q8 < AUDIO_VISUALIZER_BASELINE_GAIN_MIN) {
        baseline_gain_q8 = AUDIO_VISUALIZER_BASELINE_GAIN_MIN;
    }
    if (baseline_gain_q8 > AUDIO_VISUALIZER_BASELINE_GAIN_MAX) {
        baseline_gain_q8 = AUDIO_VISUALIZER_BASELINE_GAIN_MAX;
    }

    /* Build the windowed + baseline-normalized snapshot directly into the FFT input */
    for (sample_index = 0; sample_index < AUDIO_VISUALIZER_SAMPLE_COUNT; sample_index++) {
        s32 raw = (s32)local_pcm[(u8)(local_start + sample_index)];
        s32 windowed = (raw * (s32)audio_visualizer_window[sample_index]) >> 14;
        normalized = (windowed * (s32)baseline_gain_q8) >> 8;

        if (normalized > 32767) {
            normalized = 32767;
        } else if (normalized < -32768) {
            normalized = -32768;
        }
        audio_visualizer_fft_re[sample_index] = (s16)normalized;
        audio_visualizer_fft_im[sample_index] = 0;
    }

    audio_visualizer_fft256(audio_visualizer_fft_re, audio_visualizer_fft_im);

    fs = audio_current_sample_rate;
    if (fs < 8000) {
        fs = 44100;
    }

    for (band = 0; band < count; band++) {
        /*
         * Dynamic Bin Mapping: Calculate the precise FFT bin for the target frequency
         * based on the active sample rate.
         * bin = (f_target * 512) / fs
         */
        u32 bin32 = ((u32)target_freqs[band] * 512) / fs;
        if (bin32 < 1) {
            bin32 = 1;
        }
        if (bin32 > 127) {
            bin32 = 127;
        }
        u8 bin = (u8)bin32;

        magnitude = (s32)audio_visualizer_magnitude(audio_visualizer_fft_re[bin], audio_visualizer_fft_im[bin]);

        /* Auto-gain peak tracking per band */
        peak = audio_visualizer_band_peak[band];
        if ((u16)magnitude > peak) {
            peak = (u16)magnitude;
        } else {
            peak -= (peak >> 8); /* Decay path */
            if (peak < 64) {
                peak = 64;
            }
        }
        audio_visualizer_band_peak[band] = peak;

        /* Map calculated magnitude to maximum display height */
        magnitude = (magnitude * AUDIO_VISUALIZER_MAX_BAR_HEIGHT) / peak;

        /* Apply structural gain tweaks */
        magnitude = (magnitude * audio_visualizer_band_gain_pct[band]) / 100;

        if (magnitude > AUDIO_VISUALIZER_MAX_BAR_HEIGHT) {
            bands[band] = AUDIO_VISUALIZER_MAX_BAR_HEIGHT;
        } else {
            bands[band] = (u8)magnitude;
        }
    }
}

static void audio_stream_clear(AUDIO_STREAM_DAC *obj)
{
    /* AUDIO_STREAM_DAC *obj = (AUDIO_STREAM_DAC *)priv; */
    cbuf_clear(&(obj->cbuf));
}

static tbool audio_stream_check(AUDIO_STREAM_DAC *obj)
{
    /* AUDIO_STREAM_DAC *obj = (AUDIO_STREAM_DAC *)priv; */
    if (obj->flag) {
        if (cbuf_get_space(&(obj->cbuf)) - cbuf_get_data_size(&(obj->cbuf)) > DAC_BUF_LEN) {
            return true;
        }
    }

    return false;
}

static u32 audio_stream_free_len(AUDIO_STREAM_DAC *obj)
{
    return (cbuf_get_space(&(obj->cbuf)) - cbuf_get_data_size(&(obj->cbuf)));
}

static u32 audio_stream_data_len(AUDIO_STREAM_DAC *obj)
{
    return cbuf_get_data_size(&(obj->cbuf));
}

static void audio_stream_samplerate(AUDIO_STREAM_DAC *obj, u16 sr, u8 wait)
{
    /* AUDIO_STREAM_DAC *obj = (AUDIO_STREAM_DAC *)priv; */
    dac_set_samplerate(sr, wait);
    audio_current_sample_rate = sr; /* Capture the sample rate dynamically */
}

AT_AUDIO
static u32 audio_stream_read(AUDIO_STREAM_DAC *obj, void *buf, u32 len)
{
    u8 is_tws_working = 0;
    /* AUDIO_STREAM_DAC *obj = (AUDIO_STREAM_DAC *)priv; */
    if (obj == NULL) {
        return 0;
    }
    u32 read_len = 0;

#if (BT_TWS&BT_TWS_TRANSMIT||BT_TWS&BT_TWS_BROADCAST)
    is_tws_working = tws_get_conn_working();
#endif

    if (obj->flag || is_tws_working) { //data enough
        if (cbuf_get_data_size(&(obj->cbuf)) >= len) {
            cbuf_read(&(obj->cbuf), buf, len);
            read_len = len;
        } else {
            /*
            if(0 == get_vm_statu()){
               putchar('#');
            }
            */
            memset(buf, 0x0, len);
            if (cbuf_get_data_size(&(obj->cbuf))) {
                read_len = cbuf_get_data_size(&(obj->cbuf));
                cbuf_read(&(obj->cbuf), buf, read_len);
            }
#if (BT_TWS&BT_TWS_TRANSMIT||BT_TWS&BT_TWS_BROADCAST)
            tws_set_dac_ide();
#endif

            obj->flag = 0;
        }
    }
    if ((0 == get_vm_statu()) && (cbuf_is_write_able(&(obj->cbuf), len) >= output_buf_limit_val)) {

        music_decoder_loop_resume();
    }

    return read_len;
}

static tbool audio_stream_creat(AUDIO_STREAM_DAC *obj)
{
    if (obj == NULL) {
        return false;
    }

    if (obj->buf == NULL) {
        return false;
    }

    cbuf_init(&(obj->cbuf), obj->buf, obj->buf_size);
    obj->flag = 0;

    return true;
}

void audio_stream_destroy(AUDIO_STREAM_DAC *obj)
{

}

AUDIO_STREAM_DAC dac_stream = {
    .buf = 	sound_cbuffer,
    .buf_size = OUTPUT_BUF_SIZE,
};

const AUDIO_STREAM audio_stream_io = {
    .priv 		= &dac_stream,
    .output		= (void *)audio_stream_write,
    .clear 		= (void *)audio_stream_clear,
    .check 		= (void *)audio_stream_check,
    .free_len	= (void *)audio_stream_free_len,
    .data_len 	= (void *)audio_stream_data_len,
    .set_sr 	= (void *)audio_stream_samplerate,
};

u16 get_audio_stream_total_len()
{
    AUDIO_STREAM_DAC *obj = audio_stream_io.priv;
    if (obj) {
        return  cbuf_get_space(&(obj->cbuf));
    }
    return 0;

}
u16 get_audio_stream_dac_len()
{
    if (audio_stream_io.priv) {
        return  audio_stream_io.data_len(audio_stream_io.priv);
    }
    return 0;
}
void clear_audio_stream_clear()
{
    audio_stream_clear(audio_stream_io.priv);
}

REGISTER_SOUND_DRIVERV(dac_drv);
static void audio_init(void)
{

    sound_automute_set(AUTO_MUTE_CFG, 4, 1200, 200);//max_cnt已扩展到u16

#if SYS_DEFAULT_VOL
    sound.vol.sys_vol_l = SYS_DEFAULT_VOL;
#else
    if (vm_read(VM_SYS_VOL, (void *)&sound.vol.sys_vol_l, VM_SYS_VOL_LEN) == VM_SYS_VOL_LEN) {
        log_printf("mem_vol:%d\n", sound.vol.sys_vol_l);
        /* limit system vol */
        if (sound.vol.sys_vol_l < 10) {
            sound.vol.sys_vol_l = 10;
        }
        if (sound.vol.sys_vol_l > MAX_SYS_VOL_L) {
            sound.vol.sys_vol_l = MAX_SYS_VOL_L;
        }
    } else {
        sound.vol.sys_vol_l = 20;
        log_printf("default_vol:%d\n", sound.vol.sys_vol_l);
    }
#endif
    sound.eq_mode   			= 0;/*如果eq有记忆，这里应该读取记忆eq出来赋值*/
    sound.vol.sys_vol_r    		= sound.vol.sys_vol_l;
    sound.vol.digital_vol_l 	= MAX_DIGITAL_VOL_L;
    sound.vol.digital_vol_r 	= MAX_DIGITAL_VOL_R;
    sound.vol.max_sys_vol_l 	= MAX_SYS_VOL_L;
    sound.vol.max_sys_vol_r 	= MAX_SYS_VOL_R;
    sound.vol.max_digit_vol_l 	= MAX_DIGITAL_VOL_L;
    sound.vol.max_digit_vol_r 	= MAX_DIGITAL_VOL_R;
    sound_buf_init(sound_cbuffer, sizeof(sound_cbuffer));

    digital_vol_init(&sound.vol);
    sin_tone_init();
    if (sound_drv && (strcmp(sound_drv->name, DAC_DRV_NAME) == 0)) {
        dac_init();
    }

    ladc_init();

    audio_stream_creat(&dac_stream);
}
no_sequence_initcall(audio_init);

void sound_automute_set(u8 ctl, u16 packet_cnt, u16 Threshold, u16 max_cnt)
{
#if DAC_AUTO_MUTE_EN
    if (sys_global_value.fast_test_flag == 0x1A) {
        ctl = AUTO_MUTE_DISABLE;
    }
    if ((u8) - 1 != ctl) {
        sound.automute_ctl = ctl;
    }
    /* dac_automute_init(ctl, packet_cnt, Threshold, max_cnt); */
    dac_automute_param_init(ctl, packet_cnt, Threshold, max_cnt);//by wuxu 20190729
#endif
}


//*****************************************************************************
//
//            				SOUND Buffer Operates
//
//*****************************************************************************
void sound_buf_init(void *cbuf, u32 cbuf_size)
{
    if (cbuf) {
        cbuf_init(&sound.cbuf, cbuf, cbuf_size);
    }
}

u32 get_sound_cbuf_len(void)
{
    return sound.cbuf.data_len;
}

AT_AUDIO
u32 audio_read(void *buf, u32 len)
{
    return audio_stream_read(&dac_stream, buf, len);
}


AT_AUDIO
void audio_read_vm(void *buf, u8 flag)
{
    AUDIO_STREAM_DAC *obj = &dac_stream;
    u32 len = DAC_BUF_LEN;

    /* AUDIO_STREAM_DAC *obj = (AUDIO_STREAM_DAC *)priv; */
    if (obj == NULL) {
        return;
    }
    u32 read_len = 0;
    if (obj->flag) { //data enough
        if (cbuf_get_data_size(&(obj->cbuf)) >= len) {
            cbuf_read(&(obj->cbuf), buf, len);
            read_len = len;
        } else {
            memset(buf, 0x0, len);
            if (cbuf_get_data_size(&(obj->cbuf))) {
                read_len = cbuf_get_data_size(&(obj->cbuf));
                cbuf_read(&(obj->cbuf), buf, read_len);
            }
            obj->flag = 0;
        }
    }

    if (read_len == 0) {
        memset(buf, 0x00, len);
    }
}

typedef struct __AD_TO_DA_STREAM {
    cbuffer_t 	cbuf;
    u8 			ram[DAC_BUF_LEN];		//double buffer
    u8 			enable;
} AD_TO_DA_STREAM;

static AD_TO_DA_STREAM ad2da_obj;

void ad2da_init(void)
{
    ad2da_obj.enable = 1;
    cbuf_init(&ad2da_obj.cbuf, ad2da_obj.ram, sizeof(ad2da_obj.ram));
}

s32 ad2da_write(u8 *buf, u32 len)
{
    s32 ret;
    if (ad2da_obj.enable == 0) {
        ret = 0;
    } else {
        ret = cbuf_write(&ad2da_obj.cbuf, buf, len);
        if (len != ret) {
            /* putchar('x'); */
        }
    }
    return ret;
}

s32 ad2da_read(u8 *buf, u32 len)
{
    s32 ret;
    if (ad2da_obj.enable == 0) {
        ret = 0;
    } else {
        ret = cbuf_read(&ad2da_obj.cbuf, buf, len);
        if (len != ret) {
            /* putchar('s'); */
        }
    }
    return ret;
}
