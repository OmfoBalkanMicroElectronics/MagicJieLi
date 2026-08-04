#include "sdk_cfg.h"
#include "msg.h"
#include "task_common.h"
#include "wdt.h"
#include "task_rtc.h"
#include "task_rtc_key.h"
#include "rtc_setting.h"
#include "sys_detect.h"
#include "key_voice.h"
#include "warning_tone.h"
#include "music_player.h"
#include "rtc_ui.h"
#include "ui_api.h"
#include "led.h"
#include "audio/tone.h"


#ifdef RTC_CLK_EN
#define RTC_TASK_DEBUG_ENABLE
#ifdef RTC_TASK_DEBUG_ENABLE
#define rtc_task_printf log_printf
#else
#define rtc_task_printf(...)
#endif

static void *task_rtc_init(void *priv)
{
    rtc_task_printf("rtc task init\n");
    tone_play(TONE_RTC_MODE, 0);
    led_fre_set(C_BLED_SLOW_MODE);
    return NULL;
}

static void task_rtc_exit(void **hdl)
{
    rtc_task_printf("rtc task exit\n");
    rtc_dac_channel_off();
    task_clear_all_message();
    ui_close_rtc();
}

static void task_rtc_deal(void *hdl)
{
    int msg;
    RTC_SETTING *rtc_api = NULL;
    int msg_error = MSG_NO_ERROR;
    tbool ret = true;

    printf("****************RTC TSAK*********************\n");
    while (1) {
        clear_wdt();
        msg_error = task_get_msg(0, 1, &msg);
        if (task_common_msg_deal(hdl, msg) == false) {
            music_tone_stop();
            rtc_set_alarm_flag(rtc_api, 0);
            task_common_msg_deal(NULL, NO_MSG);
            return;
        }
        if (NO_MSG == msg) {
            continue;
        }

        if (rtc_get_setting_flag(rtc_api) == true) { //进入设置模式
            rtc_setting(msg);
            continue;
        }

        if (rtc_get_alarm_flag(rtc_api)) {
            if ((msg == MSG_RTC_SETTING)
                || (msg == MSG_ALM_SETTING)
                || (msg == MSG_ALM_SW)
                || (msg == MSG_RTC_POWER_DOWN)) {
                rtc_task_printf("stop alarm_ui\n");
                rtc_stop_alm(rtc_api);
                if (rtc_get_alarm_mode(rtc_api) > ALARM_MODE_ONCE) {
                    auto_set_next_alarm(rtc_api);
                }
                continue;
            }
        }


        switch (msg) {
        case SYS_EVENT_PLAY_SEL_END:
            rtc_api = rtc_info_init();
            rtc_update_time(rtc_api);
            ui_open_rtc(rtc_api, sizeof(RTC_SETTING));
            break;

        case MSG_RTC_SETTING:                  ///RTC时间设置
            rtc_task_printf("MSG_RTC_SETTING\n");
            rtc_start_setting(rtc_api, RTC_SET_MODE);
            break;

#if RTC_ALM_EN
        case MSG_ALM_SETTING:                     ///闹钟时间设置
            rtc_task_printf("MSG_ALM_SETTING\n");
            rtc_set_alarm_sw(rtc_api, 1);
            rtc_set_alarm_mode(rtc_api, ALARM_MODE_ONCE);
            rtc_start_setting(rtc_api, ALM_SET_MODE);
            break;
#endif

        case MSG_ALM_SW:
            if (rtc_get_alarm_sw(rtc_api)) {
                rtc_set_alarm_sw(rtc_api, 0);
            } else {
                rtc_set_alarm_sw(rtc_api, 1);
            }
            UI_menu(MENU_ALM_SET, 0, 2);
            break;

        case MSG_HALF_SECOND:
            if (rtc_get_alarm_flag(rtc_api)) {
                puts("alarm ring\n");
                sin_tone_play(250);
                UI_menu(MENU_ALM_UP, 0, 0);
            } else {
                rtc_update_time(rtc_api);
                UI_REFRESH(MENU_REFRESH);
            }
            break;

        case MSG_INPUT_NUMBER_END:
        case MSG_INPUT_TIMEOUT:
            get_input_number(NULL);
            break;

        default:
            break;
        }
    }
}

const TASK_APP task_rtc_info = {
    .skip_check  = NULL,
    .init        = task_rtc_init,
    .exit        = task_rtc_exit,
    .task        = task_rtc_deal,
    .key         = &task_rtc_key,
};

#endif
