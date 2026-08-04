#include "board_init.h"
#include "rtc_setting.h"
#include "task_rtc.h"
#include "msg.h"
#include "sys_detect.h"
#include "audio/dac_api.h"
#include "audio/audio.h"
#include "dac.h"
#include "ui_api.h"

#if RTC_CLK_EN

#define RTC_SET_DEBUG_ENABLE
#ifdef RTC_SET_DEBUG_ENABLE
#define rtc_set_printf log_printf
#else
#define rtc_set_printf(...)
#endif

RTC_SETTING rtc_set;
RTC_TIME    current_time;
RTC_TIME    current_alarm;


void puts_time(RTC_TIME *time)
{
    rtc_set_printf("time,%d-%d-%d, %d:%d:%d\n", time->dYear, time->bMonth, time->bDay, \
                   time->bHour, time->bMin, time->bSec);
}
/*----------------------------------------------------------------------------*/
/** @brief: 写入rtc时间值
    @param: 当前时间
    @return: null
    @note:
*/
/*----------------------------------------------------------------------------*/
void rtc_write_datetime(RTC_TIME *curr_time)
{
    struct rtc_data hw_date;
    RTC_TIME_to_rtc_data(&hw_date, curr_time);
    set_rtc_time(&hw_date);
}
/*----------------------------------------------------------------------------*/
/** @brief: 读取rtc时间值
    @param: 当前时间
    @return: null
    @note:
*/
/*----------------------------------------------------------------------------*/
void rtc_read_datetime(RTC_TIME *curr_time)
{
    struct rtc_data hw_date;
    get_rtc_time(&hw_date);
    rtc_data_to_RTC_TIME(curr_time, &hw_date);
}
/*----------------------------------------------------------------------------*/
/** @brief: 写入闹钟时间值
    @param: 闹钟时间
    @return: null
    @note:
*/
/*----------------------------------------------------------------------------*/
void rtc_write_alarmtime(RTC_TIME *alarm_time, u8 alarm_mode)
{
    struct rtc_data hw_date;
    alarm_time->bSec = alarm_mode & 0x03; ///save mode for  alarming power on
    RTC_TIME_to_rtc_data(&hw_date, alarm_time);
    set_rtc_alarm(&hw_date);
}
/*----------------------------------------------------------------------------*/
/** @brief: 读取闹钟时间值
    @param: 闹钟时间
    @return: null
    @note:
*/
/*----------------------------------------------------------------------------*/
void rtc_read_alarmtime(RTC_TIME *alarm_time)
{
    struct rtc_data hw_date;
    get_rtc_alarm(&hw_date);
    rtc_data_to_RTC_TIME(alarm_time, &hw_date);
}

void rtc_update_time(RTC_SETTING *rtc_set)
{
    if (rtc_set) {
        rtc_read_datetime(rtc_set->calendar_set.curr_rtc_time);
        rtc_read_alarmtime(rtc_set->alarm_set.curr_alm_time);
        puts_time(rtc_set->calendar_set.curr_rtc_time);
    }
}

/*----------------------------------------------------------------------------*/
/** @brief: 获取正在设置时钟标志
    @param: null
    @return: true 正在设置   false 不在设置模式
    @note:
*/
/*----------------------------------------------------------------------------*/
bool rtc_get_setting_flag(RTC_SETTING *rtc_set)
{
    if (rtc_set) {
        if (rtc_set->rtc_set_mode != RTC_DISPLAY) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

inline u8 rtc_get_alarm_flag(RTC_SETTING *rtc_set)
{
    if (rtc_set) {
        return rtc_set->alarm_set.alarm_flag;
    }
    return 0;
}

inline s16 rtc_get_alarm_mode(RTC_SETTING *rtc_set)
{
    if (rtc_set) {
        return rtc_set->alarm_set.current_alarm_mode;
    }
    return -1;
}

inline u8 rtc_get_alarm_sw(RTC_SETTING *rtc_set)
{
    if (rtc_set) {
        return rtc_set->alarm_set.alarm_sw;
    }
    return 0;
}

inline void rtc_set_alarm_mode(RTC_SETTING *rtc_set, u8 mode)
{
    if (rtc_set) {
        rtc_set->alarm_set.current_alarm_mode = mode;
    }
}

inline void rtc_set_setting_mode(RTC_SETTING *rtc_set, ENUM_RTC_UI mode)
{
    if (rtc_set) {
        rtc_set->rtc_set_mode = mode;
    }
}

inline void rtc_set_alarm_flag(RTC_SETTING *rtc_set, u8 flag)
{
    if (rtc_set) {
        rtc_set->alarm_set.alarm_flag = flag;
    }
}

inline void rtc_set_alarm_sw(RTC_SETTING *rtc_set, u8 sw)
{
    if (rtc_set) {
        rtc_set->alarm_set.alarm_sw = sw;
        alarm_sw(rtc_set->alarm_set.alarm_sw);
        rtc_set_printf("alm sw %d\n", rtc_set->alarm_set.alarm_sw);
    }
}

/*----------------------------------------------------------------------------*/
/** @brief: 进入设置模式
    @param: null
    @return: null
    @note:
*/
/*----------------------------------------------------------------------------*/
void rtc_start_setting(RTC_SETTING *rtc_set, u8 set_mode)
{
    if (rtc_set == NULL) {
        return;
    }
    rtc_set->calendar_set.coordinate = COORDINATE_MIN;
    rtc_set->alarm_set.coordinate = ALM_COORDINATE_MIN;

    rtc_set->calendar_set.rtc_set_cnt = 0;
    rtc_set->alarm_set.alm_set_cnt = 0;
    rtc_set->rtc_set_mode = set_mode;
    if (rtc_set->rtc_set_mode == RTC_SET_MODE) {
        UI_DIS_MAIN();
    } else {
        rtc_read_alarmtime(&current_alarm);
        UI_menu(MENU_ALM_SET, 0, 0);
    }
}

void rtc_stop_alm(RTC_SETTING *rtc_set)
{
    if (rtc_set) {
        rtc_set->alarm_set.alarm_flag = 0;
    }
    UI_DIS_MAIN();
}

void rtc_stop_alm_api()
{
    rtc_stop_alm(&rtc_set);
}

void rtc_reset_irtc()
{
    set_rtc_enable();
    rtc_clr_power_flag();
}


/*----------------------------------------------------------------------------*/
/** @brief: 时钟消息设置
    @param: msg:消息
    @return: null
    @note:
*/
/*----------------------------------------------------------------------------*/
void rtc_setting(int msg)
{
    if ((MSG_HALF_SECOND != msg) &&
        (msg != MSG_ONE_SECOND)) {
        rtc_set.calendar_set.rtc_set_cnt = 0;
        rtc_set.alarm_set.alm_set_cnt = 0;
    }

    switch (msg) {
    case MSG_RTC_SETTING://移动光标到下一项
        if (rtc_set.rtc_set_mode  != RTC_SET_MODE) {
            rtc_set.rtc_set_mode = RTC_SET_MODE;
            rtc_set.calendar_set.coordinate = COORDINATE_MIN;
        } else {
            rtc_set.calendar_set.coordinate++;
        }
        rtc_set_printf("rtc set %d\n", rtc_set.calendar_set.coordinate);
        if (rtc_set.calendar_set.coordinate > COORDINATE_MAX) { //LED显示时只显示“时”、“分”，设置也只设置这两个
            rtc_set.calendar_set.coordinate = COORDINATE_MIN;
            goto _rtc_setting_exit;
        }

        UI_DIS_MAIN();
        break;

    case MSG_ALM_SETTING://移动光标到下一项
        if (rtc_set.rtc_set_mode != ALM_SET_MODE) { //第一次设置闹钟
            rtc_set.rtc_set_mode = ALM_SET_MODE;
            rtc_set.alarm_set.coordinate = ALM_COORDINATE_MIN;
        } else {
            rtc_set.alarm_set.coordinate++;
        }

        rtc_set_printf("alm set %d\n", rtc_set.alarm_set.coordinate);
        if (rtc_set.alarm_set.coordinate > COORDINATE_MAX) { //LED显示时只显示“时”、“分”，设置也只设置这两个
            rtc_set.alarm_set.coordinate = ALM_COORDINATE_MIN;
            goto _rtc_setting_exit;
        }
        UI_menu(MENU_ALM_SET, 0, 0);
        break;

    case MSG_RTC_PLUS:
        if (rtc_set.rtc_set_mode == RTC_SET_MODE) { //万年历
            calendar_time_plus(rtc_set.calendar_set.curr_rtc_time, rtc_set.calendar_set.coordinate);
            rtc_write_datetime(rtc_set.calendar_set.curr_rtc_time);
            puts_time(rtc_set.calendar_set.curr_rtc_time);
            UI_DIS_MAIN();
        } else if (rtc_set.rtc_set_mode == ALM_SET_MODE) {
            calendar_time_plus(rtc_set.alarm_set.curr_alm_time, rtc_set.alarm_set.coordinate);
            rtc_write_alarmtime(rtc_set.alarm_set.curr_alm_time, rtc_set.alarm_set.current_alarm_mode);
            puts_time(rtc_set.alarm_set.curr_alm_time);
            UI_menu(MENU_ALM_SET, 0, 0);
        }
        rtc_set_printf("rtc plus\n");
        break;

    case MSG_RTC_MINUS:
        if (rtc_set.rtc_set_mode == RTC_SET_MODE) {	//万年历
            calendar_time_minus(rtc_set.calendar_set.curr_rtc_time, rtc_set.calendar_set.coordinate);
            rtc_write_datetime(rtc_set.calendar_set.curr_rtc_time);
            //rtc_clk_out(rtc_set.calendar_set.curr_rtc_time);
            puts_time(rtc_set.calendar_set.curr_rtc_time);
            UI_DIS_MAIN();
        } else if (rtc_set.rtc_set_mode == ALM_SET_MODE) {
            calendar_time_minus(rtc_set.alarm_set.curr_alm_time, rtc_set.alarm_set.coordinate);
            rtc_write_alarmtime(rtc_set.alarm_set.curr_alm_time, rtc_set.alarm_set.current_alarm_mode);
            //rtc_clk_out(rtc_set.alarm_set.curr_alm_time);
            puts_time(rtc_set.alarm_set.curr_alm_time);
            UI_menu(MENU_ALM_SET, 0, 0);
        }
        rtc_set_printf("rtc minus\n");
        break;

    case MSG_ALM_SW:
        //按键开关闹钟时候，自动改为设置闹钟模式
        rtc_set.rtc_set_mode = ALM_SET_MODE;
        if (rtc_set.alarm_set.alarm_sw) {
            rtc_set.alarm_set.alarm_sw = 0;
        } else {
            rtc_set.alarm_set.alarm_sw = 1;
        }
        alarm_sw(rtc_set.alarm_set.alarm_sw);
        UI_menu(MENU_ALM_SET, 0, 0);
        rtc_set_printf("alm sw %d\n", rtc_set.alarm_set.alarm_sw);
        break;

    case MSG_HALF_SECOND:
        //rtc_set_printf("rtc set H\n");
        //时间设置计时
        if (rtc_set.rtc_set_mode == RTC_SET_MODE) {
            rtc_set.calendar_set.rtc_set_cnt++;
            //时间设置超时退出
            if (rtc_set.calendar_set.rtc_set_cnt == RTC_SETTING_CNT) {
                rtc_set.calendar_set.rtc_set_cnt = 0;
                rtc_set_printf("calendar_set timeout\n");
                goto _rtc_setting_exit;
            }
        }

        if (rtc_set.rtc_set_mode == ALM_SET_MODE) {
            rtc_set.alarm_set.alm_set_cnt++;
            if (rtc_set.alarm_set.alm_set_cnt == ALM_SETTING_CNT) {
                rtc_set.alarm_set.alm_set_cnt = 0;
                rtc_set_printf("Alarm Set TimeOut\n");
                goto _rtc_setting_exit;
            }
        }
        break;
    default:
        if ((msg != MSG_RTC_POWER_DOWN) &&
            (msg != MSG_ONE_SECOND)) {
            rtc_set_printf("default : %d\n", msg);
            goto _rtc_setting_exit;
            //    task_post_msg(NULL, 1, msg);
        }
        break;
    }
    return;

_rtc_setting_exit:
    puts("rtc_setting_exit\n");
    rtc_set.alarm_set.curr_alm_time->bSec = 0;
    rtc_set.rtc_set_mode = RTC_DISPLAY;
    rtc_write_alarmtime(rtc_set.alarm_set.curr_alm_time, rtc_set.alarm_set.current_alarm_mode);
    rtc_read_datetime(rtc_set.calendar_set.curr_rtc_time);
    UI_DIS_MAIN();
    return;
}

/*----------------------------------------------------------------------------*/
/** @brief: rtc中断回调函数
    @param: flag:中断类型标志
    @return: null
    @note:
*/
/*----------------------------------------------------------------------------*/
static void rtc_isr_user_handler(u8 flag)
{
    if (RTC_ISR_ALARM_ON == flag) {
        rtc_set_printf("--ALM-ON-ISR--\n");
        rtc_set.alarm_set.alarm_sw = 0;

        alarm_sw(rtc_set.alarm_set.alarm_sw);
        rtc_set.alarm_set.alarm_flag = 1;
        task_post_msg(NULL, 1, MSG_ALM_UP);
    }
    if (RTC_ISR_PCNT == flag) {
        rtc_set_printf("--TYPE_PCNT_OVF--\n");
    }
    if (RTC_ISR_LDO5V == flag) {
        rtc_set_printf("--LDO5V DET--\n");
    }
}

/*----------------------------------------------------------------------------*/
/**@brief 	复位RTC时间
   @param 	void
   @return  void
   @note  	void reset_irtc(void)，根据当前时钟有效值，决定是否复位时钟
*/
/*----------------------------------------------------------------------------*/
void reset_irtc(RTC_TIME *curr_time, RTC_TIME *curr_alm)
{
    rtc_read_datetime(curr_time);
    rtc_read_alarmtime(curr_alm);

    rtc_set.calendar_set.curr_rtc_time = curr_time;
    rtc_set.alarm_set.curr_alm_time = curr_alm;
    if (((curr_time->dYear > 2100)
         || (curr_time->dYear < 2000))
        || (curr_time->bMonth >= 12)
        || (curr_time->bHour >= 24)
        || (curr_time->bMin >= 60)
        || (curr_time->bSec >= 60)
        || rtc_get_power_flag()) {
        puts("\n--------RTC RESET--------\n");
        rtc_set_printf("old time %d/%d/%d %d:%d:%d\n", curr_time->dYear, curr_time->bMonth, curr_time->bDay, curr_time->bHour, curr_time->bMin, curr_time->bSec);
        curr_time->dYear = 2018;
        curr_time->bMonth = 1;
        curr_time->bDay = 29;
        curr_time->bHour = 10;
        curr_time->bMin = 30;
        curr_time->bSec = 0;

        //puts("\n--------ALM RESET--------\n");
        memcpy(curr_alm, curr_time, sizeof(RTC_TIME));

        ///update date
        rtc_write_datetime(curr_time);
        rtc_write_alarmtime(curr_alm, 0);
        rtc_reset_irtc();
        rtc_set.alarm_set.alarm_sw = 0;
        alarm_sw(rtc_set.alarm_set.alarm_sw);
    } else {
        rtc_reset_irtc();
    }
    rtc_set.alarm_set.alarm_sw = get_rtc_alarm_en();//rtc_module_get_alarm_flag();
}

/****************************************************

    note        :open rtc hardware
******************************************************/
static void irtc_hw_init(void)
{
    if (check_alarm_out()) {
        rtc_set.alarm_set.alarm_flag = 1;
        //update mode ,maybe powerup alarming
        rtc_set.alarm_set.current_alarm_mode = current_alarm.bSec & 0x03;
    } else {
        rtc_set.alarm_set.alarm_flag = 0;
    }
    rtc_clk_enable(WAKE_UP_ENABLE, rtc_isr_user_handler);

    reset_irtc(&current_time, &current_alarm);
}
no_sequence_initcall(irtc_hw_init);
/*----------------------------------------------------------------------------*/
/**@brief  RTC DAC通道选择，开启
   @param  无
   @return 无
   @note   void aux_dac_channel_on(void)
*/
/*----------------------------------------------------------------------------*/
void rtc_dac_channel_on(void)
{
    dac_mute(1, 1);
    dac_channel_on(RTC_CHANNEL, FADE_ON);
    //delay(15);// os_time_dly(15);  //这个参数会影响到上电第一次进入line in的冲击声。可以根据样机调整
    dac_mute(0, 1);
    set_sys_vol(sound.vol.sys_vol_l, sound.vol.sys_vol_r, FADE_OFF);
}

/*----------------------------------------------------------------------------*/
/**@brief  RTC DAC通道选择，关闭
   @param  无
   @return 无
   @note   void aux_dac_channel_off(void)
*/
/*----------------------------------------------------------------------------*/
void rtc_dac_channel_off(void)
{
    dac_channel_off(RTC_CHANNEL, FADE_ON);
    dac_mute(0, 1);
}
/*----------------------------------------------------------------------------*/
/**@brief  RTC初始化
   @param  void
   @return 无
   @note   void rtc_info_init(void)
*/
/*----------------------------------------------------------------------------*/
RTC_SETTING *rtc_info_init()
{
    alarm_sw(rtc_set.alarm_set.alarm_sw);
    rtc_dac_channel_on();
    rtc_set.rtc_set_mode = RTC_DISPLAY;
    return &rtc_set;
}

/*----------------------------------------------------------------------------*/
/**@brief  根据闹钟模式设置循环闹钟的日期
   @param  none
   @return none
   @note
*/
/*----------------------------------------------------------------------------*/
void auto_set_next_alarm(RTC_SETTING *rtc_set)
{
    if (rtc_set == NULL) {
        return ;
    }
    if (rtc_set->alarm_set.current_alarm_mode == ALARM_MODE_EVERY_DAY) {
        //every day
        rtc_calculate_next_day(&current_alarm);
    } else {
        //one day for every week
        rtc_calculate_next_weekday(&current_alarm);
    }

    rtc_set_printf("auto_set_alarm:%d-%d-%d,%d:%d:%d\n", current_alarm.dYear, current_alarm.bMonth, current_alarm.bDay, \
                   current_alarm.bHour, current_alarm.bMin, current_alarm.bSec);

    rtc_write_alarmtime(&current_alarm, rtc_set->alarm_set.current_alarm_mode);
    rtc_set->alarm_set.alarm_sw = 1;

    alarm_sw(rtc_set->alarm_set.alarm_sw);
}

#endif  /*RTC_CLK_EN*/
