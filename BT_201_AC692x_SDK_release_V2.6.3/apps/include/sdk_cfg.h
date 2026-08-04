/*********************************************************************************************
    *   Filename        : sdk_cfg.h
    *   Description     : Config for Sound Box Case
    *   Author          : Bingquan
    *   Email           : bingquan_cai@zh-jieli.com
    *   Last modifiled  : 2018-4-07 15:36
    *   Copyright:(c)JIELI  2011-2018  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _CONFIG_
#define _CONFIG_
#include "includes.h"
#include "sdk_const_define.h"
/********               !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                  ********/
//                修改配置前请认真看看这段功能互斥说明，谢谢
//由于AC692X资源有限，SDK开发目前已知会有以下一些功能不能同时使用的情况。
//1、使用后台一定不能使用芯片内部FM功能（硬件规格确定）
//2、后台，混响，TWS功能互斥，只能存在其中一种功能（RAM资源不足）
//3、独立的拍照模式跟后台不能同时打开（地址和名字需要独立初始化）
//4、开了混响无法再打开软件EQ和BLE（RAM资源不足）
//5、开了TWS 无法使用软件EQ（运算速度限制）
//6、FM不支持录音（RAM资源不足）
//7、不能同时录linein和录mic（硬件规格确定）
//8、新增一拖二（也可支持后台），但与混响，TWS功能互斥，只能存在其中一种功能（RAM资源不足）
/*******************************************************************************/
/********************************************************************************/
//------------------------------调试类配置
/********************************************************************************/
//<开启系统打印调试功能
#define __DEBUG

#ifdef __DEBUG
//串口打印IO口选则
#define DEBUG_UART_SEL          UART2_TXPC4_RXPC5
//串口波特率选则
#define DEBUG_UART_RATE         115200    //115200
//打印函数
#define log_printf		        printf
#else
#define log_printf(...)
#endif

#define UART_UPDATA_EN          0

/********************************************************************************/
//------------------------------电源类配置
/********************************************************************************/
//<电量监测
#define SYS_LVD_EN              0
#define POWER_EXTERN_DETECT_EN  0	//外部电压电量检测，一般用于点烟器采集车载电瓶电压
//可选配置：PWR_NO_CHANGE / PWR_LDO15 / PWR_DCDC15
#define PWR_MODE_SELECT        PWR_LDO15
///蓝牙无连接自动关机计时，u16类型，0表示不自动关机
#define AUTO_SHUT_DOWN_TIME     0 //((3*60* 2)/2+10) //除以2 减去进低功耗的时间 概数
///<按键双击功能
#define KEY_DOUBLE_CLICK        0
///<电池电量低，是否切换电源输出配置
#define SWITCH_PWR_CONFIG		0

#define SYS_VDDIO_LEVEL         1
#define SYS_RTCVDD_LEVEL        1

#define SYS_LDO_REDUCE_LEVEL    2

#define LOW_POWER_NOISE_DEAL    0   //低电时底噪处理，该问题存在于VDDIO和HPVDD绑在一起的封装

/********************************************************************************/
//------------------------------音效类配置
/********************************************************************************/
//EQ config.  //more config in audio_stream.h
//EQ选则:EQ_RUN_NULL(不运行EQ) / EQ_RUN_HW (硬件EQ) / EQ_RUN_SW (软件EQ)
#define EQ_RUN_SEL              EQ_RUN_HW    /*更多音效设置请在audio_stream.h头文件配置*/
//EQ 串口在线调试使能
#define EQ_UART_DEBUG           0
//在线调EQ 串口选则.可选:UART1_TXPB0_RXPB1  /  UART1_USB_TXDP_RXDM
#define EQ_DEBUG_UART_SEL       UART1_TXPB0_RXPB1    //EQ_UART_DEBUG 为1时有效
#define EQ_DEBUG_UART_RATE      9600
//<变速变调
//注意该功能开启要考虑芯片性能， 相应提高系统频率，并且尽量不要跟其他cpu占用率高的应用同时打开
#define SPEED_PITCH_EN			0
/********************************************************************************/
//------------------------------DAC配置
/********************************************************************************/
//是否选择VCMO直推耳机
#define VCOMO_EN 	            0
//DAC声道选择：DAC_L_R_CHANNEL / DAC_L_CHANNEL / DAC_R_CHANNEL
#define DAC_CHANNEL_SLECT       DAC_L_R_CHANNEL
//<dac差分输出
#define DAC_DIFF_OUTPUT		 	0
//<dac声道合并
#define DAC_SOUNDTRACK_COMPOUND 0
//<dac声道差集
#define DAC_SOUNDTRACK_SUBTRACT 0
//<通话切换成差分输出(前提是VCOMO_EN为1)
#define CALL_USE_DIFF_OUTPUT	0
//<自动mute
#define DAC_AUTO_MUTE_EN	    1
//<按键音
#define TONE_EN     	    	0
//<非0表示使用默认音量
#define SYS_DEFAULT_VOL         5
//<非0表示提示音使用默认音量
#define TONE_DEFAULT_VOL        1

#define PA_EN                   1

#if PA_EN
#define PMUTE()         {JL_PORTB->DIR &= ~BIT(3);JL_PORTB->OUT |= BIT(3);}
#define PUMUTE()        {JL_PORTB->DIR &= ~BIT(3);JL_PORTB->OUT &= ~BIT(3);}
#endif

/********************************************************************************/
//------------------------------外设类配置
/********************************************************************************/

//SDMMC1 is disabled for UART,if you dont use UART,You can turn the '0' to '1' then comment out '#define __DEBUG', so '//#define __DEBUG'
//But DEBUG will be always important. e.g. implementing new things,or testing to see if a command or a key works.
#define SDMMC0_EN               0
#define SDMMC1_EN               0
#define USB_DISK_EN             1
#define USB_PC_EN               1

//设备在线时 上电是否进音乐模式。 1：上电设备插入不进音乐模式   0：上电进入音乐模式
#define POWERUP_DEV_IGNORE      0
//不使用设备的任务是否需要关闭设备,开启要考虑设备兼容性
#define DEV_POWER_OFF_EN        0

//usb_sd引脚复用，需要测试兼容性
#define USB_SD0_MULT_EN     0   //<需要测试兼容性
#define USB_SD1_MULT_EN     0   //<需要测试兼容性

//adkey 和 sd 复用,需要测试兼容性,注意:ADKEY分压较低的电阻值不能用,会对SD卡造成影响,请确保分压大于1.8V以上
#define ADKEY_SD_MULT_EN    0   //0 不复用  1 复用sd0 2 复用sd1

#if(USB_SD0_MULT_EN == 1)||(USB_SD1_MULT_EN == 1)
#undef USB_PC_EN
#define USB_PC_EN       0
#endif



/********************************************************************************/
//------------------------------蓝牙类配置
/********************************************************************************/
#include "bluetooth/bluetooth_api.h"

///可选配置：0(普通音箱)/BT_TWS_TRANSMIT(对箱使能)
///如果仅作为单机使用，建议不开对箱宏，如果开了对箱宏而且做单机使用会占用基带，单机使用性能没不开对箱宏好
#define BT_TWS                  0//BT_TWS_TRANSMIT
#if BT_TWS
#undef  EQ_RUN_SEL
#define EQ_RUN_SEL              EQ_RUN_NULL/*这个宏不修改，其它更多音效设置请在audio_stream.h头文件配置*/
#endif

///蓝牙连接个数选择 1 /2 一拖二
#if BT_TWS
#define BT_CONNTCT_NUM             2
#define BT_TWS_LINEIN              0  //linein 转换成对箱播放
#else
#define BT_CONNTCT_NUM             1
#define BT_TWS_LINEIN              0
#endif

//蓝牙是否开启后台模式
#if (BT_CONNTCT_NUM == 2)
#define BT_BACKGROUND_EN		0
#else
#define BT_BACKGROUND_EN		0
#endif
#if (BT_BACKGROUND_EN== 0)
///<HID拍照的独立模式只支持非后台
#define BT_HID_INDEPENDENT_MODE  0
#endif
//可选配置：NORMAL_MODE/TEST_BQB_MODE/TEST_FCC_MODE/TEST_FRE_OFF_MODE/TEST_BOX_MODE/TEST_PERFOR_MODE
#define BT_MODE             NORMAL_MODE      // TEST_PERFOR_MODE

//模拟配置
#define BT_ANALOG_CFG           0
#define BT_XOSC                 0

//蓝牙晶振频偏设置 0x0~0xf//如果频偏为正，把值改大
#define BT_OSC_INTERNAL_L       0x09
#define BT_OSC_INTERNAL_R       0x09

//------------------------------蓝牙低功耗设置
//使能该功能后只能是纯蓝牙功能，没有显示功能

//可选配置：SNIFF_EN/SNIFF_TOW_CONN_ENTER_POWERDOWN_EN
#define SNIFF_MODE_CONF	       0//	SNIFF_EN
//可选配置：BT_POWER_DOWN_EN/BT_POWER_OFF_EN
#define BT_LOW_POWER_MODE      0// BT_POWER_DOWN_EN
//可选配置：BT_OSC/RTC_OSCH/RTC_OSCL/LRC_32K
#define LOWPOWER_OSC_TYPE     BT_OSC // LRC_32K
//可选配置：32768L//24000000L//32000L
#define LOWPOWER_OSC_HZ       24000000L//  32000L
//可选配置：BT_BREDR_EN/BT_BLE_EN/(BT_BREDR_EN|BT_BLE_EN)

#if BT_TWS

#undef  SNIFF_MODE_CONF
#define SNIFF_MODE_CONF         0

#undef  BT_LOW_POWER_MODE
#define BT_LOW_POWER_MODE       0

#define BLE_BREDR_MODE          (BT_BREDR_EN)//资源充足的情况，tws 可以开启ble
#else
#define BLE_BREDR_MODE          (BT_BREDR_EN)//资源问题，开了ble，不能开启一拖二
#endif

#if (BLE_BREDR_MODE&BT_BLE_EN)
//可选配置：O--server ,1--client
#define BLE_GAP_ROLE            0
#endif

#define	BT_PHONE_NUMBER		    0
#define	BT_PHONE_VOL_SYNC       1
//是否需要音量同步功能
#define	BT_MUSIC_VOL_SYNC       1
//如果开了音量同步功能要根据实际情况看要不要配HAVE_VOL_CONTROL_KEY这个宏
#define	HAVE_VOL_CONTROL_KEY
//需要电量显示但是不需要通话功能
#define BT_HFP_EN_SCO_DIS		0
//播放手机自带来电提示音(前提是手机支持该功能)
#define BT_INBAND_RINGTONE		0
//对箱角色切换，连接手机的设备即为主机
#define BT_TWS_ROLE_SWITCH		1

///对耳主从同时按下配对按键才进行配对
#define    BT_TWS_SCAN_ENBLE        0
///主从连接上，同步播连接成功提示音、sync_led_scan
#define    BT_TWS_SYNC_CON_STATE_ENBLE        1
/********************************************************************************/
//------------------------------MUSIC MACRO
/********************************************************************************/
//SMP加密文件支持
#define MUSIC_DECRYPT_EN 		0

//<MP3
#define DEC_TYPE_MP3_ENABLE     1

//<SBC
#define DEC_TYPE_SBC_ENABLE     1

//<AAC
#define DEC_TYPE_AAC_ENABLE		0

//<8K_code_space
#define DEC_TYPE_WAV_ENABLE     1

//<8K_code_space
#define DEC_TYPE_FLAC_ENABLE    1

//<8K_code_space
#define DEC_TYPE_APE_ENABLE     0

//<32K_code_space
#define DEC_TYPE_WMA_ENABLE     0

//<32K_code_space
#define DEC_TYPE_F1A_ENABLE     0

//<2K_code_space
#define DEC_TYPE_NWT_ENABLE     0

//<76K_code_space
#define DEC_TYPE_AMR_ENABLE     0

//<60K_code_space
#define DEC_TYPE_M4A_ENABLE     0

//<112K_code_space
#define DEC_TYPE_DTS_ENABLE     0

/********************************************************************************/
//------------------------------FM MACRO
/********************************************************************************/
//<标准SDK
#define FM_RADIO_EN             1

//dependency
#if (FM_RADIO_EN == 1)
//<RDA5807FM
#define RDA5807                 0
//<BK1080FM
#define BK1080                  0
//<QN8035FM
#define QN8035                  0
//<芯片内部FM
#if BT_BACKGROUND_EN
//蓝牙RF和收音RF共用，使用后台不能用内部收音
#define FM_INSIDE               0
#else
#define FM_INSIDE               1  //Internal radio of AC6925/BT201. The board doesnt have the elements/components for FM Radio,you will have to solder them yourself.
#endif
#endif

/********************************************************************************/
//------------------------------AUX MACRO
/********************************************************************************/
#define AUX_AD_ENABLE           0
//aux检测使能
#define AUX_DETECT_EN           1

#if AUX_DETECT_EN
#define AUX_DET_MULTI_AD_KEY    0     //ad_key复用aux检测
#endif

/********************************************************************************/
//------------------------------REC MACRO
///********************************************************************************/
///<标准SDK录音功能
#define REC_EN              0


#if (REC_EN == 1)
///<MIC录音使能
#define MIC_REC_EN		1
#define AUX_REC_EN      1
#define FM_REC_EN       1
#define BT_REC_EN       1

#if (AUX_REC_EN == 1)  //AUX必须为数字通道
#undef AUX_AD_ENABLE
#define AUX_AD_ENABLE   1
#endif

#define REC_PLAY_EN     1
#endif

/********************************************************************************/
//------------------------------RTC MACRO
/********************************************************************************/
//标准SDK RTC时钟模式
#define RTC_CLK_EN              0

//dependency
#if (RTC_CLK_EN == 1)
//<RTC闹钟模式
#define RTC_ALM_EN              1
#endif

/********************************************************************************/
//------------------------------ECHO MACRO
/********************************************************************************/
//混响使能, 打开后可在BT/FM/MUSIC/LINEIN下开混响功能.
#define ECHO_EN                 0

#if ECHO_EN
//ECHO_EN为1时, 以下配置才有效
#define PITCH_EN                1
//单独混响模式使能
#define TASK_ECHO_EN            1

//混响一般不自动MUTE DAC
#undef DAC_AUTO_MUTE_EN
#define  DAC_AUTO_MUTE_EN       0

//混响不能开AUX的AD通道
#undef AUX_AD_ENABLE
#define AUX_AD_ENABLE   0

//混响与AUX录音有冲突，不能同时开
#undef AUX_REC_EN
#define AUX_REC_EN      0
#endif

/********************************************************************************/
//------------------------------SPDIF MACRO
/********************************************************************************/
//支持同轴，光纤输入的spdif task
#define SPDIF_EN                 0



/********************************************************************************/
//------------------------------FM TRANSMITTER MACRO
///********************************************************************************/
////FM发射器配置，点烟器用
#define FMTX_EN       0

#if (FMTX_EN)
//QN8027
#define QN8027                 	1
//QN8007
#define QN8007					0
#endif

/********************************************************************************/
//------------------------------UI MACRO
/********************************************************************************/
///<LED指示使能
#define LED_EN                  1

#define UI_ENABLE               1

#if (BT_LOW_POWER_MODE || SNIFF_MODE_CONF)            //进低功耗模式 not support ui
#undef  UI_ENABLE
#define UI_ENABLE               0
#endif
///dependency
#if (UI_ENABLE == 1)
#define UI_SEL_BY_RES_EN		0	  ///是否通过选屏电阻进行选屏
#define LED_7_EN                0     ///<led 支持
#define LED_1888_EN             0	  ///6脚数码管，点烟器一般用这个
#define LCD_128X64_EN           1	  ///点阵屏LCD
#else
///都不支持
#define LED_7_EN                0
#define LED_1888_EN             0
#define LCD_128X64_EN           0
#endif

#if (LCD_128X64_EN == 1)
#define LRC_LYRICS_EN			0	  ///LRC歌词显示
#else
#define LRC_LYRICS_EN           0
#endif

/********************************************************************************/
//------------------------------系统时钟等配置
/********************************************************************************/
//时钟配置  //more config in clock_interface.h
#define OSC_Hz                  24000000L	//fpga:12M / chip:24M

///<SYS_CLK   //不同工作状态的系统时钟选则
#define BT_CALL_Hz		        192000000L	//phone call clock
#define BT_REC_Hz		        192000000L	//bt rec clock

#if (EQ_RUN_SEL == EQ_RUN_SW || SPEED_PITCH_EN || ECHO_EN )    //Software EQ need Run 192M
#define MUSIC_DECODE_Hz         192000000L
#define SYS_Hz		            192000000L  //120000000L//96000000L
#else
#define MUSIC_DECODE_Hz         192000000L
#if BT_TWS
#define SYS_Hz		            192000000L
#else
#define SYS_Hz		            192000000L
#endif
#endif


/********************************************************************************/
//------------------------------有冲突的宏处理
/********************************************************************************/
//USB口用于调试时, 关闭USB_DISK/PC功能.
#if ( (defined(__DEBUG) && (DEBUG_UART_SEL == UART1_USB_TXDP_RXDM)) || \
       ( EQ_UART_DEBUG && (EQ_DEBUG_UART_SEL == UART1_USB_TXDP_RXDM) ))
#undef  USB_DISK_EN
#undef  USB_PC_EN
#define USB_DISK_EN             0
#define USB_PC_EN               0
#endif

//调试用的串口 和 EQ在线调试串口冲突
#if ( (defined(__DEBUG) && (DEBUG_UART_SEL == UART1_USB_TXDP_RXDM)) && \
       ( EQ_UART_DEBUG && (EQ_DEBUG_UART_SEL == UART1_USB_TXDP_RXDM) ))
#error "DEBUG_UART_SEL same with EQ_DEBUG_UART_SEL"
#endif

//没有内部收音也没有外部收音时不定义收音模式
#if ( (RDA5807 == 0 ) && (BK1080 == 0 ) && (QN8035 == 0) && (FM_INSIDE==0) )
#undef FM_RADIO_EN
#define FM_RADIO_EN             0
#endif

//没有FM发射芯片的时候不开点烟器的宏
#if ( (QN8007 == 0 ) && (QN8027 == 0 ) )
#undef FMTX_EN
#define FMTX_EN       0
#endif

//点烟器不需要FM收音模式
#if (FMTX_EN == 1)
#undef FM_RADIO_EN
#define FM_RADIO_EN             0
#endif

#if (ECHO_EN == 1)
#undef  DAC_AUTO_MUTE_EN
#define DAC_AUTO_MUTE_EN		0
#endif

#if (SNIFF_MODE_CONF == SNIFF_EN)
#undef  DEV_POWER_OFF_EN
#define DEV_POWER_OFF_EN    1
#endif

#endif  //end of _SDK_CFG_
