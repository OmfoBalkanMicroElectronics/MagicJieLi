#include "sdk_cfg.h"
#include "ui/ui_api.h"
#include "bt_ui.h"
#include "audio/audio.h"

#if  UI_ENABLE

#define BT_REMOTE_NAME_MAX_LEN  16
#define BT_TRACK_INFO_MAX_LEN   48

/* The Bluetooth stack returns the remote device name through a callback. */
static char bt_remote_name[BT_REMOTE_NAME_MAX_LEN + 1];
static char bt_track_title[BT_TRACK_INFO_MAX_LEN + 1];
static char bt_track_artist[BT_TRACK_INFO_MAX_LEN + 1];

#if (LED_7_EN || LED_1888_EN)
void ui_open_bt_led(void *buf, u32 len)
{
    puts("ui_open_bt_led\n");

    SET_UI_MAIN(MENU_BT_MAIN);
    SET_UI_BUF(buf, len);
    UI_DIS_MAIN();
}
#endif


#if LCD_128X64_EN
void ui_open_bt_lcd(void *buf, u32 len)
{
    puts("ui_open_bt_lcd\n");

    SET_UI_MAIN(MENU_BT_MAIN);
    SET_UI_BUF(buf, len);
    UI_DIS_MAIN();
}
#endif

void ui_open_bt(BT_DIS_VAR *buf, u32 len)
{
    if (buf == NULL) {
        return ;
    }
    memset(buf, 0x00, len);
    ui_bt_update_var((BT_DIS_VAR *)buf);

#if (LED_7_EN || LED_1888_EN)
    if (UI_var.ui_type == UI_LED) {
        ui_open_bt_led(buf, len);
    }
#endif

#if LCD_128X64_EN
    if (UI_var.ui_type == UI_LCD_128X64) {
        ui_open_bt_lcd(buf, len);
    }
#endif
}

void ui_close_bt(void)
{
    UI_CLEAR_MSG_BUF();
    SET_UI_MAIN(MENU_WAIT);
    SET_UI_BUF(NULL, 0);
    UI_DIS_MAIN();

}

extern u8 a2dp_get_status(void);

void ui_bt_set_remote_name(const u8 *name)
{
    u8 in = 0;
    u8 out = 0;

    memset(bt_remote_name, 0x00, sizeof(bt_remote_name));
    if (name == NULL) {
        /* A new connection starts with no valid AVRCP metadata. */
        memset(bt_track_title, 0x00, sizeof(bt_track_title));
        memset(bt_track_artist, 0x00, sizeof(bt_track_artist));
    }
    if (name) {
        while ((out < BT_REMOTE_NAME_MAX_LEN) && name[in]) {
            /* The LCD ANSI font is safe for printable ASCII only. */
            if ((name[in] >= 0x20) && (name[in] <= 0x7e)) {
                bt_remote_name[out++] = name[in];
            } else {
                bt_remote_name[out++] = '?';
            }
            in++;
        }
    }

    UI_REFRESH(MENU_REFRESH);
}

const char *ui_bt_get_remote_name(void)
{
    return bt_remote_name;
}

void ui_bt_set_music_info(u8 type, const u8 *info, u16 len)
{
    char *dest;
    u8 in;
    u8 out = 0;

    if (type == 1) {
        dest = bt_track_title;
    } else if (type == 2) {
        dest = bt_track_artist;
    } else {
        return;
    }

    memset(dest, 0x00, BT_TRACK_INFO_MAX_LEN + 1);
    if (info) {
        for (in = 0; (in < len) && (out < BT_TRACK_INFO_MAX_LEN); in++) {
            if ((info[in] >= 0x20) && (info[in] <= 0x7e)) {
                dest[out++] = info[in];
            } else {
                dest[out++] = '?';
            }
        }
    }
    UI_REFRESH(MENU_REFRESH);
}

const char *ui_bt_get_track_title(void)
{
    return bt_track_title;
}

const char *ui_bt_get_track_artist(void)
{
    return bt_track_artist;
}

void ui_bt_update_var(BT_DIS_VAR *bt_dis_var)
{
    if (bt_dis_var == NULL) {
        return;
    }

    bt_dis_var->ui_bt_connect_sta = get_bt_connect_status();
    bt_dis_var->ui_bt_a2dp_sta = (u8)a2dp_get_status();
    bt_dis_var->bt_eq_mode = sound.eq_mode;

    UI_REFRESH(MENU_REFRESH);
}

#endif
