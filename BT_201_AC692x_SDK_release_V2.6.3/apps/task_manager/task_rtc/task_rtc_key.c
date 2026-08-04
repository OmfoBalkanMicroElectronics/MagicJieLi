#include "common/msg.h"
#include "task_rtc_key.h"

#define ADKEY_RTC_SHORT     \
                        /*00*/    NO_MSG,\
                        /*01*/    MSG_RTC_SETTING,\
                        /*02*/    MSG_ALM_SETTING,\
                        /*03*/    MSG_RTC_PLUS,\
                        /*04*/    MSG_RTC_MINUS,\
                        /*05*/    MSG_ALM_SW,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,


#define ADKEY_RTC_LONG      \
                        /*00*/    MSG_POWER_OFF,\
                        /*01*/    MSG_SYSTEM_INFO,\
                        /*02*/    NO_MSG,\
                        /*03*/    MSG_RTC_PLUS,\
                        /*04*/    MSG_RTC_MINUS,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    MSG_CHANGE_WORKMODE,


#define ADKEY_RTC_HOLD      \
                        /*00*/    MSG_POWER_OFF_HOLD,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    MSG_RTC_PLUS,\
                        /*04*/    MSG_RTC_MINUS,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

#define ADKEY_RTC_LONG_UP   \
                        /*00*/    MSG_POWER_KEY_UP,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

const u16 task_rtc_ad_table[4][KEY_REG_AD_MAX] = {
    /*短按*/        {ADKEY_RTC_SHORT},
    /*长按*/        {ADKEY_RTC_LONG},
    /*连按*/        {ADKEY_RTC_HOLD},
    /*长按抬起*/    {ADKEY_RTC_LONG_UP},
};

/*******************************************************************
                            I/O按键表
*******************************************************************/
#define IOKEY_RTC_SHORT     \
                        /*00*/    MSG_RTC_POWER_DOWN,\
                        /*01*/    MSG_RTC_SETTING,\
                        /*02*/    MSG_ALM_SETTING,\
                        /*03*/    MSG_CHANGE_WORKMODE,\
                        /*04*/    MSG_RTC_MINUS,\
                        /*05*/    MSG_ALM_SW,\
                        /*06*/    MSG_RTC_PLUS,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

#define IOKEY_RTC_LONG      \
                        /*00*/    MSG_CHANGE_WORKMODE,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    MSG_RTC_PLUS,\
                        /*04*/    MSG_RTC_MINUS,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,


#define IOKEY_RTC_HOLD      \
                        /*00*/    NO_MSG,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    MSG_RTC_PLUS,\
                        /*04*/    MSG_RTC_MINUS,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

#define IOKEY_RTC_LONG_UP   \
                        /*00*/    NO_MSG,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

const u16 task_rtc_io_table[4][KEY_REG_IO_MAX] = {
    /*短按*/        {IOKEY_RTC_SHORT},
    /*长按*/        {IOKEY_RTC_LONG},
    /*连按*/        {IOKEY_RTC_HOLD},
    /*长按抬起*/    {IOKEY_RTC_LONG_UP},
};

/*******************************************************************
                            IR按键表
*******************************************************************/
#define IRFF00_RTC_KEY_SHORT            \
                                /*00*/    NO_MSG,\
                                /*01*/    MSG_CHANGE_WORKMODE,\
                                /*02*/    MSG_ALM_SW,\
                                /*03*/    MSG_RTC_SETTING,\
                                /*04*/    NO_MSG,\
                                /*05*/    NO_MSG,\
                                /*06*/    MSG_ALM_SETTING,\
                                /*07*/    MSG_RTC_MINUS,\
                                /*08*/    MSG_RTC_PLUS,\
                                /*09*/    NO_MSG,\
                                /*10*/    NO_MSG,\
                                /*11*/    NO_MSG,\
                                /*12*/    NO_MSG,\
                                /*13*/    NO_MSG,\
                                /*14*/    NO_MSG,\
                                /*15*/    NO_MSG,\
                                /*16*/    NO_MSG,\
                                /*17*/    NO_MSG,\
                                /*18*/    NO_MSG,\
                                /*19*/    NO_MSG,\
                                /*20*/    NO_MSG


#define IRFF00_RTC_KEY_LONG         \
                                /*00*/    NO_MSG,\
                                /*01*/    NO_MSG,\
                                /*02*/    NO_MSG,\
                                /*03*/    NO_MSG,\
                                /*04*/    NO_MSG,\
                                /*05*/    NO_MSG,\
                                /*06*/    NO_MSG,\
                                /*07*/    NO_MSG,\
                                /*08*/    NO_MSG,\
                                /*09*/    NO_MSG,\
                                /*10*/    NO_MSG,\
                                /*11*/    NO_MSG,\
                                /*12*/    NO_MSG,\
                                /*13*/    NO_MSG,\
                                /*14*/    NO_MSG,\
                                /*15*/    NO_MSG,\
                                /*16*/    NO_MSG,\
                                /*17*/    NO_MSG,\
                                /*18*/    NO_MSG,\
                                /*19*/    NO_MSG,\
                                /*20*/    NO_MSG

#define IRFF00_RTC_KEY_HOLD         \
                                /*00*/    NO_MSG,\
                                /*01*/    NO_MSG,\
                                /*02*/    NO_MSG,\
                                /*03*/    NO_MSG,\
                                /*04*/    NO_MSG,\
                                /*05*/    NO_MSG,\
                                /*06*/    NO_MSG,\
                                /*07*/    MSG_VOL_DOWN,\
                                /*08*/    MSG_VOL_UP,\
                                /*09*/    NO_MSG,\
                                /*10*/    NO_MSG,\
                                /*11*/    NO_MSG,\
                                /*12*/    NO_MSG,\
                                /*13*/    NO_MSG,\
                                /*14*/    NO_MSG,\
                                /*15*/    NO_MSG,\
                                /*16*/    NO_MSG,\
                                /*17*/    NO_MSG,\
                                /*18*/    NO_MSG,\
                                /*19*/    NO_MSG,\
                                /*20*/    NO_MSG


#define IRFF00_RTC_KEY_LONG_UP      \
                                /*00*/    NO_MSG,\
                                /*01*/    NO_MSG,\
                                /*02*/    NO_MSG,\
                                /*03*/    NO_MSG,\
                                /*04*/    NO_MSG,\
                                /*05*/    NO_MSG,\
                                /*06*/    NO_MSG,\
                                /*07*/    NO_MSG,\
                                /*08*/    NO_MSG,\
                                /*09*/    NO_MSG,\
                                /*10*/    NO_MSG,\
                                /*11*/    NO_MSG,\
                                /*12*/    NO_MSG,\
                                /*13*/    NO_MSG,\
                                /*14*/    NO_MSG,\
                                /*15*/    NO_MSG,\
                                /*16*/    NO_MSG,\
                                /*17*/    NO_MSG,\
                                /*18*/    NO_MSG,\
                                /*19*/    NO_MSG,\
                                /*20*/    NO_MSG
const u16 task_rtc_ir_table[4][KEY_REG_IR_MAX] = {
    /*短按*/        {IRFF00_RTC_KEY_SHORT},
    /*长按*/        {IRFF00_RTC_KEY_LONG},
    /*连按*/        {IRFF00_RTC_KEY_HOLD},
    /*长按抬起*/    {IRFF00_RTC_KEY_LONG_UP},
};


/*******************************************************************
                            touchkey按键表
*******************************************************************/
#define TOUCHKEY_RTC_SHORT      \
                        /*00*/    NO_MSG,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

#define TOUCHKEY_RTC_LONG       \
                        /*00*/    NO_MSG,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,


#define TOUCHKEY_RTC_HOLD       \
                        /*00*/    NO_MSG,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

#define TOUCHKEY_RTC_LONG_UP    \
                        /*00*/    NO_MSG,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

const u16 task_rtc_touch_table[4][KEY_REG_TOUCH_MAX] = {
    /*短按*/        {TOUCHKEY_RTC_SHORT},
    /*长按*/        {TOUCHKEY_RTC_LONG},
    /*连按*/        {TOUCHKEY_RTC_HOLD},
    /*长按抬起*/    {TOUCHKEY_RTC_LONG_UP},
};

const KEY_REG task_rtc_key = {
#if (KEY_AD_RTCVDD_EN||KEY_AD_VDDIO_EN)
    ._ad = task_rtc_ad_table,
#endif
#if KEY_IO_EN
    ._io = task_rtc_io_table,
#endif
#if KEY_IR_EN
    ._ir = task_rtc_ir_table,
#endif
#if KEY_TCH_EN
    ._touch = task_rtc_touch_table,
#endif
#if KEY_UART_EN
    ._uart = task_rtc_io_table,//task_bt_touch_table,
#endif
};
