# Arduino Nano boot dongle

`MagicJL_Boot_Dongle.ino` turns an Arduino Nano into a programmer that puts JieLi AC692x chips into their USB bootloader (UBOOT) mode. It power-cycles the target and clocks the `0x16EF` boot key over D+/D- immediately after power-up. Tested with the AC6905, AC6925 and AC6956.

1. Open `MagicJL_Boot_Dongle.ino` in the Arduino IDE.
2. Select the correct Arduino Nano processor/bootloader option for your board.
3. Upload the sketch to the Nano.
4. Follow the pinout and schematic in the [flashing guide](../../docs/02-flashing.md).

> [!CAUTION]
> The dongle controls the target board's power and USB data lines. Don't connect the Nano to the target until the circuit is built and the wiring has been checked.

Türkçe: [Firmware yükleme rehberi](../../docs/tr/02-firmware-yukleme.md)
