#include "sdk_cfg.h"
#include "task_fm.h"
#include "task_fm_key.h"
#include "fm_api.h"
#include "common/msg.h"
#include "audio/dac_api.h"
#include "key_drv/key.h"
#include "clock_api.h"
#include "common/common.h"
#include "task_common.h"
#include "fm_inside.h"
#include "clock.h"
#include "audio/audio.h"
#include "os/embedded/embedded_os.h"
#include "warning_tone.h"
#include "music_player.h"
#include "fm_ui.h"
#include "ui_api.h"
#include "fm/fm_inside_api.h"
#include "led.h"
#include "task_echo_key.h"
#include "rec_api.h"
#include "fat_io.h"


#include "echo_api.h"

#if FM_RADIO_EN

/* RECORD_OP_API * rec_fm_api = NULL; */
static void set_fm_channel(void);
static void del_fm_channel(u8 chl_num);
static void save_fm_channel(u16 save_freq);

FM_INFO *fm_info = NULL;
FM_MODE_VAR *fm_mode_var = NULL;
RECORD_OP_API *fm_rec_api = NULL;
////---------------test_code------------------//
extern bool fm_test_set_freq(u16 freq);
/*----------------------------------------------------------------------------*/
/**@brief  FM频点测试，直接吧数组里面频点循环设置如FM中
   @param  NULL
   @return NULL
   @note   用于直接测试听台效果
*/
/*----------------------------------------------------------------------------*/
const u16 fm_test_freq_arr[] = {
    875,
    878,
    888,
    898,
    905,
    915,
    918,
    928,
    939,
    951,
    967,
    971,
    980,
    991,
    1007,
    1012,
    1030,
    1038,
    1043,
    1049,
    1071
};

/*----------------------------------------------------------------------------*/
/**@brief  FM频点报号
   @param  freq:875-1080
   @return NULL
   @note   static bool fm_msg_filter(int msg)
   @note   FM频点报号
*/
/*----------------------------------------------------------------------------*/
static void fm_frq_play(u16 freq)
{
    /* void* a, *b, *c, *d; */

    /* if(play_sel_busy()) */
    /* { */
    /* play_sel_stop(); */
    /* fm_prompt_break = 1; */
    /* } */
    /* fm_module_mute(1); */
    /* clear_dac_buf(1); */

    /* a = tone_number_get_name(freq%10000/1000); */
    /* b = tone_number_get_name(freq%1000/100); */
    /* c = tone_number_get_name(freq%100/10); */
    /* d = tone_number_get_name(freq%10); */

    /* printf("a:%s	b:%s	c:%s	d:%s\n",a,b,c,d); */

    /* if(freq%10000/1000) */
    /* { */
    /* tone_play_by_name(FM_TASK_NAME,4,a, b, c, d); */
    /* } */
    /* else */
    /* { */
    /* tone_play_by_name(FM_TASK_NAME,3, b, c, d); */
    /* } */
}

/*----------------------------------------------------------------------------*/
/**@brief  FM频点设置
   @param  freq:875-1080
   @return NULL
   @note   static bool fm_msg_filter(int msg)
   @note   FM频点设置，循环从表中获取频点
*/
/*----------------------------------------------------------------------------*/
static void fm_test_freq_fun(void)
{
    static u8 i = 0;

    u8 max = sizeof(fm_test_freq_arr) / sizeof(fm_test_freq_arr[0]);

    if (i >= max) {
        i = 0;
    }

    fm_frq_play(fm_test_freq_arr[i]);
    fm_test_set_freq(fm_test_freq_arr[i]);
    i++;
}


/*----------------------------------------------------------------------------*/
/**@brief  FM录音过程消息过滤函数
   @param  msg；接收到的消息
   @return 1：不需要过滤，0：过滤
   @note   static bool fm_msg_filter(int msg)
   @note   FM不同工作状态时，部分消息不能进行处理，如还没初始化不能搜台等操作
*/
/*----------------------------------------------------------------------------*/
#if FM_REC_EN
static bool fm_rec_msg_filter(RECORD_OP_API *rec_fm_api, int msg)
{
    if (fm_rec_api) {
        if (rec_get_enc_sta(fm_rec_api) != ENC_STOP) { //正在录音，不响应以下消息
            if ((msg == MSG_FM_SCAN_ALL_INIT)
                || (msg == MSG_FM_SCAN_ALL)
                || (msg == MSG_FM_SCAN_ALL_UP)
                || (msg == MSG_FM_SCAN_ALL_DOWN)) {
                return 0;
            }
        }
    }
    return 1;
}
#endif

/*----------------------------------------------------------------------------*/
/**@brief  FM消息过滤函数
   @param  msg；接收到的消息
   @return 1：不需要过滤，0：过滤
   @note   static bool fm_msg_filter(int msg)
   @note   FM不同工作状态时，部分消息不能进行处理，如还没初始化不能搜台等操作
*/
/*----------------------------------------------------------------------------*/
static bool fm_msg_filter(int msg)
{
    ///FM 任何情况，都必须响应SYS_EVENT_DEL_TASK消息

    if (fm_mode_var->scan_mode == FM_UNACTIVE) { ///FM 还没初始化完成(正在播放提示音)，不响应其他消息
        if (msg == SYS_EVENT_PLAY_SEL_END) {
            return 1;
        } else {
            return 0;
        }
    } else if (fm_mode_var->scan_mode >= FM_SCAN_BUSY) { ///FM正在搜台，只响应部分按键
        if ((msg == SYS_EVENT_PLAY_SEL_END) || \
            (msg == MSG_FM_SCAN_ALL) || \
            (msg == MSG_FM_SCAN_ALL_INIT)) {
            return 1;
        } else {
            return 0;
        }
    } else {        ///一般情况下，不进行过滤
        return 1;
    }
}


static u8 fm_radio_start(void)
{
    u8 ret = FALSE;
    fm_puts("fm_radio_start!!\n");
    ret = fm_radio_init();
    if (ret) {
        mutex_resource_apply("fm", 3, fm_mutex_init, fm_mutex_stop, 0);
    } else {
        task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE);
    }
    return ret;
}

/*----------------------------------------------------------------------------*/
/**@brief  FM模式主线程初始化
   @param  priv：NULL
   @return NULL
   @note   static void fm_radio_task_init(void *priv)
*/
/*----------------------------------------------------------------------------*/
static void *task_fm_init(void *priv)
{
    u32 res;
    fm_printf("task_fm_init\n");

#if FM_REC_EN
    fat_init();
#endif

    fm_arg_open();
    res =  fm_mode_read_id(); //fm_radio_init();
    if (res) {
        tone_play(TONE_RADIO_MODE, 0);
    } else {
        fm_puts("init_fm_err\n");
        task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE);
    }

    return NULL;
}

/*----------------------------------------------------------------------------*/
/**@brief  FM模式退出
   @param  NULL
   @return NULL
   @note   static void fm_radio_task_exit(void)
*/
/*----------------------------------------------------------------------------*/
static void task_fm_exit(void **priv)
{
    /*先关闭FM模块线程，再关闭FM变采样线程*/
    fm_printf("task_fm_exit !!\n");
    mutex_resource_release("fm");

#if SWITCH_PWR_CONFIG
    extern void fm_ldo_level(u8 level);
    if (get_pwr_config_flag()) {
        fm_ldo_level(FM_LDO_REDUCE_LEVEL);
    }
#endif
    fm_radio_powerdown();
    task_clear_all_message();
    fm_arg_close();
    ui_close_fm();
#if FM_REC_EN
    rec_exit(&fm_rec_api);
    mutex_resource_release("record_play");
    fat_del();
#endif
}

static void set_fm_channel(void)
{
    if (fm_mode_var->wTotalChannel == 0) {
        fm_mode_var->wFreChannel = 0;
        fm_mode_var->wFreq = 875;
        fm_module_set_fre(FM_CUR_FRE);
        fm_module_mute(0);
        return;
    }

    if ((fm_mode_var->wFreChannel == 0) || (fm_mode_var->wFreChannel == 0xff)) {
        fm_mode_var->wFreChannel = fm_mode_var->wTotalChannel;
    } else if (fm_mode_var->wFreChannel > fm_mode_var->wTotalChannel) {
        fm_mode_var->wTotalChannel = get_total_mem_channel();
        fm_mode_var->wFreChannel = 1;
    }
    fm_mode_var->wLastwTotalChannel = fm_mode_var->wTotalChannel;
    fm_mode_var->wFreq = get_fre_via_channle(fm_mode_var->wFreChannel) + MIN_FRE;				//根据台号找频点
    fm_module_set_fre(FM_CUR_FRE);
    fm_info->dat[FM_FRE] = fm_mode_var->wFreq - MIN_FRE;
    fm_info->dat[FM_CHAN] = fm_mode_var->wFreChannel;
    fm_save_info();
    fm_module_mute(0);
}

static void del_fm_channel(u8 chl_num)
{
    u8 i = 0;
    u8 j = 0;
    u8 byte = 0;
    u8 channel = 0;

    if (fm_mode_var->wTotalChannel == 0) {
        fm_module_mute(0);
        return;
    }
    if ((chl_num == 0) || (chl_num > fm_mode_var->wTotalChannel)) {
        return;
    }

    //找到channel 有效频点 标记位
    for (i = 0; i < MEM_FM_LEN; i++) {
        byte = fm_info->dat[FM_CHANNL + i];
        for (j = 0; j < 8; j++) {
            if ((byte & (1 << j)) != 0) {
                channel++;
                if (chl_num == channel) {
                    fm_info->dat[FM_CHANNL + i] &= (~(1 << j));
                    goto del_end;
                }
            }
        }
    }

    return;

del_end:

    fm_save_info();

    fm_mode_var->wTotalChannel--;
    if (fm_mode_var->wTotalChannel == 0) {
        fm_mode_var->wFreq = 0;
        fm_module_mute(0);
    } else {
        if (chl_num < fm_mode_var->wFreChannel) {
            //删除当前播放的之前电台
            fm_mode_var->wFreChannel--;
        }

        if (fm_mode_var->wFreChannel > fm_mode_var->wTotalChannel) {
            fm_mode_var->wFreChannel = 1;
        }
    }

    set_fm_channel();

}

static void save_fm_channel(u16 save_freq)
{
    u8 i = 0;
    u8 j = 0;
    u8 byte = 0, byte1;
    u8 channel = 0;

    byte = save_freq - MIN_FRE;
    i = (byte >> 3);
    if (i >= MEM_FM_LEN) {
        return;
    }

    j = (byte & 0x07);

    if ((fm_info->dat[FM_CHANNL + i] & (1 << j)) != 0) {
        //already save
        return;
    }
    fm_info->dat[FM_CHANNL + i] |= (1 << j);

    fm_save_info();

    //确定 channel num
    for (i = 0; i < MEM_FM_LEN; i++) {
        byte1 = fm_info->dat[FM_CHANNL + i];
        for (j = 0; j < 8; j++) {
            if ((byte1 & (1 << j)) != 0) {
                channel++;
                if (byte == (i * 8 + j)) {
                    goto save_end;
                }
            }
        }
    }

save_end:

    fm_mode_var->wTotalChannel++;
    fm_mode_var->wFreChannel = channel;
    set_fm_channel();

}


/*----------------------------------------------------------------------------*/
/**@brief  FM模式主线程
   @param  p：NULL
   @return NULL
   @note   static void fm_radio_task(void *p)
*/
/*----------------------------------------------------------------------------*/
static void task_fm_deal(void *p)
{
    int msg = NO_MSG;
    int error = MSG_NO_ERROR;
    u8 scan_counter = 0;
    u32 fm_input_num = 0;
    u32 res;

    led_fre_set(C_BLED_SLOW_MODE);
    fm_puts("\n***********************RADIO TASK********************\n");

    while (1) {
        error = task_get_msg(0, 1, &msg);
        if (task_common_msg_deal(p, msg) == false) {
            music_tone_stop();
            task_common_msg_deal(NULL, NO_MSG);
            return ;
        }

#if FM_REC_EN
        if ((msg == MSG_REC_START) || (msg == MSG_REC_DEL)) {
            if (FALSE == is_cur_resource("fm")) { //当前资源不属fm所有，不允许录音，防止资源冲突
                continue;
            }
        }
        rec_msg_deal_api(&fm_rec_api, msg); //record 流程
#endif

        if (NO_MSG == msg) {
            continue;
        }


        if ((msg != MSG_HALF_SECOND) && (msg != MSG_ONE_SECOND) && (msg != MSG_FM_SCAN_ALL)) {
            printf("\nfm_msg=   0x%x\n", msg);
        }

        if (!fm_msg_filter(msg)) { //根据FM状态，过滤消息
            continue;
        }

        switch (msg) {
        case SYS_EVENT_PLAY_SEL_END:
            printf("MUSIC SYS_EVENT_PLAY_TONE_END\n");
            if (FM_UNACTIVE == fm_mode_var->scan_mode) {
                fm_radio_start();
            }
            ui_open_fm(&fm_mode_var, sizeof(FM_MODE_VAR **));


            break;

        case  MSG_FM_SCAN_ALL_INIT:
            fm_printf("MSG_FM_SCAN_ALL_INIT\n");
            /* led_fre_set(5,0); */
            if (fm_mode_var->scan_mode == FM_SCAN_STOP) {
                led_fre_set(C_BLED_FAST_MODE);
                delay_n10ms(50);
                fm_info->dat[FM_CHAN] = 0;
                fm_info->dat[FM_FRE] = 0;
                clear_all_fm_point();
                fm_mode_var->wTotalChannel = 0;
                fm_mode_var->wFreChannel = 0;
                fm_mode_var->wFreq = MIN_FRE - 1;//自动搜索从最低的频点开始
                scan_counter = MAX_CHANNL;
                fm_mode_var->scan_mode = FM_SCAN_ALL;
            } else {
                scan_counter = 1;//再搜索一个频点就停止
            }

        case  MSG_FM_SCAN_ALL:
            //fm_printf("MSG_FM_SCAN_ALL\n");
            if (fm_radio_scan(fm_mode_var->scan_mode)) {
                fm_printf("\nFIND FM[%d] total_ch = %d, cnt=%d\n", fm_mode_var->wFreq, fm_mode_var->wTotalChannel, scan_counter);
                if (fm_mode_var->scan_mode == FM_SCAN_ALL) {
                    // printf("wait 1s \n");
                    res = 10;
                    while (res) {
                        delay_n10ms(10);
                        res--;
                    }

                } else {                                                  //找到一个台，半自动搜索，播放当前频点
                    fm_mode_var->scan_mode = FM_SCAN_STOP;
                    set_fm_channel();
                    fm_module_mute(0);
                    // UI_menu(MENU_REFRESH);
                    UI_DIS_MAIN();
                    led_fre_set(C_BLED_SLOW_MODE);
                    /* led_fre_set(15,0); */
                    break;
                }
            }
            scan_counter--;
            if (scan_counter == 0) {
                if (fm_mode_var->scan_mode == FM_SCAN_ALL) {               //全频点搜索结束，播放第一个台
                    task_post_msg(NULL, 1, MSG_FM_NEXT_STATION);
                    fm_mode_var->scan_mode = FM_SCAN_STOP;
                    fm_printf("FM_SCAN_OVER \n");
                    led_fre_set(C_BLED_SLOW_MODE);
                    //fm_insice_scan_info_printf(875,1080);
                } else if ((fm_mode_var->scan_mode == FM_SCAN_NEXT) || (fm_mode_var->scan_mode == FM_SCAN_PREV)) {
                    fm_mode_var->scan_mode = FM_SCAN_STOP;
                    led_fre_set(C_BLED_SLOW_MODE);
                    fm_printf("FM_SCAN_OVER \n");
                }
            } else {   //搜索过程
                delay_n10ms(1);
                if (fm_mode_var->scan_mode != FM_SCAN_STOP) {
                    res = task_post_msg(NULL, 1, MSG_FM_SCAN_ALL);
                    if (res == OS_Q_FULL) {
                        task_post_msg(NULL, 1, MSG_FM_SCAN_ALL);
                    }
                    /* os_sched_unlock(); */
                    UI_DIS_MAIN();
                }
            }
            break;

        case MSG_FM_SCAN_ALL_DOWN:
            fm_printf("MSG_FM_SCAN_ALL_DOWN\n");
            /* fm_mode_var->scan_mode = FM_SCAN_NEXT; */
            fm_mode_var->scan_mode = FM_SCAN_PREV;
            scan_counter = MAX_CHANNL;
            task_post_msg(NULL, 1, MSG_FM_SCAN_ALL);
            break;

        case MSG_FM_SCAN_ALL_UP:
            fm_printf("MSG_FM_SCAN_ALL_UP\n");
            /* fm_mode_var->scan_mode = FM_SCAN_PREV; */
            fm_mode_var->scan_mode = FM_SCAN_NEXT;
            scan_counter = MAX_CHANNL;
            task_post_msg(NULL, 1, MSG_FM_SCAN_ALL);
            break;

        case  MSG_FM_PREV_STEP:
            fm_printf("MSG_FM_PREV_STEP\n");
            res = fm_module_set_fre(FM_FRE_DEC);
            if (res) {
                save_fm_point(fm_mode_var->wFreq - MIN_FRE);
                fm_mode_var->wTotalChannel = get_total_mem_channel();
            }
            fm_mode_var->wFreChannel = get_channel_via_fre(fm_mode_var->wFreq - MIN_FRE);
            fm_info->dat[FM_FRE] = fm_mode_var->wFreq - MIN_FRE;
            fm_info->dat[FM_CHAN] = fm_mode_var->wFreChannel;
            fm_save_info();
            fm_module_mute(0);
            if (!res) {
                fm_mode_var->wFreChannel = 0;
            }

            UI_DIS_MAIN();
            break;

        case MSG_FM_NEXT_STEP:
            fm_printf("MSG_FM_NEXT_STEP\n");
            res = fm_module_set_fre(FM_FRE_INC);
            if (res) {
                save_fm_point(fm_mode_var->wFreq - MIN_FRE);
                fm_mode_var->wTotalChannel = get_total_mem_channel();
            }
            fm_mode_var->wFreChannel = get_channel_via_fre(fm_mode_var->wFreq - MIN_FRE);
            fm_info->dat[FM_FRE] = fm_mode_var->wFreq - MIN_FRE;
            fm_info->dat[FM_CHAN] = fm_mode_var->wFreChannel;
            fm_save_info();
            fm_module_mute(0);
            if (!res) {
                fm_mode_var->wFreChannel = 0;
            }

            UI_DIS_MAIN();
            break;

        case MSG_FM_PREV_STATION:
            fm_printf("MSG_FM_PREV_STATION\n");

            if (fm_mode_var->wTotalChannel == 0) {
                break;
            }

            fm_mode_var->wFreChannel -= 2;

        case MSG_FM_NEXT_STATION:
            fm_printf("MSG_FM_NEXT_STATION\n");

            if (fm_mode_var->wTotalChannel == 0) {
                break;
            }

            fm_mode_var->wFreChannel++;
            set_fm_channel();
            UI_menu(MENU_FM_FIND_STATION, 0, 2);
            break;

        case MSG_INPUT_NUMBER_END:
        case MSG_INPUT_TIMEOUT: {
            if (get_input_number(&fm_input_num)) {
                if (fm_input_num < 100) { //选择台号
                    if (fm_input_num > fm_mode_var->wTotalChannel) {
                        break;
                    }
                    fm_mode_var->wFreChannel = fm_input_num;
                    set_fm_channel();
                } else {  //选择频率
                    if ((fm_input_num < 875) || (fm_input_num > 1080)) {
                        break;
                    }
                    fm_mode_var->wFreq = fm_input_num;
                    res = fm_module_set_fre(FM_CUR_FRE);
                }
            }
            fm_module_mute(0);
            UI_DIS_MAIN();
        }
        break;
        case MSG_FM_PP:
            if (fm_mode_var->fm_mute == 0) {
                fm_module_mute(1);
            } else {
                fm_module_mute(0);
            }
            break;

        case MSG_VOL_UP:
        case MSG_VOL_DOWN:
            if (sound.vol.sys_vol_l && (get_tone_status() == 0)) {
                fm_module_mute(0);
            }
            //fm_printf("fm_vol recv\n");
            break;


        case MSG_PROMPT_PLAY:
        case MSG_LOW_POWER:
            puts("fm_low_power\n");
            /* fm_prompt_play(BPF_LOW_POWER_MP3); */
            break;

        case MSG_FM_TEST1:
            fm_mode_var->wFreq = 875;
            fm_module_set_fre(FM_CUR_FRE);
            fm_module_mute(0);
            fm_printf("fm_mode_var->wFreq = %d\n", fm_mode_var->wFreq);
            break;


        case MSG_HALF_SECOND:

            break;

        default:
            break;
        }
    }
}

const TASK_APP task_fm_info = {
    .skip_check = NULL,//
    .init 		= task_fm_init,
    .exit 		= task_fm_exit,
    .task 		= task_fm_deal,
    .key 		= &task_fm_key,
};


#endif
