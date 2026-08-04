#include "sdk_cfg.h"
#include "ui/ui_api.h"
#include "ui/led/led7_drv.h"
#include "ui/led/led_1888_drv.h"
#include "ui/lcd/lcd_drv_api.h"
#include "ui/lcd/lcd_disp.h"
#include "key_drv/key.h"
#include "common/msg.h"
#include "common/app_cfg.h"
#include "rtc_setting.h"
//#include "dac/dac_api.h"
//#include "ui/lcd/lcd_ui_menu.h"
#include "common/common.h"
#include "circular_buf.h"
#include "audio/audio.h"
#include "adc_api.h"
#include "clock_api.h"
#include "fmtx_api.h"
#include "task_common.h"


#if UI_ENABLE

UI_VAR UI_var;   //UI 显示变量
UI_DISP_API *ui_disp_api;

#if LED_7_EN
static const UI_DISP_API ui_led7 = {
    ///--------common------///
    .ui_string_menu = led7_show_string_menu,
    .ui_IR_number = led7_show_IR_number,
    .ui_vol_set = led7_show_volume,
    .ui_clear = led7_clear_icon,
    .ui_setX = led7_setX,
    .ui_power = led7_show_power,

    ///--------music------///
    .ui_music_main = led7_show_music_main,
    .ui_eq_set = led7_show_eq,
    .ui_file_num = led7_show_filenumber,
    .ui_play_mode = led7_show_playmode,

#if FM_RADIO_EN
    ///--------fm------///
    .ui_FM_main = led7_show_fm_main,
    .ui_FM_channel = led7_show_fm_station,
#endif
    ///--------rtc------///
    .ui_RTC_main = led7_show_RTC_main,
    .ui_RTC_set = led7_show_RTC_main,
    .ui_ALM_set = led7_show_alarm,

    ///--------rec------///
#if  REC_EN
    .ui_REC_main = led7_show_rec_main,
#endif

    ///--------echo------///
#if  ECHO_EN
    .ui_echo_main = led7_show_string_menu,
#endif
    ///--------aux------///
    .ui_AUX_main = led7_show_linin_main,

    ///--------bt------///
    .ui_BT_main = led7_show_string_menu,

    ///--------pc------///
    .ui_PC_main = led7_show_string_menu,
    .ui_menu_list  = (void *)led7_show_string_menu,
};
#endif

#if LED_1888_EN
static const UI_DISP_API ui_led1888 = {
    ///--------common------///
    .ui_string_menu = led1888_show_string_menu,
    .ui_IR_number = led1888_show_IR_number,
    .ui_vol_set = led1888_show_volume,
    .ui_clear = led1888_clear_icon,
    .ui_setX = led1888_setX,
    .ui_power = led1888_show_power,

    ///--------music------///
    .ui_music_main = led1888_show_music_main,
    .ui_eq_set = led1888_show_eq,
    .ui_file_num = led1888_show_filenumber,
    .ui_play_mode = led1888_show_playmode,

    ///--------fm------///
    .ui_FM_main = led1888_show_fmtx_main,
    .ui_FM_channel = led1888_show_fm_station,
    ///--------rtc------///
    .ui_RTC_main = led1888_show_RTC_main,
    .ui_RTC_set = led1888_show_RTC_main,
    .ui_ALM_set = led1888_show_alarm,

    ///--------rec------///
#if REC_EN
    .ui_REC_main = led1888_show_rec_main,
#endif
    ///--------aux------///
    .ui_AUX_main = led1888_show_linin_main,

    ///--------bt------///
    .ui_BT_main = led1888_show_bt_main,

    ///--------pc------///
    .ui_PC_main = led1888_show_string_menu,
    .ui_menu_list  = (void *)led1888_show_string_menu,
};
#endif

#if LCD_128X64_EN
static const UI_DISP_API ui_lcd12864 = {
    ///--------common------///
    .ui_string_menu = lcd_disp_string_menu,
    .ui_IR_number = lcd_disp_IR_number,
    .ui_vol_set = lcd_disp_vol,
    .ui_clear = lcd_clear,
    .ui_setX = lcd_setX,
    .ui_power = lcd_disp_power,

    ///--------music------///
    .ui_music_main = lcd_music_main,
    .ui_eq_set = lcd_disp_eq,//(void *)lcd_music_main,
    .ui_file_num = lcd_disp_filenumber,//lcd_music_main,
    .ui_play_mode = lcd_disp_playmode,//(void *)lcd_music_main,

    ///--------fm------///
#if FMTX_EN
    .ui_FM_main = (void *)lcd_fmtx_main,
#else
    .ui_FM_main = (void *)lcd_fm_main,
#endif
    .ui_FM_channel = (void *)lcd_fm_main,

    ///--------rtc------///
    .ui_RTC_main = lcd_rtc_main,
    .ui_RTC_set = lcd_rtc_set,
    .ui_ALM_set = lcd_alarm_set,

    ///--------rec------///
    .ui_REC_main = lcd_rec_main,

    ///--------aux------///
    .ui_AUX_main = lcd_aux_main,

    ///--------bt------///
    .ui_BT_main = lcd_bt_main,

    ///--------pc------///
    .ui_PC_main = lcd_pc_main,

    ///--------echo------///
#if  ECHO_EN
    .ui_echo_main = lcd_echo_main,
#endif

#if LCD_SUPPORT_MENU
    /*menu list*/
    .ui_menu_list  = enter_menulist_display,
#else
    /*menu list*/
    .ui_menu_list  = (void *)lcd_clear,
#endif
};
#endif
static cbuffer_t ui_msg_cbuf;
static u32 ui_msg_pool[32];
static u8 ui_boot_hold = 1;
static u8 ui_boot_pending_menu = MENU_POWER_UP;

bool post_ui_msg(u32 *msg, u8 len)
{
    u32 ui_msg[4];
    if (len > 4) {
        len = 4;
    }
    CPU_INT_DIS();
    memset(ui_msg, 0x0, 4 * sizeof(u32));
    memcpy(ui_msg, msg, len * sizeof(u32));
    if (cbuf_write(&ui_msg_cbuf, (void *)ui_msg, 4 * sizeof(u32)) == 0) {
        CPU_INT_EN();
        return false;
    }
    CPU_INT_EN();
    return true;
}

bool  pend_ui_msg(u32 *msg)
{
    if (msg == NULL) {
        return false;
    }

    CPU_INT_DIS();
    if (cbuf_read(&ui_msg_cbuf, (void *)msg, 4 * sizeof(u32)) == 0) {
        CPU_INT_EN();
        return false;
    }
    CPU_INT_EN();
    return true;
}
void ui_msg_buf_init()
{
    cbuf_init(&ui_msg_cbuf, ui_msg_pool, sizeof(ui_msg_pool));
}

void ui_msg_buf_clear(void)
{
    cbuf_clear(&ui_msg_cbuf);
}

bool ui_dis_main()
{
    u32 msg[4];
    msg[0] = UI_MSG_MAIN_MENU;
    return post_ui_msg(msg, 1);
}

bool ui_dis_menu(u8 menu, u32 arg, u8 back_main_cnt)
{
    u32 msg[4];
    if (back_main_cnt) {
        msg[0] = UI_MSG_SUB_MENU;
        msg[3] = back_main_cnt;
    } else {
        msg[0] = UI_MSG_OTHER_MENU;
    }
    msg[1] = menu;
    msg[2] = arg;
    return post_ui_msg(msg, 4);
}

//主界面刷新显示，显示参数存在 UI_var.ui_buf_adr
bool ui_menu_reflash(u8 menu)
{
    u32 msg[4];
    msg[0] = UI_MSG_REFLASH;
    msg[1] = menu;
    return post_ui_msg(msg, 2);
}

//非主界面刷新显示，需要参数，如音量
bool ui_menu_arg_do(u8 menu, u32 arg)
{
    u32 msg[4];
    msg[0] = UI_MSG_SUB_MENU;
    msg[1] = menu;
    msg[2] = arg;
    return post_ui_msg(msg, 3);
}

//主界面刷新显示,如果当前界面不是互斥界面就刷新
bool ui_menu_spec(u8 NewMenu, u32 CurMenu)
{
    u32 msg[4];
    msg[0] = UI_MSG_REFLASH;
    msg[1] = NewMenu;
    msg[2] = CurMenu;
    if (UI_var.bCurMenu != CurMenu) {
        return post_ui_msg(msg, 3);
    }
    return false;
}

inline void ui_set_main(u32 menu)
{
    if (ui_boot_hold && (menu != MENU_POWER_UP)) {
        ui_boot_pending_menu = menu;
        return;
    }
    UI_var.bMainMenu = menu;
}

void ui_boot_complete(void)
{
    if (!ui_boot_hold) {
        return;
    }

    ui_boot_hold = 0;
    UI_var.bMainMenu = ui_boot_pending_menu;
    ui_dis_main();
}

inline void ui_set_lock(u32 menu)
{
    UI_var.bLockMenu = menu;
}

inline void ui_set_unlock()
{
    UI_var.bLockMenu = 0;
}

inline void ui_set_buf(u32 *addr, u32 len)
{
    UI_var.ui_buf_adr = addr;
    UI_var.ui_buf_len = len;
}

inline void ui_set_sys_vol(u8 *vol)
{
    UI_var.sys_vol = vol;
}

//check curmenu
u32 ui_menu_get_arg(u8 type)
{
    if (type == 1) {
        return UI_var.bCurMenu;
    } else if (type == 0) {
        return UI_var.bMainMenu;
    } else {
        return 0;
    }
}

const u16 ui_ad_table[] = {
    LCD_SEG_3X9_AD,
    LCD_SEG_4X8_AD,
    LED_1888_AD,
    LCD_128X32_AD,
    LCD_128X64_AD,
};
void ui_select(void)
{
    memset(&UI_var, 0x00, sizeof(UI_var));

#if UI_SEL_BY_RES_EN
    u8 i = 0;
    u8 ui_sel_num = 0;
    u16 ui_ad_val = 0;
    u32 ui_ad_sum = 0;

    PORTR_PU(PORTR1, 1);
    PORTR_PD(PORTR1, 0);
    PORTR_DIR(PORTR1, 1);
    PORTR_DIE(PORTR1, 1);
    PORTR1_ADCEN_CTL(1);

    /* JL_PORTA->PD &= ~BIT(3); */
    /* JL_PORTA->PU |= BIT(3); */
    /* JL_PORTA->DIR |= BIT(3); */
    /* JL_PORTA->DIE &= ~BIT(3); */

    adc_init_api(AD_CH_PR1, LSB_CLK, 0);
    for (i = 0; i < 10; i++) {
        ui_ad_val = adc_res_api(AD_CH_PR1);
        ui_ad_sum += ui_ad_val;
    }
    ui_ad_val = (u16)ui_ad_sum / 10;

    for (ui_sel_num = 0; ui_sel_num < sizeof(ui_ad_table) / sizeof(ui_ad_table[0]); ui_sel_num++) {
        if (ui_ad_val > ui_ad_table[ui_sel_num]) {
            break;
        }
    }
    UI_var.ui_type = ui_sel_num;

    adc_off_api();

    PORTR1_ADCEN_CTL(0);

#else

#if (LED_7_EN || LED_1888_EN)
    UI_var.ui_type = UI_LED;
#endif

#if LCD_SEG_3X9_EN
    UI_var.ui_type = UI_LCD_SEG_3X9;
#endif

#if LCD_SEG_4X8_EN
    UI_var.ui_type = UI_LCD_SEG_4X8;
#endif

#if LCD_128X64_EN
    UI_var.ui_type = UI_LCD_128X64;
#endif

#endif
    printf("UI_SEL = %d\n", UI_var.ui_type);
}

void ui_init_api(void)
{
#if LED_7_EN
    if (UI_var.ui_type == UI_LED) {
        led7_init();
        ui_disp_api = (UI_DISP_API *)&ui_led7;
    }
#endif

#if LED_1888_EN
    if (UI_var.ui_type == UI_LED) {
        led1888_init();
        ui_disp_api = (UI_DISP_API *)&ui_led1888;
    }
#endif

#if LCD_128X64_EN
    if (UI_var.ui_type == UI_LCD_128X64) {
        lcd_init();

        ui_disp_api = (UI_DISP_API *)&ui_lcd12864;
    }
#endif

    ui_msg_buf_init();

    /* Show the welcome screen once; MENU_WAIT is reserved for loading. */
    SET_UI_MAIN(MENU_POWER_UP);
    SET_UI_BUF(NULL, 0);
    SET_UI_SYS_VOL(&sound.vol.sys_vol_l);
    UI_var.bMenuReturnmax = UI_RETURN;
    UI_DIS_MAIN();
}

/*----------------------------------------------------------------------------*/
/**@brief   UI 显示界面处理函数
   @param   menu：需要显示的界面
   @return  无
   @note    void UI_menu_api(u8 menu)
*/
/*----------------------------------------------------------------------------*/
void ui_menu_api(u8 menu, s32 arg)
{
#if LCD_128X64_EN
    if (lcd_system_info_is_active()) {
        ui_disp_api->ui_setX(0);
        ui_disp_api->ui_clear();
        lcd_system_info_draw();
        draw_lcd_buf();
        return;
    }
#endif
#if FMTX_EN
    if ((UI_var.bCurMenu == MENU_FM_DISP_FRE) && (menu != MENU_FM_DISP_FRE)) {
        fmtx_set_state(FREQ_TRANSMIT);
    }
#endif
    if ((UI_var.bCurMenu == MENU_INPUT_NUMBER) && (menu != MENU_INPUT_NUMBER)) {
        get_input_number(NULL);		//当前界面不是红外输入界面，清除之前输入的数值
    }

    UI_var.bCurMenu = menu;

    ui_disp_api->ui_setX(0);
    ui_disp_api->ui_clear();

    switch (UI_var.bCurMenu) {
    /*-----System Power On UI*/
    case MENU_POWER_UP:
    case MENU_WAIT:

#if RTC_ALM_EN
    case MENU_ALM_UP:
#endif
    case MENU_DEV_ERR:
        ui_disp_api->ui_string_menu(UI_var.bCurMenu);
        break;

    case MENU_PC_MAIN:
        ui_disp_api->ui_PC_main(UI_var.bCurMenu);
        break;

    case MENU_BT_MAIN:
        ui_disp_api->ui_BT_main(UI_var.bCurMenu);
        break;

    case MENU_AUX_MAIN:
        ui_disp_api->ui_AUX_main(UI_var.bCurMenu);
        break;

    /*-----Common Info UI*/
    case MENU_MAIN_VOL:
        ui_disp_api->ui_vol_set((s32)*UI_var.sys_vol);
        break;

    case MENU_INPUT_NUMBER:
        ui_disp_api->ui_IR_number(arg);
        break;

    /*-----Music Related UI*/
    case MENU_MUSIC_MAIN:
        ui_disp_api->ui_music_main();
        break;
    case MENU_MUSIC_PAUSE:
        ui_disp_api->ui_music_main();
        break;
    case MENU_FILENUM:
        ui_disp_api->ui_file_num();
        break;
    case MENU_SET_EQ:
        ui_disp_api->ui_eq_set(arg);
        break;

    case MENU_SET_PLAY_MODE:
        ui_disp_api->ui_play_mode(arg);
        break;

    /*-----FM Related UI*/
    case MENU_FM_MAIN:
    case MENU_FM_DISP_FRE:
        /* ui_puts("--fm_dis_main---\n"); */
        ui_disp_api->ui_FM_main();
        break;
    case MENU_FM_FIND_STATION:
    case MENU_FM_CHANNEL:
        ui_disp_api->ui_FM_channel();
        break;

#if RTC_CLK_EN
    case MENU_RTC_MAIN:
    //RTC_setting_var.bMode = 0;    //模式与界面同步返回
    case MENU_RTC_SET:
        ui_disp_api->ui_RTC_main();
        break;

#if RTC_ALM_EN
    case MENU_ALM_SET:
        ui_disp_api->ui_ALM_set();
        break;
#endif/*RTC_ALM_EN*/
#endif/*RTC_CLK_EN*/

#if REC_EN
    case MENU_RECODE_MAIN:
        ui_disp_api->ui_REC_main();
        break;
#endif/*REC_EN*/

#if ECHO_EN
    case MENU_ECHO_MAIN:
        ui_disp_api->ui_echo_main(UI_var.bCurMenu);
        break;
#endif/*REC_EN*/



#if _MENU_LIST_SUPPORT_
    case MENU_LIST_DISPLAY:
        ui_disp_api->ui_menu_list(arg);
        break;
#endif

    case MENU_RECODE_ERR:
        break;

    case MENU_POWER:
        ui_disp_api->ui_power();
        break;

    default:
        break;
    }

#if LCD_128X64_EN
    if (UI_var.ui_type == UI_LCD_128X64) {
        draw_lcd_buf();
    }
#endif
}

void UI_run()
{
    u32 msg[4];
    u8 menu;
    u32 arg;
    while (1) {
        memset(msg, 0x0, 4);
        if (pend_ui_msg(msg) == false) {
            return;
        }

        if (msg[0] == UI_MSG_REFLASH) {
            if (msg[1] == MENU_SEC_REFRESH) {
                if (UI_var.bMenuReturnCnt < UI_var.bMenuReturnmax) {
                    UI_var.bMenuReturnCnt++;
                    if (UI_var.bMenuReturnCnt == UI_var.bMenuReturnmax) {
#if FMTX_EN
                        if (UI_var.bCurMenu == MENU_FM_DISP_FRE) {
                            fmtx_set_state(FREQ_TRANSMIT);
                        }
#endif
                        UI_var.bCurMenu = UI_var.bMainMenu;
                    } else {
                        continue;
                    }
                } else {
                    continue;
                }
            }
        }

        UI_var.param_menu = msg[1];

        switch (msg[0]) { //action
        case UI_MSG_MAIN_MENU:
            menu = UI_var.bMainMenu;
            arg  = msg[2];
            break;

        case UI_MSG_REFLASH:
            menu = UI_var.bCurMenu;
            arg  = UI_var.param;
            break;

        case UI_MSG_SUB_MENU:
            menu = msg[1];
            arg  = msg[2];
            UI_var.param = msg[2];
            UI_var.bMenuReturnCnt = 0;
            UI_var.bMenuReturnmax = msg[3];
            break;

        case UI_MSG_OTHER_MENU:
            menu = msg[1];
            arg = msg[2];
            UI_var.param = msg[2];
            //    ui_menu_api(msg[1], msg[2]);
            break;

        case UI_MSG_MENULIST:
        default:
            return;
            //    ui_menu_api(msg[1], UI_NO_ARG);
        }
        ui_menu_api(menu, arg);
    }

}

LOOP_DETECT_REGISTER(ui_run_loop) = {
    .time = 25,
    .fun  = UI_run,
};
#endif
