#ifndef _BT_UI_H_
#define _BT_UI_H_

#include "bluetooth/avctp_user.h"
#include "ui_api.h"

typedef struct _BT_DIS_VAR {
    u8 ui_bt_connect_sta;
    u8 ui_bt_a2dp_sta;
    u8 reserver[2];
    u8 bt_eq_mode;
} BT_DIS_VAR;

extern BT_DIS_VAR bt_ui_var;

#if UI_ENABLE
void ui_open_bt(BT_DIS_VAR *buf, u32 len);
void ui_close_bt(void);
void ui_bt_update_var(BT_DIS_VAR *bt_dis_var);
void ui_bt_set_remote_name(const u8 *name);
const char *ui_bt_get_remote_name(void);
void ui_bt_set_music_info(u8 type, const u8 *info, u16 len);
const char *ui_bt_get_track_title(void);
const char *ui_bt_get_track_artist(void);
#else
#define ui_open_bt(...)
#define ui_close_bt(...)
#define ui_bt_update_var(...)
#define ui_bt_set_remote_name(...)
#define ui_bt_get_remote_name() ((const char *)0)
#define ui_bt_set_music_info(...)
#define ui_bt_get_track_title() ((const char *)0)
#define ui_bt_get_track_artist() ((const char *)0)
#endif

#endif/*_BT_UI_H_*/
