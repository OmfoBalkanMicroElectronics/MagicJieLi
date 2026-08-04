/*--------------------------------------------------------------------------*/
/**@file     key.h
   @brief
   @details
   @author
   @date   2011-4-26
   @note
*/
/*----------------------------------------------------------------------------*/
#ifndef _KEY_
#define _KEY_

#include "typedef.h"
#include "sdk_cfg.h"
// #define KEY_UART_DEBUG

#ifdef KEY_UART_DEBUG
#define key_puts           puts
#define key_printf         printf
#define key_buf            put_buf
#else
#define key_puts(...)
#define key_printf(...)
#define key_buf(...)
#endif/*KEY_UART_DEBUG*/

/*按键类型定义*/
#define KEY_AD_RTCVDD_EN       0   ///<采用RTCVDD电源的AD按键使能
#define KEY_AD_VDDIO_EN        1   ///<采用VDDIO电源的AD按键使能
#define KEY_IO_EN              0   ///<IO按键使能
#define KEY_IR_EN              1   ///<红外遥控使能, PA4 (see IR_IO)
#define KEY_TCH_EN             0   ///<触摸按键使能
#define KEY_UART_EN            0   ///<串口按键使能
#define KEY_ENCODER_EN		   0   ///<旋转编码器使能

#if ((KEY_AD_RTCVDD_EN == 1) && (KEY_AD_VDDIO_EN == 1))
#error  "AD_KEY power support only one"
#endif


/*按键类型*/
typedef enum {
    KEY_TYPE_AD,
    KEY_TYPE_IO,
    KEY_TYPE_IR,
    KEY_TYPE_TOUCH,
    KEY_TYPE_UART,
    MAX_TYPE_KEY,
} KEY_TYPE;

/*按键门槛值*/
#define KEY_BASE_CNT  4
#define KEY_LONG_CNT  75
#define KEY_HOLD_CNT  15
#define KEY_SHORT_CNT 7

/*按键状态*/
#define KEY_SHORT_UP    0x0
#define KEY_LONG        0x1
#define KEY_HOLD        0x2
#define KEY_LONG_UP     0x3

#define KEY_DOUBLE_CLICK_CNT    35 //35*10ms

#define NO_KEY          0xff

typedef struct {
    KEY_TYPE key_type;
    void (*key_init)(void);
    u8(*key_get_value)(void);
} key_interface_t;


#define KEY_REG_AD_MAX			(10)
#define KEY_REG_IO_MAX			(10)
#define KEY_REG_IR_MAX			(21)
#define KEY_REG_TOUCH_MAX		(10)
#define KEY_REG_UART_MAX		(10)

typedef struct __KEY_REG {
#if (KEY_AD_RTCVDD_EN||KEY_AD_VDDIO_EN)
    const u16(*_ad)[KEY_REG_AD_MAX];
#endif
#if KEY_IO_EN
    const u16(*_io)[KEY_REG_IO_MAX];
#endif
#if KEY_IR_EN
    const u16(*_ir)[KEY_REG_IR_MAX];
#endif
#if KEY_TCH_EN
    const u16(*_touch)[KEY_REG_TOUCH_MAX];
#endif
#if KEY_UART_EN
    const u16(*_uart)[KEY_REG_UART_MAX];
#endif
} KEY_REG;

void key_msg_reg(const KEY_REG *reg);
u8 __get_key_invalid_flag(void);
#if (KEY_AD_RTCVDD_EN | KEY_AD_VDDIO_EN | KEY_IO_EN |KEY_IR_EN |KEY_TCH_EN |KEY_UART_EN)
#define key_msg_table_reg key_msg_reg
#define get_key_invalid_flag() __get_key_invalid_flag()
#else
#define key_msg_table_reg(...)
#define get_key_invalid_flag() 0
#endif

#endif
