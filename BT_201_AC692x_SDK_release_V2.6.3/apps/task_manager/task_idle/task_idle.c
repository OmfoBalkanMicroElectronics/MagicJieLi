#include "task_idle.h"
#include "task_idle_key.h"
#include "msg.h"
#include "task_manager.h"
#include "task_common.h"
#include "audio/dac_api.h"
#include "dac.h"
#include "power_manage_api.h"
#include "dev_manage.h"
#include "warning_tone.h"


#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif

#define TASK_IDLE_DEBUG_ENABLE

#ifdef TASK_IDLE_DEBUG_ENABLE
#define task_idle_printf log_printf
#else
#define task_idle_printf(...)
#endif// TASK_IDLE_DEBUG_ENABLE

extern u32 os_time_get(void);
//
//static tbool task_idle_skip_check(void)
//{
//    task_idle_printf("task_idle_skip_check !!\n");
//    return false;//true;//
//}
//

static void *task_idle_init(void *priv)
{
    /* dac_channel_on(MUSIC_CHANNEL, 0); */
    task_idle_printf("task_idle_init !!\n");
    if (priv == IDLE_POWER_OFF) {
        tone_play(TONE_POWER_OFF, 0);
    } else if (priv == IDLE_POWER_UP) {
        /*
        tone_play(TONE_POWER_ON, 0);
        do{
            task_common_msg_deal(NULL, NO_MSG);
        }while(get_tone_status());
        */
    }
    return priv;
}

static void task_idle_exit(void **hdl)
{
    task_idle_printf("task_idle_exit !!\n");
    task_clear_all_message();
}

extern void delay(u32 d);
static void task_idle_deal(void *hdl)
{
    int error = MSG_NO_ERROR;
    int msg = NO_MSG;
    task_idle_printf("task_idle_deal !!\n");
    u32 i = 0;
    u8 flag = 0;

    /* log_printf("-----------------------------start------------------------------\n"); */
    /* tone_play(0); */
    /* delay(1000); */
    /* tone_play(1); */
    /* delay(100); */
    /* tone_play(2); */
    /* delay(100); */
    /* tone_play(3); */
    /* delay(100); */
    /* tone_play(4); */
    /* tone_play(5); */
    /* log_printf("-----------------------------end------------------------------\n"); */
    while (1) {

        error = task_get_msg(0, 1, &msg);
        if (task_common_msg_deal(hdl, msg) == false) {
            return ;
        }
        if (NO_MSG == msg) {
            continue;
        }

        task_idle_printf("idle msg = %x\n", msg);
        switch (msg) {
        case MSG_HALF_SECOND:
            /* task_idle_printf("-I_H-"); */
            if ((os_time_get() > 200) && (os_time_get() < 500)) {	//2~5s
                //task_time_out, run default task
                task_switch(TASK_ID_BT, NULL);
                return;
            }
            break;

        ///test ------------------------
        case SYS_EVENT_DEC_END:
            /* task_idle_printf("\n---------------sbc notice SYS_EVENT_DEC_END\n"); */
            /* sbc_notice_stop(sbc_hdl); */
            /* sbc_notice_play(i); */
            /* i++; */
            break;

        case SYS_EVENT_PLAY_SEL_END:
            task_idle_printf("SYS_EVENT_PLAY_TONE_END\n");
            if (hdl == IDLE_POWER_OFF) {
                task_idle_printf("idle enter soft_poweroff\n");
#if RTC_CLK_EN
                set_lowpower_keep_32K_osc_flag(1);
#endif
                enter_sys_soft_poweroff();
            }
            break;

        case MSG_POWER_OFF:
            task_idle_printf("idle power off\n");
#if RTC_CLK_EN
            set_lowpower_keep_32K_osc_flag(1);
#endif
            enter_sys_soft_poweroff();
            break;

        default:
            break;
        }
    }
}

const TASK_APP task_idle_info = {
    /* .name 		= TASK_APP_IDLE, */
    .skip_check = NULL,//task_idle_skip_check,//
    .init 		= task_idle_init,
    .exit 		= task_idle_exit,
    .task 		= task_idle_deal,
    .key 		= &task_idle_key,
};

