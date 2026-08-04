#include "sdk_cfg.h"
#include "task_fm_key.h"
#include "common/msg.h"

#if FM_RADIO_EN
/*******************************************************************
                            AD按键表
*******************************************************************/

#define ADKEY_FM_SHORT		\
                        /*00_220K*/    NO_MSG,\
                        /*01_100K*/    NO_MSG,\
                        /*02_51K*/     NO_MSG,\
                        /*03_33K*/     NO_MSG,\
                        /*04_24K*/     NO_MSG,\
                        /*05_15K*/     NO_MSG,\
                        /*06_9.1K*/    MSG_CHANGE_WORKMODE,\
                        /*07_6.2K*/    MSG_FM_SCAN_ALL_UP,\
                        /*08_3K*/      MSG_FM_SCAN_ALL_DOWN,\
                        /*09_0K*/      MSG_FM_PP,

#define ADKEY_FM_LONG		\
                        /*03*/    NO_MSG,\
                        /*01*/    MSG_SYSTEM_INFO,\
                        /*02*/    NO_MSG,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    MSG_EQ_MODE,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

#define ADKEY_FM_HOLD		\
                        /*00*/    NO_MSG,\
                        /*01*/    NO_MSG,\
                        /*02*/    NO_MSG,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    MSG_VOL_UP,\
                        /*08*/    MSG_VOL_DOWN,\
                        /*09*/    NO_MSG,

#define ADKEY_FM_LONG_UP	\
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

const u16 task_fm_ad_table[4][KEY_REG_AD_MAX] = {
    /*短按*/	    {ADKEY_FM_SHORT},
    /*长按*/		{ADKEY_FM_LONG},
    /*连按*/		{ADKEY_FM_HOLD},
    /*长按抬起*/	{ADKEY_FM_LONG_UP},
};

/*******************************************************************
                            I/O按键表
*******************************************************************/
#define IOKEY_FM_SHORT		\
                        /*00*/    MSG_FM_SCAN_ALL_INIT,\
                        /*01*/    MSG_FM_TEST1 /*MSG_FM_NEXT_STATION*/,\
                        /*02*/    MSG_FM_TEST2 /*MSG_FM_PREV_STATION*/,\
                        /*03*/    MSG_FM_TEST3 /*MSG_CHANGE_WORKMODE*/,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*09*/    NO_MSG,

#define IOKEY_FM_LONG		\
                        /*00*/    MSG_CHANGE_WORKMODE,\
                        /*01*/    MSG_VOL_UP,\
                        /*02*/    MSG_VOL_DOWN,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,


#define IOKEY_FM_HOLD		\
                        /*00*/    NO_MSG,\
                        /*01*/    MSG_VOL_UP,\
                        /*02*/    MSG_VOL_DOWN,\
                        /*03*/    NO_MSG,\
                        /*04*/    NO_MSG,\
                        /*05*/    NO_MSG,\
                        /*06*/    NO_MSG,\
                        /*07*/    NO_MSG,\
                        /*08*/    NO_MSG,\
                        /*09*/    NO_MSG,

#define IOKEY_FM_LONG_UP	\
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

const u16 task_fm_io_table[4][KEY_REG_IO_MAX] = {
    /*短按*/	    {IOKEY_FM_SHORT},
    /*长按*/		{IOKEY_FM_LONG},
    /*连按*/		{IOKEY_FM_HOLD},
    /*长按抬起*/	{IOKEY_FM_LONG_UP},
};

/*******************************************************************
                            IR按键表
*******************************************************************/

#define IRFF00_FM_KEY_SHORT			\
                                /*00*/    MSG_ECHO_SW,\
							    /*01*/    MSG_CHANGE_WORKMODE,\
								/*02*/    MSG_MUTE,\
								/*03*/    MSG_ECHO_MUSIC_VOL_SW,\
								/*04*/    MSG_ECHO_MUSIC_VOL_SET,\
								/*05*/    MSG_ECHO_MIC_VOL_SET,\
								/*06*/    NO_MSG,\
								/*07*/    MSG_ECHO_DECAY_SET,\
								/*08*/    MSG_ECHO_PITCH_SET,\
								/*09*/    MSG_0,\
                                /*10*/    MSG_ECHO_DEEP_SET,\
								/*11*/    NO_MSG,\
								/*12*/    MSG_1,\
                                /*13*/    NO_MSG ,\
							    /*14*/    NO_MSG,\
								/*15*/    NO_MSG,\
								/*16*/    NO_MSG,\
								/*17*/    NO_MSG ,\
								/*18*/    NO_MSG ,\
								/*19*/    NO_MSG,\
								/*20*/    NO_MSG,


#define IRFF00_FM_KEY_LONG			\
								/*00*/    NO_MSG,\
                                /*01*/    NO_MSG,\
								/*02*/    NO_MSG,\
								/*03*/    MSG_FM_SCAN_ALL_INIT,\
								/*04*/    MSG_FM_PREV_STATION,\
								/*05*/    MSG_FM_NEXT_STATION,\
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

#define IRFF00_FM_KEY_HOLD			\
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


#define IRFF00_FM_KEY_LONG_UP 		\
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
const u16 task_fm_ir_table[4][KEY_REG_IR_MAX] = {
    /*短按*/	    {IRFF00_FM_KEY_SHORT},
    /*长按*/		{IRFF00_FM_KEY_LONG},
    /*连按*/		{IRFF00_FM_KEY_HOLD},
    /*长按抬起*/	{IRFF00_FM_KEY_LONG_UP},
};


/*******************************************************************
                            touchkey按键表
*******************************************************************/
#define TOUCHKEY_FM_SHORT		\
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

#define TOUCHKEY_FM_LONG		\
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


#define TOUCHKEY_FM_HOLD		\
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

#define TOUCHKEY_FM_LONG_UP	\
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

const u16 task_fm_touch_table[4][KEY_REG_TOUCH_MAX] = {
    /*短按*/	    {TOUCHKEY_FM_SHORT},
    /*长按*/		{TOUCHKEY_FM_LONG},
    /*连按*/		{TOUCHKEY_FM_HOLD},
    /*长按抬起*/	{TOUCHKEY_FM_LONG_UP},
};

/*******************************************************************
                            按键总驱动表
*******************************************************************/
const KEY_REG task_fm_key = {
#if (KEY_AD_RTCVDD_EN||KEY_AD_VDDIO_EN)
    ._ad = task_fm_ad_table,
#endif
#if KEY_IO_EN
    ._io = task_fm_io_table,
#endif
#if KEY_IR_EN
    ._ir = task_fm_ir_table,
#endif
#if KEY_TCH_EN
    ._touch = task_fm_touch_table,
#endif
#if KEY_UART_EN
    ._uart = NULL,
#endif
};

#endif
