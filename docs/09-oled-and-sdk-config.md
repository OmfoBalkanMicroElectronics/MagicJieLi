# OLED wiring & SDK configuration

This guide is for MagicJL's BT201 / AC6925 reference board with an OLED. Your board's pins, display driver and peripherals may differ.

## 128×64 I²C OLED wiring

MagicJL drives the OLED with a software I²C driver. The lines below are defined in [`i2c_oled.c`](../BT_201_AC692x_SDK_release_V2.6.3/apps/cpu/ui/lcd/i2c_oled.c):

| BT201 / AC6925 pin | Board's UART label | OLED pin | Function |
| --- | --- | --- | --- |
| PB0 | TX | SCL | I²C clock |
| PB1 | RX | SDA | I²C data |
| 3.3 V | 3V3 | VCC | OLED supply |
| GND | GND | GND | Common ground |

```text
BT201 UART header                 I²C OLED
─────────────────                 ────────
PB0  (TX)  ─────────────────────► SCL
PB1  (RX)  ─────────────────────► SDA
3.3 V      ─────────────────────► VCC
GND        ─────────────────────► GND
```

> [!IMPORTANT]
> The header is labelled `TX`/`RX`, but on this board MagicJL uses those pins as the OLED's I²C lines, not as a UART. Power the OLED from **3.3 V** and always share GND. A display or wiring with 5 V I²C levels can damage the AC6925's pins.

> [!CAUTION]
> Don't set `EQ_UART_DEBUG` to `1` in `sdk_cfg.h`. That feature uses `UART1_TXPB0_RXPB1` by default and collides with the OLED on PB0/PB1.

### If the screen stays blank

1. Check that SCL/SDA aren't swapped: **PB0 → SCL**, **PB1 → SDA**.
2. Make sure OLED VCC is 3.3 V and GND is shared.
3. `UI_ENABLE = 1` and `LCD_128X64_EN = 1` must be set.
4. `EQ_UART_DEBUG` must stay `0`.
5. Check that [`i2c_oled.c`](../BT_201_AC692x_SDK_release_V2.6.3/apps/cpu/ui/lcd/i2c_oled.c) is included in the project.

## `sdk_cfg.h` guide

Main configuration file:

[`BT_201_AC692x_SDK_release_V2.6.3/apps/include/sdk_cfg.h`](../BT_201_AC692x_SDK_release_V2.6.3/apps/include/sdk_cfg.h)

Before any change, keep a working `.bfu` or a full UBOOT flash dump. When you change more than one feature, use **Build → Rebuild**.

### Key switches for MagicJL

| Switch | Current value | Effect / caution |
| --- | ---: | --- |
| `UI_ENABLE` | `1` | Whole UI framework. Setting `0` also turns the OLED off. |
| `LCD_128X64_EN` | `1` | Enables the 128×64 dot-matrix OLED/LCD. |
| `LED_7_EN` / `LED_1888_EN` | `0` / `0` | 7-segment / 1888 display drivers; keep off on the OLED board. |
| `LRC_LYRICS_EN` | `0` | LRC lyrics for local music. Enabling it can affect RAM and UI behaviour. |
| `EQ_UART_DEBUG` | `0` | If enabled, the default UART1 on PB0/PB1 collides with the OLED. |
| `USB_DISK_EN` | `1` | Music and updates from a USB stick. |
| `USB_PC_EN` | `1` | USB PC mode. The SDK may disable these modes if USB debug is selected. |
| `SDMMC0_EN` / `SDMMC1_EN` | `0` / `0` | SD card interfaces. `SDMMC1` shares pins with the active debug UART; see below. |
| `ADKEY_SD_MULT_EN` | `0` | ADKEY/SD pin sharing. Don't change it without testing divider/SD compatibility. |
| `BT_TWS` | `0` | True-wireless stereo pairing. Has RAM and EQ dependencies. |
| `BT_CONNTCT_NUM` | `1` | Number of simultaneous Bluetooth connections. (The misspelling is in the SDK.) |
| `BT_BACKGROUND_EN` | `0` | Bluetooth background mode. Can conflict with TWS/echo/FM resources. |
| `BT_MUSIC_VOL_SYNC` | `1` | Syncs music volume with the phone. |
| `FM_RADIO_EN` / `FM_INSIDE` | `1` / `1` | On in the SDK, but the reference BT201 has no FM RF parts; it won't work without adding hardware. |
| `AUX_AD_ENABLE` | `0` | The reference board has no AUX input circuit; keep it off. |
| `REC_EN` | `0` | Recording. Needs RAM, an input and a file system. |
| `ECHO_EN` | `0` | Echo/reverb. Resource limits with software EQ, BLE and TWS. |
| `UART_UPDATA_EN` | `0` | The SDK's UART update option. Independent of the Arduino Nano UBOOT dongle method. |
| `BT_LOW_POWER_MODE` / `SNIFF_MODE_CONF` | `0` / `0` | If enabled, the SDK automatically disables the UI. Keep off for the MagicJL OLED. |
| `SYS_Hz`, `MUSIC_DECODE_Hz` | `192000000L` | The clock MagicJL's UI and FFT/spectrum were tested at. Lowering it can hurt smoothness. |

### Resource constraints

According to the SDK notes at the top of `sdk_cfg.h`, these combinations need care:

- Bluetooth background mode can't be used together with the internal FM.
- Background mode, echo and TWS can't all be used at once.
- Echo together with software EQ and BLE can cause RAM conflicts.
- Software EQ is not supported while TWS is on.
- FM recording is not supported.
- Line-in and microphone recording can't be used at the same time.

> [!TIP]
> Before enabling a feature, change only its `#define` line, Rebuild, and run a short functional test first. Changing several macros at once makes errors hard to trace.

### SDMMC1 and the debug UART

In MagicJL's current configuration, system debug output is on:

```c
#define __DEBUG

#ifdef __DEBUG
#define DEBUG_UART_SEL  UART2_TXPC4_RXPC5
#define DEBUG_UART_RATE 115200
#endif
```

This debug UART shares a pin resource with the SDK's `SDMMC1` interface. To use `SDMMC1` for an SD card, make **both** of these changes in `sdk_cfg.h`:

```c
//#define __DEBUG        // turn off UART debug output

#define SDMMC0_EN 0
#define SDMMC1_EN 1      // enable SDMMC1
```

Then **Build → Rebuild**. With `__DEBUG` off, `log_printf(...)` becomes an empty macro and no error or key logs arrive over UART. Turning debug back on is very useful while developing new features or tracing key/message mappings.

> [!IMPORTANT]
> This doesn't affect the OLED. The OLED is on PB0/PB1, while the system debug UART uses PC4/PC5 in the default MagicJL config. Your board still needs a real SD socket and wiring for `SDMMC1`.

## Debug UART note

General debug output uses `UART2_TXPC4_RXPC5` at `115200` baud in the current configuration. This is separate from the OLED's PB0/PB1 lines. Don't use UART1 debug or EQ debug on PB0/PB1 while the OLED is connected.
