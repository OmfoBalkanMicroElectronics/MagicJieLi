#include "sdk_cfg.h"
#include "common/common.h"
#include "ui/lcd/lcd_drv_api.h"
#include "ui/lcd/lcd_spi.h"
#include "ui/lcd/lcd_disp.h"
#include "ui/lcd/lcd_drv_interface.h"
#include "uicon/ui_con.h"
#include "uicon/ui.h"
#include "ui/ui_api.h"
#include "music_ui.h"
#include "bt_ui.h"
#include "ui/ui_common.h"
#include "fm_ui.h"
#include "timer.h"
#include "key_drv/key.h"
#include "file_operate/file_op.h"
#include "dac.h"
#include "audio/audio.h"
#include "rtc_setting.h"
#include "fmtx_api.h"
#include "eq.h"
#include "task_manager.h"
#include "power.h"
#include "task_fm.h"
#include "lyrics_api.h"
#include "encode/encode.h"
#include "rec_api.h"

#define MUSIC_UI_FILENAME_TEXTSIZE  14
#define MUSIC_UI_FOLDER_TEXTSIZE    14

#if  LCD_128X64_EN

u16 music_ui_filename_pt;
u16 music_ui_foldername_pt;

static const _lcd_area_size_t time_set_backgroup[] = {
    //left,top,right,bottom

    //DVcTime1_5
    {19, 3, 55, 4},
    {64, 3, 82, 4},
    {91, 3, 109, 4},

    //DVcTime2_5
    {41, 5, 59, 6},
    {68, 5, 86, 6},

    //DVcRzImg1_5
    {90, 5, 106, 6},
};

static const GUI_POINT  del_pic_point = {0, 32};
static const GUI_POINT  pause_pic_point = {16, 32};
static const GUI_POINT  vol_pic_point = {0, 0};
static const GUI_POINT  mute_pic_point = {112, 0};
//static const GUI_POINT  echo_pic_point = {80,0};
static const GUI_POINT  eq_pic_point = {110, 0};
static const GUI_POINT  playmode_pic_point = {96, 0};
static const GUI_POINT  rec_opt_pic_point = {32, 48};

static const u8 play_mode_pic[] = {DVcRzImg1_6, DVcRzImg15_6, DVcRzImg3_6, DVcRzImg4_6, DVcRzImg2_6};
static const u8 eq_mode_pic[] = {DVcRzImg5_6, DVcRzImg6_6, DVcRzImg7_6, DVcRzImg8_6, DVcRzImg9_6, DVcRzImg10_6};
static const u8 rec_opt_pic[] = {DVcRzImg4_8, DVcRzImg3_8, DVcRzImg2_8, DVcRzImg4_8};
static const u8 mode_pic[] = {DVcRzImg4_3, DVcRzImg5_3, DVcRzImg6_3}; //SD/U/bt
extern const LCD_INFO lcd_info;
static volatile u8 loading_icon_frame;
static u8 loading_animation_tick;
static u8 music_visualizer_active;
static u8 music_visualizer_idle_ticks;
static u8 music_visualizer_animation_tick;
static u8 music_visualizer_bar_height[10];
static u8 music_visualizer_peak_height[10];
static u8 system_info_active;
static u8 system_info_page;
static u8 system_info_tick;
static const char boot_welcome_text[] = "   Hosgeldiniz!";
static volatile u8 boot_text_char_count = 3;
static u8 boot_animation_tick;
static u8 boot_animation_complete_tick;
static const char boot_credit_text[] = "By OBM(BR4)";
static u8 boot_credit_phase;
static s16 boot_credit_x = -66;

static const u8 boot_font_blank[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u8 boot_font_b[]     = {0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e};
static const u8 boot_font_y[]     = {0x00, 0x11, 0x11, 0x0f, 0x01, 0x11, 0x0e};
static const u8 boot_font_o[]     = {0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e};
static const u8 boot_font_m[]     = {0x11, 0x1b, 0x15, 0x15, 0x11, 0x11, 0x11};
static const u8 boot_font_lparen[] = {0x06, 0x08, 0x10, 0x10, 0x10, 0x08, 0x06};
static const u8 boot_font_r[]     = {0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11};
static const u8 boot_font_4[]     = {0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02};
static const u8 boot_font_rparen[] = {0x0c, 0x02, 0x01, 0x01, 0x01, 0x02, 0x0c};

static const u8 *lcd_boot_credit_glyph(char c)
{
    switch (c) {
    case 'B': return boot_font_b;
    case 'y': return boot_font_y;
    case 'O': return boot_font_o;
    case 'M': return boot_font_m;
    case '(': return boot_font_lparen;
    case 'R': return boot_font_r;
    case '4': return boot_font_4;
    case ')': return boot_font_rparen;
    default:  return boot_font_blank;
    }
}

/* Draw one pixel into the active 128x64 OLED framebuffer. */
static void lcd_set_pixel(s16 x, s16 y)
{
    if ((x < 0) || (x >= LCDCOLUMN) || (y < 0) || (y >= LCDPAGE * 8)) {
        return;
    }
    lcd_info.lcd_buff[(y >> 3) * LCDCOLUMN + x] |= BIT(y & 0x07);
}

/* Small italic-style 5x7 font, positioned below the welcome text. */
static void lcd_draw_boot_credit(s16 start_x)
{
    const u8 *glyph;
    u8 letter;
    u8 row;
    u8 column;
    s16 x = start_x;

    for (letter = 0; boot_credit_text[letter] != '\0'; letter++) {
        glyph = lcd_boot_credit_glyph(boot_credit_text[letter]);
        for (row = 0; row < 7; row++) {
            for (column = 0; column < 5; column++) {
                if (glyph[row] & BIT(4 - column)) {
                    lcd_set_pixel(x + column + ((6 - row) >> 1), 43 + row);
                }
            }
        }
        x += 6;
    }
}

/* The UI loop runs every 50 ms; advance the spinner every 150 ms. */
static void lcd_loading_animation_loop(void)
{
    if (UI_var.bCurMenu != MENU_WAIT) {
        loading_animation_tick = 0;
        return;
    }

    if (++loading_animation_tick >= 1) {
        loading_animation_tick = 0;
        loading_icon_frame = (loading_icon_frame + 1) % 12;
        UI_REFRESH(MENU_REFRESH);
    }
}

LOOP_DETECT_REGISTER(lcd_loading_animation_detect) = {
    .time = 25,
    .fun  = lcd_loading_animation_loop,
};

static void lcd_draw_music_visualizer(void)
{
    u8 bar;
    u8 x;
    u8 y;
    u8 height;
    u8 target_height;
    u8 peak_height;
    u8 spectrum[10];
    u8 fall_step;
    u8 peak_gap;
    u8 height_limit;

    audio_visualizer_get_spectrum(spectrum, sizeof(spectrum));
    /* Keep the top 12 pixels free for Bluetooth AVRCP metadata.  The
     * per-band AGC is intentionally lively, so system volume also limits
     * the visible peak instead of letting quiet playback fill the screen. */
    height_limit = 16 + sound.vol.sys_vol_l;
    if (height_limit > 48) {
        height_limit = 48;
    }

    for (bar = 0; bar < 10; bar++) {
        target_height = spectrum[bar];
        if (target_height > height_limit) {
            target_height = height_limit;
        }
        if (target_height > music_visualizer_bar_height[bar]) {
            music_visualizer_bar_height[bar] += (target_height - music_visualizer_bar_height[bar] + 1) >> 1;
        } else if (music_visualizer_bar_height[bar] > target_height) {
            /* Fast, proportional fall */
            fall_step = ((music_visualizer_bar_height[bar] - target_height) >> 1) + 1;
            music_visualizer_bar_height[bar] -= fall_step;
        }
        height = music_visualizer_bar_height[bar];
        if (height >= music_visualizer_peak_height[bar]) {
            music_visualizer_peak_height[bar] = height;
        }

        for (x = 0; x < 8; x++) {
            for (y = 0; y < height; y++) {
                lcd_set_pixel(5 + bar * 12 + x, 61 - y);
            }
        }

        peak_height = music_visualizer_peak_height[bar];
        if (peak_height) {
            for (x = 0; x < 8; x++) {
                lcd_set_pixel(5 + bar * 12 + x, 60 - peak_height);
            }
        }

        /* Peak-hold dot decay */
        if (music_visualizer_peak_height[bar] > music_visualizer_bar_height[bar]) {
            peak_gap = music_visualizer_peak_height[bar] - music_visualizer_bar_height[bar];
            if (peak_gap > 2) {
                music_visualizer_peak_height[bar] -= 2;
            } else {
                music_visualizer_peak_height[bar] = music_visualizer_bar_height[bar];
            }
        }
    }
}

/* Smoothly advance only while the alternate Music screen is visible. */
static void lcd_music_visualizer_animation_loop(void)
{
    if (!music_visualizer_active) {
        music_visualizer_animation_tick = 0;
        return;
    }

    if (++music_visualizer_animation_tick >= 1) {
        music_visualizer_animation_tick = 0;
        UI_REFRESH(MENU_REFRESH);
    }
}

LOOP_DETECT_REGISTER(lcd_music_visualizer_animation_detect) = {
    .time = 25,
    .fun  = lcd_music_visualizer_animation_loop,
};

void lcd_music_visualizer_tick(u8 playing)
{
    if (!playing || ((UI_var.bCurMenu != MENU_MUSIC_MAIN) && (UI_var.bCurMenu != MENU_BT_MAIN))) {
        music_visualizer_active = 0;
        music_visualizer_idle_ticks = 0;
        memset(music_visualizer_bar_height, 0, sizeof(music_visualizer_bar_height));
        memset(music_visualizer_peak_height, 0, sizeof(music_visualizer_peak_height));
        return;
    }

    if (!music_visualizer_active && (++music_visualizer_idle_ticks >= 10)) {
        music_visualizer_active = 1;
        memset(music_visualizer_bar_height, 0, sizeof(music_visualizer_bar_height));
        memset(music_visualizer_peak_height, 0, sizeof(music_visualizer_peak_height));
        UI_DIS_MAIN();
    }
}

void lcd_music_visualizer_activity(void)
{
    music_visualizer_idle_ticks = 0;
    if (music_visualizer_active) {
        music_visualizer_active = 0;
        memset(music_visualizer_bar_height, 0, sizeof(music_visualizer_bar_height));
        memset(music_visualizer_peak_height, 0, sizeof(music_visualizer_peak_height));
        UI_DIS_MAIN();
    }
}

/* Type one character every 100 ms while the boot screen is active. */
static void lcd_boot_animation_loop(void)
{
    if (UI_var.bCurMenu != MENU_POWER_UP) {
        boot_text_char_count = 3;
        boot_animation_tick = 0;
        boot_animation_complete_tick = 0;
        boot_credit_phase = 0;
        boot_credit_x = -66;
        return;
    }

    if (boot_text_char_count >= sizeof(boot_welcome_text) - 1) {
        if (boot_credit_phase == 0) {
            /* Pause briefly after the greeting, then start the credit slide. */
            if (++boot_animation_complete_tick >= 6) {
                boot_animation_complete_tick = 0;
                boot_credit_phase = 1;
                UI_REFRESH(MENU_REFRESH);
            }
        } else if (boot_credit_phase == 1) {
            boot_credit_x += 4;
            if (boot_credit_x >= 31) {
                boot_credit_x = 31;
                boot_credit_phase = 2;
                boot_animation_complete_tick = 0;
            }
            UI_REFRESH(MENU_REFRESH);
        } else if (++boot_animation_complete_tick >= 10) {
            ui_boot_complete();
        }
        return;
    }

    if (++boot_animation_tick >= 2) {
        boot_animation_tick = 0;
        boot_text_char_count++;
        UI_REFRESH(MENU_REFRESH);
    }
}

LOOP_DETECT_REGISTER(lcd_boot_animation_detect) = {
    .time = 25,
    .fun  = lcd_boot_animation_loop,
};

/* Draw a small 12-dot loading ring directly into the 128x64 framebuffer. */
static void lcd_draw_loading_icon(void)
{
    static const u8 dot_xy[][2] = {
        {6, 0}, {9, 1}, {11, 4}, {12, 7}, {11, 10}, {9, 12},
        {6, 13}, {3, 12}, {1, 10}, {0, 7}, {1, 4}, {3, 1},
    };
    u8 i;
    u8 dx;
    u8 dy;
    u8 x;
    u8 y;
    u8 dot_size;

    for (i = 0; i < sizeof(dot_xy) / sizeof(dot_xy[0]); i++) {
        dot_size = (i == loading_icon_frame) ? 3 : 1;
        for (dy = 0; dy < dot_size; dy++) {
            for (dx = 0; dx < dot_size; dx++) {
                x = 58 + dot_xy[i][0] + dx;
                y = 2 + dot_xy[i][1] + dy;
                lcd_info.lcd_buff[(y >> 3) * LCDCOLUMN + x] |= BIT(y & 0x07);
            }
        }
    }
}

///------------common-api--------------
void lcd_setX(u8 X)
{

}

int unicode_len(u8 *str)
{
    u8 *src = str;
    while ((*str != 0) || (*(str + 1) != 0)) {
        str += 2;
    }
    return (int)(str - src);
}

void lcd_set_cur_menu(u8 menu)
{
    UI_var.bMenuReturnCnt = 0;
    UI_var.bCurMenu = menu;
}

u16 lcd_disp_text2(const char *str, u8 dvtxt_num, u8 flags)
{
    DTEXT disp_txt;
    u16 show_bytes;

    disp_txt.buf = (u8 *)str;
    disp_txt.flags = flags;

    if ((flags & ANSI) != 0) {
        disp_txt.len = strlen((const char *)disp_txt.buf);
    } else {
        disp_txt.len = unicode_len((u8 *)disp_txt.buf);
    }

    show_bytes = ui_text(dvtxt_num, &disp_txt);
    return show_bytes;
}

void lcd_disp_text(const char *str, u8 dvtxt_num)
{
    lcd_disp_text2(str, dvtxt_num, ANSI);
}

void lcd_disp_string_menu(u8 menu)
{
    const char loading_string[] = {"   Yukleniyor...  "};
    const char dev_err_string_Chinese[][17] = {
        "    DEVICE     ",
        "  NOT FOUND!!! ",
    };

    /* Keep boot and inter-task screens separate. */
    if (menu == MENU_POWER_UP) {
        lcd_disp_welcome();
        return;
    }

    if (menu == MENU_WAIT) {
        lcd_draw_loading_icon();
        lcd_disp_text(loading_string, DVcTxt1_2);
        return;
    }

#if RTC_ALM_EN
    if (menu == MENU_ALM_UP) {
        lcd_alarm_ring();
        return;
    }
#endif

    if (menu == MENU_DEV_ERR) {
        lcd_disp_text(dev_err_string_Chinese[0], DVcTxt2_1);
        lcd_disp_text(dev_err_string_Chinese[1], DVcTxt3_1);
        return;
    }
}

void lcd_null_dis(void)
{

}

int lcd_check_sys_vol(void)
{
#if 1
    if ((*(UI_var.sys_vol) == 0) || (is_dac_mute())) {
        ui_pic(DVcRzImg1_3, (void *)&mute_pic_point);
        return 1;
    } else {
        return 0;
    }
#endif
}

void  lcd_check_echo_sw(void)
{

}

void lcd_disp_freq(u16 freq, u8 font_size)
{
    char lcd_freq_str[9] = {"     MHz"};
    u32 freq_temp;

    if (font_size == 2) {
        itoa4(freq);
        if (freq <= 999) {
            lcd_freq_str[0] = bcd_number[1];
            lcd_freq_str[1] = bcd_number[2];
            lcd_freq_str[2] = '.';
            lcd_freq_str[3] = bcd_number[3];
        } else {
            lcd_freq_str[0] = bcd_number[0];
            lcd_freq_str[1] = bcd_number[1];
            lcd_freq_str[2] = bcd_number[2];
            lcd_freq_str[3] = '.';
            lcd_freq_str[4] = bcd_number[3];
        }
        lcd_disp_text(lcd_freq_str, DVcTxt1_3);
        ui_pic(DVcRzImg7_3, 0);
    } else if (font_size == 1) {
        ui_pic(DVcRzImg1_4, 0);

        if (freq <= 999) {
            freq_temp = freq % 1000 / 10;
        } else {
            freq_temp = freq / 10;
        }
        ui_number(DVcNumber1_4, (u32)freq_temp, -1, -1, 1);

        freq_temp = freq % 10;
        ui_number(DVcNumber2_4, (u32)freq_temp, -1, -1, 1);
    }
}

void lcd_disp_rec_error(void)
{
#if REC_EN
    const char lcd_string[] = {"  REC error!!!  "};
    lcd_clear();
    lcd_disp_text(lcd_string, DVcTxt1_2);
    lcd_set_cur_menu(MENU_RECODE_ERR);
#endif
}

void  lcd_disp_rec_opt_state(RECORD_OP_API *rec_api)
{
#if REC_EN
    u32 rec_sta = 0;
    rec_sta = rec_get_enc_sta(rec_api);
    ui_pic(DVcRzImg1_8, 0);
    ui_pic(rec_opt_pic[rec_sta], (GUI_POINT *)&rec_opt_pic_point);
#endif
}

///------------main-display--------------
void lcd_disp_welcome(void)
{
    char lcd_string[sizeof(boot_welcome_text) + 1];
    u8 visible_chars = boot_text_char_count;

    if (visible_chars > sizeof(boot_welcome_text) - 1) {
        visible_chars = sizeof(boot_welcome_text) - 1;
    }

    memcpy(lcd_string, boot_welcome_text, visible_chars);
    if (visible_chars < sizeof(boot_welcome_text) - 1) {
        lcd_string[visible_chars] = '_';
        lcd_string[visible_chars + 1] = '\0';
    } else {
        lcd_string[visible_chars] = '\0';
    }

    lcd_disp_text(lcd_string, DVcTxt1_2);

    if (boot_credit_phase) {
        lcd_draw_boot_credit(boot_credit_x);
    }
}

void lcd_disp_IR_number(s32 input_number)
{
    input_number &= 0xffff;
    ui_number(DVcNumber1_2, input_number, -1, -1, 0);
}

void lcd_disp_vol(s32 vol)
{
    ui_pic(DVcRzImg5_8, 0);
    ui_number(DVcNumber1_8, vol, -1, -1, 0);
}

void lcd_disp_power(void)
{
    u32 voltage;
    ui_pic(DVcRzImg1_5, 0);
    voltage = get_power_external_value() / 10;
    ui_number(DVcNumber1_5, (u32)voltage, -1, -1, 1);
    voltage = get_power_external_value() % 10;
    ui_number(DVcNumber2_5, (u32)voltage, -1, -1, 1);
}

void lcd_disp_dev(void *dev, u8 dvtxt_num)
{
    char lcd_dev_str[][4] = {
        "SD0",
        "SD1",
        "USB",
        "NON",
    };
    if (dev == sd0) {
        lcd_disp_text(lcd_dev_str[0], dvtxt_num);
    } else if (dev == sd1) {
        lcd_disp_text(lcd_dev_str[1], dvtxt_num);
    } else if (dev == usb) {
        lcd_disp_text(lcd_dev_str[2], dvtxt_num);
    } else {
        lcd_disp_text(lcd_dev_str[3], dvtxt_num);
    }
}

void lcd_disp_eq(s32 eq_mode)
{
    char eq_str[][17] = {
        "     NORMAL",
        "      ROCK",
        "       POP",
        "     KLASIK",
        "       CAZ",
        "      ULKE",
    };

    lcd_disp_text((char *)eq_str[eq_mode], DVcTxt3_1);
}

void lcd_disp_playmode(s32 play_mode)
{
    char playmode_str[][17] = {
        "      TUMU",
        "   TEK KAYNAK",
        "    TEK PARCA",
        "    KARISIK",
        "     KLASOR",
    };

    lcd_disp_text((char *)playmode_str[play_mode - REPEAT_ALL], DVcTxt3_1);
}

void lcd_disp_filenumber(void)
{
    MUSIC_DIS_VAR *music_var;

    music_var = (MUSIC_DIS_VAR *)UI_var.ui_buf_adr;
    if (music_var) {
        ui_number(DVcNumber1_2, music_var->ui_curr_file, -1, -1, 0);
    }
}

///-------------app-display---------------
int text_scroll_deal(u16 disp_len, u16 text_size, int pt)
{
    if ((disp_len <= text_size) || (disp_len < pt)) {
        return 0;
    }

    disp_len = disp_len - pt;
    if (text_size >= disp_len) {
        return 0;
    }

    return pt + 1;
}

int disp_unicode_len(u8 *str)
{
    int char_cnt = 0;
    u8 *src = str;
    while ((*str != 0) || (*(str + 1) != 0)) {
        if (*(str + 1) == 0) {
            char_cnt++;
        }
        str += 2;
    }
    return (int)(str - src) - char_cnt;
}

///------------music-display--------------
void lcd_music_main(void)
{
    char lcd_file_num[] = {" 0000/0000"};
    char lcd_playtime_str[] = {"00:00"};
    u8 flags;
    u32 tmp;
    u8 *point;

    const char lcd_string_Chinese[] = {
        "   Yukleniyor...  ",
    };

    MUSIC_DIS_VAR *music_var;
    music_var = (MUSIC_DIS_VAR *)UI_var.ui_buf_adr;

    if (music_var) {
        if (music_var->ui_total_file == 0) {
            lcd_disp_text(lcd_string_Chinese, DVcTxt1_2);
            return;
        }

        if (music_visualizer_active && (music_var->curr_statu == MUSIC_DECODER_ST_PLAY)) {
            lcd_draw_music_visualizer();
            return;
        }

#if FMTX_EN
        lcd_disp_freq(fmtx_get_freq(), 2);
#endif

        if ((music_var->ui_curr_device == sd0) || (music_var->ui_curr_device == sd1)) {
            ui_pic(mode_pic[0], NULL);
        } else if (music_var->ui_curr_device == usb) {
            ui_pic(mode_pic[1], NULL);
        }
        ui_pic(eq_mode_pic[music_var->eq_mode], (GUI_POINT *)&eq_pic_point);
        ui_pic(play_mode_pic[music_var->play_mode - REPEAT_ALL], (GUI_POINT *)&playmode_pic_point);

        if (music_var->ui_file_info->update_flag != 0) {
            music_ui_filename_pt = 0;
            music_ui_foldername_pt = 0;
            music_var->ui_file_info->update_flag = 0;
        }

        point = (u8 *)music_var->ui_file_info->file_name.lfn;
        tmp = music_var->ui_file_info->file_name.lfn_cnt;
        if (tmp > 0) {
            flags = UNICODE_LE;
            if (UI_var.param_menu == MENU_REFRESH) {
                if (disp_unicode_len(point) > MUSIC_UI_FILENAME_TEXTSIZE) {
                    music_ui_filename_pt = text_scroll_deal(tmp, MUSIC_UI_FILENAME_TEXTSIZE, music_ui_filename_pt);
                }
            }
        } else {
            flags = ANSI;
        }
        ui_pic(DVcRzImg3_3, 0);
        lcd_disp_text2((const char *)point + (music_ui_filename_pt & 0xfffe), DVcTxt2_3, flags);

        if ((music_var->opt_state & MUSIC_OPT_BIT_DEL_CONFIRM) != 0) {
            ui_pic(DVcRzImg4_7, (GUI_POINT *)&del_pic_point);
            return;
        }

        if ((music_var->opt_state & MUSIC_OPT_BIT_FF) != 0) {
            ui_pic(DVcRzImg2_7, (GUI_POINT *)&pause_pic_point);
            return;
        }

        if ((music_var->opt_state & MUSIC_OPT_BIT_FR) != 0) {
            ui_pic(DVcRzImg1_7, (GUI_POINT *)&pause_pic_point);
            return;
        }

        if ((music_var->opt_state & MUSIC_OPT_BIT_DEL) != 0) {
            music_var->opt_state &= ~MUSIC_OPT_BIT_DEL;
            ui_pic(DVcRzImg4_7, (GUI_POINT *)&del_pic_point);
            return;
        }

#if LRC_LYRICS_EN
        if ((MUSIC_DECODER_ST_PLAY == music_var->curr_statu)) {
            if (music_var->lrc_flag == true) {
                if (lrc_show_api(music_var->play_time, 0)) {
                    return;
                }
            }
        }
#endif

        if ((MUSIC_PLAYRR_ST_PAUSE == music_var->curr_statu)) {
            ui_pic(DVcRzImg3_7, (GUI_POINT *)&pause_pic_point);
            return;
        }

        itoa2_api(music_var->play_time / 60, (u8 *)&lcd_playtime_str[0]);
        itoa2_api(music_var->play_time % 60, (u8 *)&lcd_playtime_str[3]);
#if  POWER_EXTERN_DETECT_EN
        lcd_disp_text(lcd_playtime_str, DVcTxt1_5);
#else
        lcd_disp_text(lcd_playtime_str, DVcTxt3_3);
#endif

        itoa4_api(music_var->ui_curr_file, (u8 *)&lcd_file_num[1]);
        itoa4_api(music_var->ui_total_file, (u8 *)&lcd_file_num[6]);
#if POWER_EXTERN_DETECT_EN
        lcd_disp_text(lcd_file_num, DVcTxt2_5);
#else
        lcd_disp_text(lcd_file_num, DVcTxt4_3);
#endif

#if POWER_EXTERN_DETECT_EN
        lcd_disp_power();
#endif
#if 1
        point = (u8 *)music_var->ui_file_info->dir_name.lfn;
        tmp = music_var->ui_file_info->dir_name.lfn_cnt;
        if (tmp > 0) {
            flags = UNICODE_LE;
            if (UI_var.param_menu == MENU_REFRESH) {
                if (disp_unicode_len(point) > MUSIC_UI_FOLDER_TEXTSIZE) {
                    music_ui_foldername_pt = text_scroll_deal(tmp, MUSIC_UI_FOLDER_TEXTSIZE, music_ui_foldername_pt);
                }
            }
        } else {
            flags = ANSI;
        }

        ui_pic(DVcRzImg2_3, 0);
        lcd_disp_text2((const char *)point + (music_ui_foldername_pt & 0xfffe), DVcTxt5_3, flags);
#endif
    }
}

///------------bt-display--------------
static u8 bt_status_card_last = 0xff;
static u8 bt_status_animation_frame = 3;
static u8 bt_status_animation_tick;
static u8 bt_top_icon_frame;
static u8 bt_top_icon_tick;
static u8 bt_music_name_show;
static u8 bt_music_text_tick;
static u8 bt_spectrum_artist_show;
static u8 bt_spectrum_metadata_tick;
static s16 bt_spectrum_marquee_x = 128;
static u8 bt_spectrum_marquee_tick;

/* Compact pixel font: it avoids the SDK text control, whose marquee resets
 * every time the spectrum redraws the framebuffer. */
static const u8 bt_marquee_font[36][5] = {
    {2,5,7,5,5},{6,5,6,5,6},{3,4,4,4,3},{6,5,5,5,6},
    {7,4,6,4,7},{7,4,6,4,4},{3,4,5,5,3},{5,5,7,5,5},
    {7,2,2,2,7},{1,1,1,5,2},{5,5,6,5,5},{4,4,4,4,7},
    {5,7,7,5,5},{5,7,7,7,5},{2,5,5,5,2},{6,5,6,4,4},
    {2,5,5,3,1},{6,5,6,5,5},{3,4,2,1,6},{7,2,2,2,2},
    {5,5,5,5,7},{5,5,5,5,2},{5,5,7,7,5},{5,5,2,5,5},
    {5,5,2,2,2},{7,1,2,4,7},
    {7,5,5,5,7},{2,6,2,2,7},{7,1,7,4,7},{7,1,3,1,7},
    {5,5,7,1,1},{7,4,7,1,7},{3,4,7,5,7},{7,1,2,2,2},
    {7,5,7,5,7},{7,5,7,1,6},
};

static const u8 *lcd_bt_marquee_glyph(char c)
{
    static const u8 punctuation[][5] = {
        {0,0,0,0,0}, {0,0,7,0,0}, {0,0,0,0,2}, {0,0,7,0,7},
    };

    if ((c >= 'a') && (c <= 'z')) {
        c -= 'a' - 'A';
    }
    if ((c >= 'A') && (c <= 'Z')) {
        return bt_marquee_font[c - 'A'];
    }
    if ((c >= '0') && (c <= '9')) {
        return bt_marquee_font[26 + c - '0'];
    }
    if (c == '-') {
        return punctuation[1];
    }
    if (c == '.') {
        return punctuation[2];
    }
    if (c == '_') {
        return punctuation[3];
    }
    if (c == '&') {
        static const u8 ampersand[] = {6,5,2,5,3};
        return ampersand;
    }
    return punctuation[0];
}

static const char *lcd_bt_spectrum_metadata_text(void)
{
    if (bt_spectrum_artist_show && ui_bt_get_track_artist()[0]) {
        return ui_bt_get_track_artist();
    }
    if (ui_bt_get_track_title()[0]) {
        return ui_bt_get_track_title();
    }
    return ui_bt_get_remote_name();
}

static void lcd_draw_bt_spectrum_metadata(void)
{
    const char *text;
    const u8 *glyph;
    u8 letter;
    u8 row;
    u8 column;
    s16 x;

    text = lcd_bt_spectrum_metadata_text();
    if ((text == NULL) || !text[0]) {
        return;
    }

    x = bt_spectrum_marquee_x;
    for (letter = 0; text[letter]; letter++) {
        glyph = lcd_bt_marquee_glyph(text[letter]);
        for (row = 0; row < 5; row++) {
            for (column = 0; column < 3; column++) {
                if (glyph[row] & BIT(2 - column)) {
                    lcd_set_pixel(x + column, 2 + row);
                }
            }
        }
        x += 4;
    }
}

static u8 lcd_bt_connected_state(u8 status)
{
    return (status == BT_STATUS_CONNECTING) ||
           (status == BT_STATUS_TAKEING_PHONE) ||
           (status == BT_STATUS_PLAYING_MUSIC);
}

static u8 lcd_bt_status_icon_x(u8 icon_width)
{
    return ((128 - icon_width) / 2) + ((3 - bt_status_animation_frame) * 4);
}

static void lcd_draw_bt_waiting_ring(void)
{
    static const u8 dot_xy[][2] = {
        {64, 2}, {68, 3}, {70, 6}, {71, 10}, {70, 14}, {68, 17},
        {64, 18}, {60, 17}, {58, 14}, {57, 10}, {58, 6}, {60, 3},
    };
    u8 i;
    u8 dx;
    u8 dy;
    u8 dot_size;

    for (i = 0; i < sizeof(dot_xy) / sizeof(dot_xy[0]); i++) {
        dot_size = (i == bt_top_icon_frame) ? 3 : 1;
        for (dy = 0; dy < dot_size; dy++) {
            for (dx = 0; dx < dot_size; dx++) {
                lcd_set_pixel(dot_xy[i][0] + dx, dot_xy[i][1] + dy);
            }
        }
    }
}

static void lcd_draw_bt_connected_tick(void)
{
    u8 i;

    for (i = 0; i < 5; i++) {
        lcd_set_pixel(56 + i, 12 + i);
        lcd_set_pixel(57 + i, 12 + i);
    }
    for (i = 0; i < 9; i++) {
        lcd_set_pixel(60 + i, 16 - i);
        lcd_set_pixel(61 + i, 16 - i);
    }
}

static void lcd_draw_bt_music_notes(void)
{
    u8 i;

    for (i = 0; i < 11; i++) {
        lcd_set_pixel(59 + i, 6);
    }
    for (i = 0; i < 10; i++) {
        lcd_set_pixel(60, 7 + i);
        lcd_set_pixel(69, 7 + i);
    }
    for (i = 0; i < 4; i++) {
        lcd_set_pixel(57 + i, 16);
        lcd_set_pixel(58 + i, 17);
        lcd_set_pixel(66 + i, 15);
        lcd_set_pixel(67 + i, 16);
    }
}

static void lcd_draw_bt_phone(void)
{
    static const u16 phone_bitmap[] = {
        0x0380, 0x0780, 0x0c80, 0x0c80, 0x04c0, 0x0380,
        0x01c0, 0x0064, 0x0026, 0x0026, 0x003c, 0x001c,
    };
    u8 row;
    u8 column;

    for (row = 0; row < sizeof(phone_bitmap) / sizeof(phone_bitmap[0]); row++) {
        for (column = 0; column < 12; column++) {
            if (phone_bitmap[row] & BIT(11 - column)) {
                lcd_set_pixel(58 + column, 4 + row);
            }
        }
    }
}

static void lcd_draw_bt_top_status(u8 status)
{
    if (status == BT_STATUS_TAKEING_PHONE) {
        lcd_draw_bt_phone();
    } else if (status == BT_STATUS_PLAYING_MUSIC) {
        lcd_draw_bt_music_notes();
    } else if (status == BT_STATUS_CONNECTING) {
        lcd_draw_bt_connected_tick();
    } else {
        lcd_draw_bt_waiting_ring();
    }
}

void lcd_system_info_show(void)
{
    system_info_active = 1;
    system_info_page = 0;
    system_info_tick = 0;
    UI_DIS_MAIN();
}

u8 lcd_system_info_is_active(void)
{
    return system_info_active;
}

void lcd_system_info_draw(void)
{
    static const char info_text[][3][17] = {
        {"    MAGICJL", " DEVELOPED BY", "   OBM TEAM"},
        {"  DEVELOPER /", "  MAINTAINER:", "  BARBAROR4"},
        {" SW: MJL_BT201", "  RELEASE V1.0", "  192MHZ PI32V2"},
        {"   128KB SRAM", "  512KB FLASH", "AC692 UNLOCKED ;)"},
    };

    lcd_clear();
    lcd_disp_text(info_text[system_info_page][0], DVcTxt1_1);
    lcd_disp_text(info_text[system_info_page][1], DVcTxt2_1);
    lcd_disp_text(info_text[system_info_page][2], DVcTxt3_1);
}

static void lcd_system_info_loop(void)
{
    if (!system_info_active) {
        return;
    }

    if (++system_info_tick >= 80) { /* two seconds per page */
        system_info_tick = 0;
        if (++system_info_page >= 4) {
            system_info_active = 0;
        }
        UI_DIS_MAIN();
    }
}

LOOP_DETECT_REGISTER(lcd_system_info_detect) = {
    .time = 25,
    .fun  = lcd_system_info_loop,
};

/* Keep the normal Bluetooth status card alive independently of the spectrum. */
static void lcd_bt_status_animation_loop(void)
{
    BT_DIS_VAR *bt_var;
    u8 refresh = 0;

    if (UI_var.bCurMenu != MENU_BT_MAIN) {
        bt_status_card_last = 0xff;
        bt_status_animation_frame = 3;
        bt_status_animation_tick = 0;
        bt_top_icon_frame = 0;
        bt_top_icon_tick = 0;
        bt_music_name_show = 0;
        bt_music_text_tick = 0;
        bt_spectrum_artist_show = 0;
        bt_spectrum_metadata_tick = 0;
        bt_spectrum_marquee_x = 128;
        bt_spectrum_marquee_tick = 0;
        return;
    }

    bt_var = (BT_DIS_VAR *)UI_var.ui_buf_adr;
    if (bt_var == NULL) {
        return;
    }

    if (!lcd_bt_connected_state(bt_var->ui_bt_connect_sta)) {
        if (++bt_top_icon_tick >= 5) {
            bt_top_icon_tick = 0;
            bt_top_icon_frame = (bt_top_icon_frame + 1) % 12;
            refresh = 1;
        }
    } else {
        bt_top_icon_frame = 0;
        bt_top_icon_tick = 0;
    }

    if ((bt_var->ui_bt_connect_sta == BT_STATUS_PLAYING_MUSIC) &&
        ui_bt_get_remote_name()[0]) {
        if (++bt_music_text_tick >= 40) {
            bt_music_text_tick = 0;
            bt_music_name_show = !bt_music_name_show;
            refresh = 1;
        }
    } else {
        bt_music_name_show = 0;
        bt_music_text_tick = 0;
    }

    if ((bt_var->ui_bt_connect_sta == BT_STATUS_PLAYING_MUSIC) &&
        (ui_bt_get_track_title()[0] || ui_bt_get_track_artist()[0])) {
        if (++bt_spectrum_metadata_tick >= 40) {
            bt_spectrum_metadata_tick = 0;
            bt_spectrum_artist_show = !bt_spectrum_artist_show;
            refresh = 1;
        }

        if (++bt_spectrum_marquee_tick >= 2) {
            bt_spectrum_marquee_tick = 0;
            bt_spectrum_marquee_x--;
            if (bt_spectrum_marquee_x < -(s16)(strlen(lcd_bt_spectrum_metadata_text()) * 4)) {
                bt_spectrum_marquee_x = 128;
            }
            refresh = 1;
        }
    } else {
        bt_spectrum_artist_show = 0;
        bt_spectrum_metadata_tick = 0;
        bt_spectrum_marquee_x = 128;
        bt_spectrum_marquee_tick = 0;
    }

    if (bt_status_animation_frame < 3) {
        if (++bt_status_animation_tick >= 2) {
            bt_status_animation_tick = 0;
            bt_status_animation_frame++;
            refresh = 1;
        }
    }

    if (refresh) {
        UI_REFRESH(MENU_REFRESH);
    }
}

LOOP_DETECT_REGISTER(lcd_bt_status_animation_detect) = {
    .time = 25,
    .fun  = lcd_bt_status_animation_loop,
};

void lcd_bt_main(u8 menu)
{
    BT_DIS_VAR *bt_var;
    GUI_POINT main_pic_point;
    char *lcd_string;
    u8 connected = 0; /* Initialized to zero to satisfy compiler warnings */

    bt_var = (BT_DIS_VAR *)UI_var.ui_buf_adr;

    const char lcd_string_Chinese[][20] = {
        " BT Bagli Degil",
        "    BT Baglandi   ",
        "     BT Muzik     ",
        "     BT ARAMA      ",
    };

    if (bt_var) {
        /* Direct rendering bypass for the spectrum analyzer in Bluetooth mode */
        if (music_visualizer_active && (bt_var->ui_bt_connect_sta == BT_STATUS_PLAYING_MUSIC)) {
            lcd_draw_music_visualizer();
            lcd_draw_bt_spectrum_metadata();
            return;
        }

        connected = lcd_bt_connected_state(bt_var->ui_bt_connect_sta);
        if (connected != bt_status_card_last) {
            bt_status_card_last = connected;
            bt_status_animation_frame = 0;
            bt_status_animation_tick = 0;
        }

        ui_pic(mode_pic[2], NULL);
        ui_pic(eq_mode_pic[bt_var->bt_eq_mode], (GUI_POINT *)&eq_pic_point);
        lcd_draw_bt_top_status(bt_var->ui_bt_connect_sta);

#if FMTX_EN
        lcd_disp_freq(fmtx_get_freq(), 2);
#endif

        if (connected) {
#if  POWER_EXTERN_DETECT_EN
            ui_pic(DVcRzImg2_2, 0);
#else
            main_pic_point.x = lcd_bt_status_icon_x(60);
            main_pic_point.y = 32;
            ui_pic(DVcRzImg2_2, &main_pic_point);
#endif
        } else {
#if  POWER_EXTERN_DETECT_EN
            ui_pic(DVcRzImg1_2, 0);
#else
            main_pic_point.x = lcd_bt_status_icon_x(42);
            main_pic_point.y = 32;
            ui_pic(DVcRzImg1_2, &main_pic_point);
#endif
        }

        if (BT_STATUS_CONNECTING == bt_var->ui_bt_connect_sta) {
            lcd_string = (char *)lcd_string_Chinese[1];
        } else if (BT_STATUS_PLAYING_MUSIC == bt_var->ui_bt_connect_sta) {
            if (bt_music_name_show && ui_bt_get_remote_name()[0]) {
                lcd_string = (char *)ui_bt_get_remote_name();
            } else {
                lcd_string = (char *)lcd_string_Chinese[2];
            }
        } else if (BT_STATUS_TAKEING_PHONE == bt_var->ui_bt_connect_sta) {
            lcd_string = (char *)lcd_string_Chinese[3];
        } else {
            lcd_string = (char *)lcd_string_Chinese[0];
        }

        lcd_disp_text(lcd_string, DVcTxt2_1);

#if POWER_EXTERN_DETECT_EN
        lcd_disp_power();
#endif
    }
}

///------------pc-display--------------
void lcd_pc_main(u8 menu)
{
#if USB_PC_EN
    const char lcd_string[] = {"Bilgisyr.(MagicJL)"};
    lcd_disp_text(lcd_string, DVcTxt1_1);
    ui_pic(DVcRzImg1_1, 0);
#endif
}

void lcd_echo_main(u8 menu)
{
#if ECHO_EN
    const char lcd_string[] = {"ECHO"};
    lcd_disp_text(lcd_string, DVcTxt1_1);
#endif
}

///------------fm-display--------------
extern u8 get_sys_halfsec(void);
void lcd_fmtx_main(void)
{
    if ((fmtx_get_whether_to_flash_freq() == 1) && get_sys_halfsec()) {

    } else {
        lcd_disp_freq(fmtx_get_freq(), 1);
    }
}

void lcd_fm_main(void)
{
#if FM_RADIO_EN
    const char lcd_string[] = {
        "     Radyo (pro)",
    };
    char lcd_freq_str[] = {"        87.5MHz "};

    lcd_disp_text(lcd_string, DVcTxt1_1);
    ui_pic(DVcRzImg7_3, 0);

    FM_MODE_VAR *fm_var;
    fm_var = *(FM_MODE_VAR **)UI_var.ui_buf_adr;

    if (fm_var) {
        if (fm_var->wFreChannel > 0) {
            itoa2((u8)fm_var->wFreChannel);
            lcd_freq_str[3] = 'C';
            lcd_freq_str[4] = bcd_number[0];
            lcd_freq_str[5] = bcd_number[1];
        }

        itoa4(fm_var->wFreq);

        if (fm_var->wFreq <= 999) {
            bcd_number[0] = ' ';
        }
        lcd_freq_str[7] = bcd_number[0];
        lcd_freq_str[8] = bcd_number[1];
        lcd_freq_str[9] = bcd_number[2];
        lcd_freq_str[11] = bcd_number[3];

        lcd_disp_text(lcd_freq_str, DVcTxt3_1);
    }
#endif
}

void lcd_fm_channel(void)
{

}

///------------rtc-display--------------
void lcd_rtc_main(void)
{
#if RTC_CLK_EN
    RTC_SETTING *rtc_var;
    RTC_TIME *ui_time_var;

    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;
    if (rtc_var == 0) {
        return;
    }

    if (rtc_var->rtc_set_mode == RTC_SET_MODE) {
        lcd_rtc_set();
        return;
    } else {
        if (rtc_var->rtc_set_mode == ALM_SET_MODE) {
            lcd_alarm_set();
            return;
        }
    }

    const char lcd_string[] = {
        "Saat",
    };
    lcd_disp_text(lcd_string, DVcTxt1_1);

    if (rtc_var) {
        ui_time_var = rtc_var->calendar_set.curr_rtc_time;
        ui_time(DVcTime2_12, (TIME *)ui_time_var, -1, -1, 0);
        ui_time(DVcTime1_12, (TIME *)ui_time_var, -1, -1, 0);
    }
#endif
}

///------------rec-display--------------
void lcd_rec_main(void)
{
#if REC_EN
    RECORD_OP_API *rec_var_p;
    ENC_CTL *enc_ctl_var;
    u32 rec_time;

    char lcd_time_str[] = {"     00:00:00  "};
    const char lcd_string[] = {
        "RECORD",
    };
    char rec_nodevice_str[] = {"   no device!!!"};
    const char lcd_sta_str[][17] = {
        "    ENC_STOP    ",
        "    ENC_START   ",
        "    ENC_PAUSE   ",
    };

    lcd_disp_text(lcd_string, DVcTxt1_1);

    if (UI_var.ui_buf_adr) {
        rec_var_p = *(RECORD_OP_API **)UI_var.ui_buf_adr;
        if (rec_var_p) {
            enc_ctl_var = rec_var_p->enc_ctl;
            if (enc_ctl_var) {
                lcd_disp_text((const char *)(lcd_sta_str[enc_ctl_var->enable]), DVcTxt2_1);
                rec_time = enc_ctl_var->file_info.enc_time_cnt;

                if (enc_ctl_var->enable != ENC_STOP) {
                    lcd_disp_dev(rec_get_cur_dev(rec_var_p), DVcTxt3_1);
                    itoa2_api(rec_time / 3600, (u8 *)&lcd_time_str[5]);
                    itoa2_api(rec_time % 3600 / 60, (u8 *)&lcd_time_str[8]);
                    itoa2_api(rec_time % 60, (u8 *)&lcd_time_str[11]);
                    lcd_disp_text((const char *)(lcd_time_str), DVcTxt3_1);
                }
            }
        } else {
            lcd_disp_text((const char *)(lcd_sta_str[ENC_STOP]), DVcTxt2_1);
            if (dev_get_phydev_total(MUSIC_DEV_TYPE, DEV_ONLINE) == 0) {
                lcd_disp_text(rec_nodevice_str, DVcTxt3_1);
                return;
            }
        }
        lcd_disp_rec_opt_state(rec_var_p);
    }
#endif
}

///------------aux-display--------------
void lcd_aux_main(u8 menu)
{
    const char lcd_string_Chinese[] = {"      AUX"};
    RECORD_OP_API *rec_var_p;

    rec_var_p = *(RECORD_OP_API **)UI_var.ui_buf_adr;

#if FMTX_EN
    lcd_disp_freq(fmtx_get_freq(), 2);
#else
    lcd_disp_text(lcd_string_Chinese, DVcTxt1_1);
#endif

#if POWER_EXTERN_DETECT_EN
    ui_pic(DVcRzImg2_5, 0);
    lcd_disp_power();
#else
    ui_pic(DVcRzImg2_1, 0);
#endif

    if (rec_var_p) {
        lcd_disp_rec_opt_state(rec_var_p);
    }
}

///------------rtc,alarm set-display--------------
void lcd_rtc_set(void)
{
#if RTC_CLK_EN
    RTC_SETTING *rtc_var;
    RTC_TIME *ui_time_var;
    _lcd_area_size_t *area_pt;
    const char lcd_string[] = {"时钟设置"};

    lcd_disp_text(lcd_string, DVcTxt1_1);
    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if (rtc_var) {
        ui_time_var = rtc_var->calendar_set.curr_rtc_time;
        ui_time(DVcTime2_13, (TIME *)ui_time_var, -1, -1, 0);
        ui_time(DVcTime1_13, (TIME *)ui_time_var, -1, -1, 0);

        if (rtc_var->calendar_set.coordinate <= COORDINATE_MAX) {
            area_pt = (_lcd_area_size_t *)&time_set_backgroup[rtc_var->calendar_set.coordinate];
            lcd_TurnPixelReverse_Rect(area_pt->left, area_pt->top, area_pt->right, area_pt->bottom);
        }
    }
#endif
}

///------------rtc,alarm set-display--------------
void lcd_alarm_set(void)
{
#if RTC_ALM_EN
    RTC_SETTING *rtc_var;
    RTC_TIME *ui_time_var;
    const char lcd_string[] = {"闹钟设置"};

    lcd_disp_text(lcd_string, DVcTxt1_1);
    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if (rtc_var) {
        ui_time_var = rtc_var->alarm_set.curr_alm_time;
        ui_time(DVcTime2_13, (TIME *)ui_time_var, -1, -1, 0);
        ui_time(DVcTime1_13, (TIME *)ui_time_var, -1, -1, 0);

        if (rtc_var->alarm_set.alarm_sw != 0) {
            ui_pic(DVcRzImg1_13, 0);
        }

        if (rtc_var->alarm_set.coordinate <= COORDINATE_MAX) {
            _lcd_area_size_t *area_pt = (void *)&time_set_backgroup[rtc_var->alarm_set.coordinate];
            lcd_TurnPixelReverse_Rect(area_pt->left, area_pt->top, area_pt->right, area_pt->bottom);
        }
    }
#endif
}

///------------rtc,alarm set-display--------------
void lcd_alarm_ring(void)
{
#if RTC_ALM_EN
    RTC_SETTING *rtc_var;
    RTC_TIME *ui_time_var;
    const char lcd_string[] = {"闹钟响铃"};
    const GUI_POINT  ring_pic_point = {21, 40};

    lcd_disp_text(lcd_string, DVcTxt1_1);
    rtc_var = (RTC_SETTING *)UI_var.ui_buf_adr;

    if (rtc_var) {
        ui_time_var = rtc_var->alarm_set.curr_alm_time;
        ui_time(DVcTime1_13, (TIME *)ui_time_var, -1, -1, 0);
        ui_pic(DVcRzImg1_13, 0);
        ui_pic(DVcRzImg1_13, (GUI_POINT *)&ring_pic_point);
        lcd_check_sys_vol();
    }
#endif
}

#endif
