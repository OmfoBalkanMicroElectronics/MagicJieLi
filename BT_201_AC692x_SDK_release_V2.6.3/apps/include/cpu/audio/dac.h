/*******************************************************************************************
 File Name: dac.h

 Version: 1.00

 Discription:


 Author:yulin deng

 Email :flowingfeeze@163.com

 Date:2014-01-13 17:09:41

 Copyright:(c)JIELI  2011  @ , All Rights Reserved.
*******************************************************************************************/
#ifndef _DAC_H_
#define _DAC_H_

#include "typedef.h"
#include "circular_buf.h"
#include "audio/dac_api.h"
#include "audio/audio_stream.h"

#define MAX_SYS_VOL_L         30      ///<系统主音量级数
#define MAX_SYS_VOL_R         30

#define MAX_DIGITAL_VOL_L     31      ///<系统数字音量级数
#define MAX_DIGITAL_VOL_R     31

/**
  *LDO选择
  *选择LDO1，DAC_VDD需要电容
  *选择LDO2 ,DAC_VDD不需要电容
  *LDO2的性噪比差点
  */
#define LDO_SLECT  LDO_1

#define BT_CHANNEL         	DAC_DIGITAL_CH
#define MUSIC_CHANNEL      	DAC_DIGITAL_CH
#define RTC_CHANNEL        	DAC_DIGITAL_CH
#define FM_INSI_CHANNEL     DAC_DIGITAL_CH
#define FM_IIC_CHANNEL  	DAC_AMUX1
#define LINEIN_CHANNEL		DAC_AMUX1
#define UDISK_CHANNEL      	DAC_DIGITAL_CH

struct DAC_CTL {
    volatile u8 toggle;
    volatile u32 dac_off_delay;
};
extern struct DAC_CTL dac;


void dac_init(void);
void dac_toggle(u8 toggle);
void dac_int_enable(void);
void dac_int_disable(void);

#endif
