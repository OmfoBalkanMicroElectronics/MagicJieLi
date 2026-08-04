#include "common/common.h"
#include "board_init.h"
#include "msg.h"
#include "key.h"
#include "key_drv_encoder.h"

#if KEY_ENCODER_EN

static void encoder_key_init(void)
{
    ENCODER_INIT();
}

no_sequence_initcall(encoder_key_init);

/**********************************************************
扫描编码器子函数
在编码器引脚A为低电平期间：
编码器引脚B从0到1为正转，编码器引脚B从1到0为反转。
**********************************************************/
static void encoder_scan(void)
{
    static u8 Aold = 0, Bold = 0; //定义了两个变量用来储蓄上一次调用此方法是编码开关两引脚的电平
    static u8 st1 = 0, st2 = 0; //定义了一个变量用来储蓄以前是否出现了两个引脚都为高电平的状态

    ENCODER_INIT();

    if ((A_ENCODER_IN && B_ENCODER_IN)) {
        st1 = 1;
    }

    else if (((!A_ENCODER_IN) && (!B_ENCODER_IN))) {
        st2 = 1;
    }

    if (st1) {
        if (((!A_ENCODER_IN) && (!B_ENCODER_IN))) {
            if (Bold) {   //为高说明编码开关在向加大的方向转
                st1 = 0;
                task_post_msg(NULL, 1, MSG_VOL_UP);
            }
            if (Aold) {   //为高说明编码开关在向减小的方向转
                st1 = 0;
                task_post_msg(NULL, 1, MSG_VOL_DOWN);
            }
        }
    }

    if (st2) {
        if (((A_ENCODER_IN) && (B_ENCODER_IN))) {
            if (!Bold) {   //为高说明编码开关在向加大的方向转
                st2 = 0;
                task_post_msg(NULL, 1, MSG_VOL_UP);
            }
            if (!Aold) {   //为高说明编码开关在向减小的方向转
                st2 = 0;
                task_post_msg(NULL, 1, MSG_VOL_DOWN);
            }
        }
    }

    if (A_ENCODER_IN) {
        Aold = 1;
    } else {
        Aold = 0;
    }

    if (B_ENCODER_IN) {
        Bold = 1;
    } else {
        Bold = 0;
    }

}

LOOP_DETECT_REGISTER(encoder_scan_loop) = {
    .time = 1,
    .fun  = encoder_scan,
};

#endif

