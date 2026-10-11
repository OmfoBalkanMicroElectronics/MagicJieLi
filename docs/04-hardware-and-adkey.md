# Hardware & ADKEY

MagicJL is built for the **BT201 board with an OLED and a JieLi AC6925**. Other AC692x boards may differ in LCD resources, pins, flash size, button resistor network and peripherals.

## ADKEY map

| Resistor | Function |
| --- | --- |
| 100 kΩ | Delete request in music mode; long press: system info |
| 33 kΩ | Repeat mode |
| 24 kΩ | Previous folder; hold: rewind |
| 15 kΩ | Next folder; hold: fast-forward |
| 9.1 kΩ | Mode; long press: EQ preset |
| 6.2 kΩ | Next track; hold: volume up |
| 3 kΩ | Previous track; hold: volume down |
| 0 Ω | Play / pause; long press: mute |

The key tables live in `apps\task_manager\task_*\*_key.c`.

## IR and UART

The SDK can define **PA4** as the IR input. That needs a suitable 38 kHz IR receiver and the correct remote mapping. UART pin selection is in `apps\include\sdk_const_define.h`. Check the board's actual wiring and voltage levels before connecting anything.

See also: [OLED wiring & SDK config](09-oled-and-sdk-config.md)
