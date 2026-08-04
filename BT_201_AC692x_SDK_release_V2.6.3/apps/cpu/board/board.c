#include "sdk_cfg.h"
#include "common/common.h"
#include "clock.h"
#include "hw_cpu.h"
#include "cpu.h"
#include "irq.h"
#include "irq_api.h"
#include "wdt.h"
#include "power.h"
#include "board_init.h"
#include "task_manager.h"
#include "led.h"
#include "pwm_led.h"
#include "msg.h"
#include "uart_param.h"
#include "uart.h"
#include "audio/pdm_link.h"
#include "rtc_api.h"
#include "power_manage_api.h"
#include "charge.h"
#include "flash_api.h"
#include "updata.h"
#include "task_idle.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif
/*----------------------------------------------------------------------------*/
static void reset_source_check(void)
{
    if (JL_SYSTEM->RST_SRC == 0) {
        log_printf("POWERUP RST\n");
        return;
    }

    if (JL_SYSTEM->RST_SRC & BIT(2)) {
        log_printf("VCM RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(3)) {
        log_printf("4S OR 8S RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(6)) {
        log_printf("LVD RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(7)) {
        log_printf("WTD RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(8)) {
        log_printf("SOFT RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(9)) {
        log_printf("POWEROFF RST\n");
    } else {
        log_printf("OTHER RST : %x\n", JL_SYSTEM->RST_SRC);
    }
}


typedef struct _boot_arg {
    u8 osc_type;
    u8 dev_type;
    u8 spi_port;
    u8 osc_freq;
    u32 file_size;
    u32 boot_method;
} BOOT_ARG;
BOOT_ARG boot_arg_list sec_used(.reserved_info);


u32 args[3];
extern void spdif_open();
__attribute__((noinline))
void board_main(u32 cfg_addr, u32 addr, u32 res, u32 first_start_flag)
{
    args[0] = cfg_addr;
    args[1] = addr;
    args[2] = res;

    clock_init_app(SYS_CLOCK_IN, OSC_Hz, SYS_Hz);

    uart_module_init();
    //clock_dump();
log_printf("\r\n");
log_printf(" ╔══════════════════════════════════════════════════════╗\r\n");
log_printf(" ║  __  __   _    ____ ___ ____                         ║\r\n");
log_printf(" ║ |  \\/  | /_\\  / ___|_ _/ ___|                        ║\r\n");
log_printf(" ║ | |\\/| |//_\\\\| |  _ | | |                            ║\r\n");
log_printf(" ║ | |  | / ___ \\ |_| || | |___                         ║\r\n");
log_printf(" ║ |_|  |/_/   \\_\\____|___\\____|                        ║\r\n");
log_printf(" ║                                  _ _     _           ║\r\n");
log_printf(" ║                                 | | |   | |          ║\r\n");
log_printf(" ║                                 | | |   | |          ║\r\n");
log_printf(" ║                             _   | | |   | |          ║\r\n");
log_printf(" ║                            | |__| | |   | |___       ║\r\n");
log_printf(" ║                             \\____/|_|   |_____|      ║\r\n");
log_printf(" ╠══════════════════════════════════════════════════════╣\r\n");
log_printf(" ║  [FIRMWARE] MJL_BT201_Release_v1.0                   ║\r\n");
log_printf(" ║  [COMPILED] "__DATE__" - "__TIME__"               ║\r\n");
log_printf(" ║  [BUILD BY] BarbaroR4(OBM)                           ║\r\n");
log_printf(" ║  [SYS STAT] Start Flag: 0x%02X                       ║\r\n", first_start_flag);
log_printf(" ╚══════════════════════════════════════════════════════╝\r\n");


    updata_result_get(first_start_flag);

    flash_storage_init(cfg_addr);

    task_message_init();

    log_printf("boot_arg_list->osc_type : %d\n", boot_arg_list.osc_type);
    log_printf("boot_arg_list->dev_type : %d\n", boot_arg_list.dev_type);
    log_printf("boot_arg_list->spi_post : %d\n", boot_arg_list.spi_port);
    log_printf("boot_arg_list->osc_freq : %d\n", boot_arg_list.osc_freq);
    log_printf("boot_arg_list->file_size: %d\n", boot_arg_list.file_size);
    log_printf("boot_arg_list->boot_method: %d\n", boot_arg_list.boot_method);

    reset_source_check();

    u8 reg;
    reg = get_wake_up_type();
    log_printf("reg0 : %x\n", reg);

    reg = check_io_wakeup_pend();
    log_printf("reg1 : %x\n", reg);

    power_init_app(PWR_MODE_SELECT, 0);

    /* close_wdt(); */
    open_wdt(WD_32S);			//set wd 32s

    irq_global_enable();

    board_init();

    /* spdif_open(); */
    /* mcpwm_demo(); */
    /* rtc_test_demo(); */
    /* pdm_link_demo(); */
    /* lowpower_test_demo(); */
    //led_pwm_demo();
}

/*主程序入口**/
void main(u32 cfg_addr, u32 addr, u32 res, u32 first_start_flag)
{
    static u8 cnt = 0;
    board_main(cfg_addr,  addr, res, first_start_flag);

    puts("task_run\n");
#if POWERUP_DEV_IGNORE
    task_switch(TASK_ID_BT, NULL);
#else
    task_switch(TASK_ID_IDLE, IDLE_POWER_UP);
#endif
    /* task_switch(TASK_ID_ECHO, NULL); */
    /* task_switch(TASK_ID_RTC, NULL); */
    /* task_switch(TASK_ID_FM, NULL); */
    /* task_switch(TASK_ID_MUSIC, NULL); */
    /* task_switch(TASK_ID_LINEIN, NULL); */
    while (1) {
        task_manager();
    }
}

