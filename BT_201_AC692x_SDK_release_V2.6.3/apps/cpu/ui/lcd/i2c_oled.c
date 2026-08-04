#include "sdk_cfg.h"

#include "common.h"
#include "hw_cpu.h"

#include "uicon/dc_showres.h"   /* UI_LCD_INFO, LCD_INFO */
#include "uicon/menu.h"         /* language definitions for font_init_api */

#include "uicon_api.h"          /* uicon_init_api */
#include "font_api.h"           /* font_init_api */

#if LCD_128X64_EN

#define DISP_I2C_ADDR           0x3C

#define DISP_WIDTH              128
#define DISP_HEIGHT             64
#define DISP_PAGES              (DISP_HEIGHT/8)

/* Display buffer */
static u8 disp_buff[DISP_PAGES][DISP_WIDTH];

/* Currently-displayed buffer */
static u8 cdisp_buff[DISP_PAGES][DISP_WIDTH];

/* Is the display attached? */
static u8 disp_available;

/*----------------------------------------------------------*/

static void si2c_init(void) {
        /* PB0 = SCL, PB1 = SDA */
        JL_PORTB->OUT &= ~(3<<0);
        JL_PORTB->DIR |=  (3<<0);
        JL_PORTB->DIE |=  (3<<0);
        JL_PORTB->PU  |=  (3<<0);
        JL_PORTB->PD  &= ~(3<<0);
}

static void si2c_start(void) {
        /* generate a start condition */
        JL_PORTB->DIR &= ~(1<<1);
        delay(10);
        JL_PORTB->DIR &= ~(1<<0);
        delay(10);
}

static void si2c_stop(void) {
        /* generate a stop condition */
        JL_PORTB->DIR |= (1<<0);
        delay(10);
        JL_PORTB->DIR |= (1<<1);
        delay(10);
}

static int si2c_sendb(u8 val) {
        int ack;

        for (int i = 0; i < 8; i++) {
                if (val & 0x80) JL_PORTB->DIR |= (1<<1);
                else            JL_PORTB->DIR &= ~(1<<1);
                val <<= 1;
                delay(10);
                JL_PORTB->DIR |= (1<<0);
                delay(10);
                JL_PORTB->DIR &= ~(1<<0);
        }

        JL_PORTB->DIR |= (1<<1);

        delay(10);
        JL_PORTB->DIR |= (1<<0);
        delay(10);
        ack = (JL_PORTB->IN & (1<<1)) ? 0 : 1;
        JL_PORTB->DIR &= ~(1<<0);

        JL_PORTB->DIR &= ~(1<<1);

        return ack;
}

/*----------------------------------------------------------*/

/*
 * Send commands and data to the display controller.
 */
static int disp_send(const u8 *cmd, int cmdlen, const u8 *data, int datalen) {
        int ack;

        si2c_start();

        ack = si2c_sendb(DISP_I2C_ADDR << 1);
        if (ack) {
                /* Send commands */
                if (cmdlen > 0) {
                        if (datalen > 0) {
                                /* as separate command tokens */
                                while (cmdlen--) {
                                        si2c_sendb(0x80);
                                        si2c_sendb(*cmd++);
                                }
                        } else {
                                /* as a single bulk */
                                si2c_sendb(0x00);
                                while (cmdlen--) {
                                        si2c_sendb(*cmd++);
                                }
                        }
                }

                /* Send data */
                if (datalen > 0) {
                        /* as a single bulk */
                        si2c_sendb(0x40);
                        while (datalen--) {
                                si2c_sendb(*data++);
                        }
                }
        }

        si2c_stop();

        return ack;
}

static int disp_present(int scol, int ecol, int srow, int erow) {
        int ack;

        for (int page = srow; page <= erow; page++) {
                /* Send the row to the display */
                u8 cmd[3] = {
                        0x00 + (scol & 0xF),    /* Column position, low */
                        0x10 + (scol >> 4),     /* Column position, high */
                        0xB0 + page             /* Page index */
                };
                ack = disp_send(cmd, sizeof cmd, &disp_buff[page][scol], ecol-scol+1);

                /* Update the 'currently displayed' buffer, too */
                memcpy(&cdisp_buff[page][scol], &disp_buff[page][scol], ecol-scol+1);

                if (!ack) break;
        }

        return ack;
}

static void disp_setup(void) {
        int ack;

        static const u8 init_cmds[] = {
                0xAE,           /* display off */
                0xA8,0x3F,      /* mux ratio = 64 */
                0xD3,0x00,      /* display offset = 0 */
                0x40,           /* display start line = 0 */
                0xA1,           /* reversed segment remap */
                0xC8,           /* reversed COM scan direction */
                0xDA,0x12,      /* COM pins config - no left/right swap - interlaced mode */
                0x81,0x7F,      /* contrast control */
                0xA4,           /* display CGRAM contents */
                0xA6,           /* normal display */
                0xD5,0x80,      /* osc frequency '8', clock divider '0' */
                0x8D,0x14,      /* en charge pump regulator */
                0xAF,           /* display on */
        };

        ack = disp_send(init_cmds, sizeof init_cmds, NULL, 0);
        if (ack) {
                /* Load the full contents of the display buffer into the display */
                disp_present(0, DISP_WIDTH-1, 0, DISP_PAGES-1);
                /* We've got a display now */
                disp_available = 1;
        }
}

static void disp_init(void) {
        si2c_init();
        disp_setup();
}

static void disp_update(void) {
        if (!disp_available) {
                /* Display is not available yet. Try to reinit it. */
                disp_setup();
                /* The display will be loaded with the full display buffer contents on init
                 * so there is no need to do stuff below */
                return;
        }

        for (int page = 0; page < DISP_PAGES; page++) {
                int scol, ecol;

                /* Find the first changed column */
                for (scol = 0; scol < DISP_WIDTH; scol++) {
                        if (disp_buff[page][scol] != cdisp_buff[page][scol])
                                break;
                }

                /* Find the last changed column */
                for (ecol = DISP_WIDTH-1; ecol >= scol; ecol--) {
                        if (disp_buff[page][ecol] != cdisp_buff[page][ecol])
                                break;
                }

                if (scol <= ecol) {
                        /* Load the changed area into the display. */
                        int ack = disp_present(scol, ecol, page, page);
                        if (!ack) {
                                /* Maybe the display has been disconnected? */
                                disp_available = 0;
                                break;
                        }
                }
        }
}

/*----------------------------------------------------------*/

/*
 * Clear a range of complete pages
 */
void lcd_clear_area(u8 top, u8 bottom) {
        for (int page = top; page < bottom; page++) {
                memset(&disp_buff[page], 0, DISP_WIDTH);
        }
}

/*
 * Same as lcd_clear_area, but it updates the display too.
 */
void lcd_clear_area_with_draw(u8 top, u8 bottom) {
        lcd_clear_area(top, bottom);
        disp_update();
}

/*
 * Clear a section of the display buffer
 */
void lcd_clear_area_rect(u8 top, u8 bottom, u8 left, u8 right) {
        for (int page = top; page < bottom; page++) {
                memset(&disp_buff[page][left], 0, right - left);
        }
}

/*
 * Invert a range of complete pages
 */
u32 lcd_TurnPixelReverse_Page(u8 start, u8 count) {
        for (int page = start; count > 0; count--, page++) {
                for (int x = 0; x < DISP_WIDTH; x++)
                        disp_buff[page][x] ^= 0xff;
        }

        return TRUE;
}

/*
 * Invert a section of the display buffer
 */
u32 lcd_TurnPixelReverse_Rect(u8 left, u8 top, u8 right, u8 bottom) {
        for (int page = top; page <= bottom; page++) {
                for (int x = left; x <= right; x++)
                        disp_buff[page][x] ^= 0xff;
        }

        return TRUE;
}

static const UI_LCD_IO ui_lcd_io = {
        .lcd_clear_area_callback                = lcd_clear_area,
        .lcd_clear_area_with_draw_callback      = lcd_clear_area_with_draw,
        .lcd_clear_area_rect_callback           = lcd_clear_area_rect,
        .lcd_TurnPixelReverse_Page_callback     = lcd_TurnPixelReverse_Page,
        .lcd_TurnPixelReverse_Rect_callback     = lcd_TurnPixelReverse_Rect,
};

const LCD_INFO lcd_info = {
        .ui_lcd_callback        = (void *)&ui_lcd_io,
        .lcd_width              = DISP_WIDTH,
        .lcd_height             = DISP_HEIGHT,
        .lcd_buff               = (void *)disp_buff,
};

/*----------------------------------------------------------*/

/*
 * Display the buffer contents on the screen
 */
void draw_lcd_buf(void) {
        disp_update();
}

/*
 * Clear the display buffer
 */
void lcd_clear(void) {
        memset(disp_buff, 0, sizeof disp_buff);
}

/*
 * Initialize the display
 */
void lcd_init(void) {
        if (!uicon_init_api())
                return;

        if (!font_init_api(Chinese_Simplified))
                return;

        disp_init();

        lcd_clear();
}

#endif // LCD_128X64_EN
