# UI, Bluetooth & spectrum

MagicJL adds an animated boot screen, a loading ring, Bluetooth connection screens, call/music icons, AVRCP metadata and a system info screen.

## AVRCP now-playing

During Bluetooth playback, the AVRCP title and artist are captured (up to 48 characters). They scroll one after the other in a custom pixel-font marquee. If the phone sends no metadata, the connected device name is shown instead.

## FFT spectrum

PCM is sampled just before the DAC. A 256-point Q15 FFT with a Hann window runs on the LCD refresh path, so no full FFT ever runs inside the audio interrupt. The result is drawn as 10 log-spaced bands (60 Hz – 9 kHz) with smoothing, per-band auto-gain and peak-hold.

Bar height:

```c
height_limit = 16 + sound.vol.sys_vol_l;
if (height_limit > 48) {
    height_limit = 48;
}
```

This lives in `apps/cpu/ui/lcd/lcd_disp.c`. Per-band display gains are in the `audio_visualizer_band_gain_pct` table in `apps/cpu/audio/audio.c`. These gains only affect the display, not the sound.
