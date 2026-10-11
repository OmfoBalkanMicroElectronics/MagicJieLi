# MagicJL: Custom Firmware for the JieLi AC6925 (BT201)

> A small audio chip can have a big personality.

**English** · [Türkçe](README.tr.md)

<p align="center">
  <img src="images/flash/bt201.jpg" alt="MagicJL target board: BT201 with a JieLi AC6925" width="720">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/SoC-JieLi%20AC6925-red" alt="SoC: JieLi AC6925">
  <img src="https://img.shields.io/badge/Board-BT201-blue" alt="Board: BT201">
  <img src="https://img.shields.io/badge/ISA-PI32V2-6f42c1" alt="ISA: PI32V2">
  <img src="https://img.shields.io/badge/Clock-192%20MHz-orange" alt="Clock: 192 MHz">
  <img src="https://img.shields.io/badge/IDE-Code%3A%3ABlocks%2016.01-3b82f6" alt="IDE: Code::Blocks 16.01">
  <img src="https://img.shields.io/badge/Firmware-MagicJL%20v1.0-brightgreen" alt="Firmware: MagicJL v1.0">
  <img src="https://img.shields.io/badge/License-MIT%20(MagicJL%20parts)%20%2B%20vendor-lightgrey" alt="License: MIT for MagicJL parts, vendor terms for the SDK">
</p>

**MagicJL** is community firmware for the cheap **BT201** Bluetooth audio board, which is built around the **JieLi AC6925** (AC692x family). It replaces the stock speaker/player interface with a richer UI on a 128×64 OLED. Features include animated Bluetooth status screens, AVRCP "now playing" text, and a real-time **10-band FFT spectrum analyzer** computed in fixed-point on a chip that has no FPU or hardware FFT.

The AC692x is a niche part with almost no English documentation. This repository is also a practical reference for working with it: building the vendor SDK, entering the bootloader with an Arduino Nano, taking and verifying full flash dumps, restoring them, and configuring the SDK without breaking the board.

> [!NOTE]
> **Unofficial community project.** MagicJL is not affiliated with or endorsed by Zhuhai JieLi Technology. Original MagicJL work is MIT-licensed under [LICENSE-MAGICJL.md](LICENSE-MAGICJL.md). The bundled vendor SDK and libraries stay under their owners' terms. See [NOTICE.md](NOTICE.md).

---

## Contents

- [Features](#features)
- [Hardware](#hardware)
- [Quick start](#quick-start)
- [Flashing](#flashing)
- [Controls (ADKEY)](#controls-adkey)
- [How the spectrum analyzer works](#how-the-spectrum-analyzer-works)
- [User interface](#user-interface)
- [Local music player](#local-music-player)
- [SDK configuration](#sdk-configuration)
- [Repository layout](#repository-layout)
- [About the AC692x](#about-the-ac692x)
- [The MagicJL family](#the-magicjl-family)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [Credits](#credits)

## Features

- **Animated boot screen** with typewriter welcome text and a sliding credit
- **Custom Bluetooth screens:** spinning "waiting for connection" ring, connected/music/call icons, connected device name
- **AVRCP now-playing:** track title and artist scroll across the screen in a custom pixel font
- **10-band FFT spectrum analyzer** fed by real PCM data, with smoothing, peak-hold and per-band auto-gain, in both Bluetooth and USB/SD playback
- **System info screen** that opens over any mode without interrupting playback
- **Reworked ADKEY layout:** seek, folder navigation, EQ presets on long-press Mode, volume on hold
- **Safe file deletion** with a two-step on-screen confirmation
- **Turkish UI text** (written in ASCII for the built-in font)

## Hardware

| | |
| --- | --- |
| **SoC** | JieLi AC6925 (AC692x family), PI32V2 core @ 192 MHz |
| **Memory** | 128 KB SRAM, 512 KB SPI flash (BT201 reference board) |
| **Display** | 128×64 I²C OLED on PB0 (SCL) / PB1 (SDA), driven by software I²C |
| **Input** | Resistor-ladder ADKEY buttons |
| **Storage** | USB mass storage; SD card optional (see [SDK configuration](docs/09-oled-and-sdk-config.md)) |
| **Programmer** | Arduino Nano boot dongle ([sketch](hardware/arduino-nano-dongle/)) |

> [!IMPORTANT]
> MagicJL targets one specific **BT201 + OLED** board layout. Other AC692x boards can have different pins, flash size, display, button resistors and RF parts. On another board, **back up the stock firmware first** ([guide](docs/08-backup-and-recovery.md)) and expect to edit `sdk_cfg.h`.

## Quick start

**Option A: prebuilt firmware.** Download the latest `.bfu` from [Releases](https://github.com/OmfoBalkanMicroElectronics/MagicJieLi/releases) and go to [Flashing](#flashing). Only use it on the same BT201 hardware.

**Option B: build from source.** This repository is the complete, buildable SDK tree. You don't need patches, overlays or file copying.

1. Install **Code::Blocks 16.01** (`codeblocks-16.01mingw-setup.exe`), then **open and close it once** so it creates its config file.
2. Install a **JieLi PI32V2 LTO toolchain** (`jl_toolchain_pi32v2_lto`) that supports the AC692x. Some distributions need a machine-bound licence (`*.key` → `*.lic`). MagicJL has also been built with a toolchain that needs no licence.
3. Open `BT_201_AC692x_SDK_release_V2.6.3/apps/sdk.cbp` in Code::Blocks.
4. Select the **`SDK_flash`** target and the **`Release`** configuration.
5. Run **Build → Rebuild** the first time, and again whenever libraries, linker settings or board config change. For plain `.c`/`.h` edits, a normal **Build** is enough.

If no compiler is selected, go to *Project → Build options → Compiler settings* and pick the JieLi PI32V2 profile.

The output is written to `apps/download/post_build/flash/`. `updata.bfu` is the update image, and `Map.txt` is the symbol/RAM map. Build outputs are git-ignored. Prebuilt images are published only through Releases.

Full guides: **[documentation index](docs/README.md)**.

## Flashing

> [!WARNING]
> Flashing the wrong image, losing power or miswiring D+/D- can brick the board. **Take and verify a full flash dump before flashing anything.** See [Backup & recovery](docs/08-backup-and-recovery.md).

There are two ways to flash:

1. **USB stick / SD card:** if the stock firmware supports JieLi's update mechanism, put `updata.bfu` on the storage and insert it.
2. **Arduino Nano boot dongle:** for boards where `.bfu` updates don't work. The Nano power-cycles the chip and sends the `0x16EF` boot key over the USB data lines within the first milliseconds after power-up. The chip then enters JieLi's UBOOT/programming mode.

These are different mechanisms. USB/SD uses the running firmware's own updater, while the dongle puts the chip straight into its built-in USB bootloader. **Measure D+ and D- before connecting; don't trust wire colours.**

Pinout, relay and relay-less schematics, and timing notes: [Firmware flashing guide](docs/02-flashing.md).

## Controls (ADKEY)

The button resistor ladder has a custom layout for this project.

| Resistor | Short press | Long press / hold |
| --- | --- | --- |
| 0 Ω | Play / pause | Mute |
| 3 kΩ | Previous track | Volume down |
| 6.2 kΩ | Next track | Volume up |
| 9.1 kΩ | Change mode | Cycle EQ preset |
| 15 kΩ | Next folder | Fast-forward |
| 24 kΩ | Previous folder | Rewind |
| 33 kΩ | Cycle repeat mode | — |
| 100 kΩ | Delete file (local music; asks for confirmation) | System info screen |

Folder, repeat and delete actions apply in local music mode. Key maps live in `apps/task_manager/task_*/task_*_key.c`. Edit them if your resistor network is different.

## How the spectrum analyzer works

The AC692x has no hardware FFT or FPU, so the analyzer runs entirely in software with fixed-point maths:

1. **Capture:** final PCM samples are read just before they enter the DAC queue (`audio_stream_write`). Only every 4th sample is stored into a 256-entry ring buffer, so the cost on the audio path is only a few dozen operations per buffer.
2. **Window:** a 256-point **Hann window** (Q14) reduces spectral leakage.
3. **Transform:** a 256-point **radix-2 decimation-in-time FFT** with Q15 twiddles and per-stage scaling.
4. **Bands:** bins are mapped to 10 log-spaced bands centred at 60, 125, 250, 500 Hz, 1, 1.6, 2.5, 4, 6 and 9 kHz. The mapping follows the current DAC sample rate.
5. **Display:** each band gets auto-gain against its own recent peak, smoothing and peak-hold. Overall bar height scales with volume.

The FFT runs **only on the UI refresh path, never inside the DAC interrupt**, so Bluetooth and local playback stay glitch-free.

**Tuning:**

```c
// apps/cpu/ui/lcd/lcd_disp.c: overall bar height
height_limit = 16 + sound.vol.sys_vol_l;   // 16 = minimum movement at low volume
if (height_limit > 48) {                   // 48 = max height without hitting the text row
    height_limit = 48;
}

// apps/cpu/audio/audio.c: display-only per-band gain in percent (bass → treble)
static const u8 audio_visualizer_band_gain_pct[10] = {
    100, 100, 100, 100, 100, 110, 120, 130, 140, 150,
};
```

Raising a gain only makes that bar look taller. It does not change the audio EQ.

## User interface

**Boot:** `Hosgeldiniz!` ("Welcome!") types in letter by letter, then `By OBM(BR4)` slides in before the normal mode screen appears.

**Bluetooth status:**

| State | Screen |
| --- | --- |
| Not connected | Spinning dotted ring, "waiting for connection" |
| Connected | Check mark and `BT Baglandi` ("BT connected") |
| Playing music | Music icon, device name / AVRCP info, spectrum analyzer |
| Call | Phone icon and call state |

**AVRCP now-playing:** if the phone sends metadata, title and artist scroll right-to-left in a small pixel font above the spectrum, stored up to 48 characters. If the phone sends no metadata, the connected device name is shown. The marquee is independent of the SDK's own scrolling-text control, so it doesn't tear while the spectrum redraws.

**System info:** a **long press on the 100 kΩ key** opens an auto-paging info screen over any mode, then returns to the previous UI:

```text
MAGICJL / DEVELOPED BY / OBM TEAM
DEVELOPER / MAINTAINER: / BARBAROR4
SW: MJL_BT201 / RELEASE V1.0 / 192MHZ PI32V2
128KB SRAM / 512KB FLASH / AC692 UNLOCKED ;)
```

## Local music player

Plays music from a USB stick, or from SD if enabled. The SDK's music task handles scanning, folders, decoding, resume points and display state. MagicJL adds:

- Play/pause, next/previous track and folder, hold-to-seek. The FF/FR icon clears when the key is released.
- File name, folder name, track number and play time on screen
- EQ preset indicator and long-press Mode to cycle presets
- Repeat modes: all, single track, random and folder
- **Two-step delete confirmation.** The first press never deletes anything.
- Spectrum visualizer after about five seconds of inactivity during playback. Any key returns to the normal screen.
- On-screen notices for decoder errors, device removal and missing files

> [!CAUTION]
> Deleting removes the file from the physical storage. Test with throwaway media first.

| File | Responsibility |
| --- | --- |
| `apps/task_manager/task_music/task_music.c` | Music messages, delete confirmation, file/folder actions |
| `apps/task_manager/task_music/task_music_key.c` | Local music ADKEY map |
| `apps/common/music_player/music_player.c` | Playback and delete back-end |
| `apps/common/music_player/music_decoder.c` | Decoder state, seek and play time |
| `apps/cpu/ui/lcd/lcd_disp.c` | File name, time, EQ, repeat, delete/seek icons and the visualizer |

## SDK configuration

All feature switches live in [`apps/include/sdk_cfg.h`](BT_201_AC692x_SDK_release_V2.6.3/apps/include/sdk_cfg.h). Defaults for the BT201 reference board:

| Feature | Status |
| --- | --- |
| 128×64 OLED (`UI_ENABLE`, `LCD_128X64_EN`) | On |
| Bluetooth Classic, 1 connection, volume sync | On |
| USB disk / USB PC mode | On |
| SD card (`SDMMC0_EN` / `SDMMC1_EN`) | Off. `SDMMC1` shares pins with the debug UART. |
| FM radio | Enabled in the SDK, but the BT201 has no FM RF parts |
| AUX / line-in | Off (no input circuit on the board) |
| Recording, echo, TWS | Off |
| Low-power / sniff mode | Off (enabling it disables the UI) |
| Debug UART | `UART2` on PC4/PC5 @ 115200 |

> [!CAUTION]
> Keep `EQ_UART_DEBUG = 0`. It uses UART1 on **PB0/PB1**, the same pins as the OLED.

See [OLED wiring & SDK configuration](docs/09-oled-and-sdk-config.md) for the full table, resource conflicts and how to enable SD on `SDMMC1`.

## Repository layout

```text
MagicJieLi/
├─ BT_201_AC692x_SDK_release_V2.6.3/   # Complete, buildable MagicJL SDK tree
│  ├─ apps/                            # Application source: MagicJL work happens here
│  ├─ include_lib/                     # Vendor library headers (ABI-sensitive)
│  └─ tools/                           # Vendor makefiles and libc
├─ hardware/arduino-nano-dongle/       # Boot dongle sketch
├─ docs/                               # Guides (English); docs/tr/ has the Turkish originals
├─ images/                             # Board photo and dongle schematics
├─ LICENSE-MAGICJL.md                  # MIT licence for original MagicJL work
├─ NOTICE.md                           # Third-party / vendor notice
└─ README.md
```

Key MagicJL files inside `apps/`:

| Path | Purpose |
| --- | --- |
| `cpu/ui/lcd/lcd_disp.c` | Screens, animations, system info, Bluetooth UI, visualizer drawing |
| `cpu/ui/lcd/i2c_oled.c` | Software-I²C OLED driver (PB0/PB1) |
| `cpu/audio/audio.c` | PCM capture, FFT and band computation |
| `cpu/ui/task_ui/bt_ui.c` | Stores device name and AVRCP title/artist |
| `btstack/user/bt_configs_user.c` | AVRCP metadata callback registration |
| `task_manager/task_*/*_key.c` | Per-mode ADKEY/IOKEY maps |
| `task_manager/task_common.c` | System info message shared by all modes |
| `include/sdk_cfg.h` | Board and feature switches |

**Editing safely:** do day-to-day work in `apps/`, and change `apps/include/` with care. Don't touch `include_lib/` unless you know the ABI. Use Rebuild after changing libraries, linker settings or board config.

## About the AC692x

According to JieLi's family documentation, the AC692x offers:

- 32-bit DSP-oriented core, 128 KB SRAM, 16 KB 4-way instruction cache
- 64 interrupt sources with 4 programmable priority levels
- Bluetooth 2.1 + 4.2 (Classic + BLE)
- 16-bit stereo DAC, 12-bit stereo ADC with mic amplifier
- USB full-speed OTG, 3× UART, SPI, SD host, SPI flash interface
- 16-channel 10-bit ADC (used for ADKEY and voltage sensing)
- RTC, watchdog, PWM, I²C, SPDIF in, GPIO wake-up
- Built-in FM receiver (76–108 MHz; not usable at the same time as Bluetooth)

These are **family capabilities**. What actually works depends on the parts on your board, its pin mapping and `sdk_cfg.h`. For example, turning FM on in software does nothing without the RF parts.

**PI32V2** is JieLi's in-house CPU architecture: 16 general-purpose registers, 64-bit operations on register pairs, and fixed-point DSP arithmetic. That's why the visualizer uses Q15 maths throughout instead of floats.

Vendor datasheets and SDK manuals are **not** included, because the vendor marks them as non-redistributable. Get them from your SDK package or the vendor.

## The MagicJL family

This repository is **MagicJL v1.0**, the first generation. Two successors are in development on the same AC692x SDK:

| Generation | Display | Status |
| --- | --- | --- |
| **MagicJL** v1.0 (this repo) | 128×64 I²C OLED | Released |
| **MagicJL Plus** | 128×64 SSD1306 OLED | Tested on AC6925A, not yet published; a port to the MagicJL PLUS board (AC6921A8, SH1106 SPI OLED) is planned: external SPI-flash font pack with 37 UI languages, launcher with background play, DS3231 clock and calendar, text reader, favourites, custom EQ |
| **MagicJL Color 2.0** | 1.69" ST7789 240×280 IPS | In development, not yet tested on hardware: full-colour animated GUI, PC simulator and MagicJL Studio, AC6921A + PCM5102A build |

## Roadmap

- Track progress bar from AVRCP position/duration
- Selectable spectrum styles: mirror, waveform, VU meter
- Sliding transitions between system info pages
- Settings menu for brightness and visualizer style
- Idle clock / screensaver
- Hidden long-press animation: `BLACKFIN AWAKENS`

## Contributing

Bug reports, photos, ports to other AC692x boards and ideas are all welcome. Please open an issue first. When reporting a problem, include your board, display, flash size and the relevant `sdk_cfg.h` changes.

Before submitting a pull request, **do not add proprietary SDK files, vendor PDFs or binaries** that aren't already in the tree.

## Credits

- **[kagaimiq/jl-uboot-tool](https://github.com/kagaimiq/jl-uboot-tool)** and **[kagaimiq/jl-misctools](https://github.com/kagaimiq/jl-misctools)**: essential open tools for JieLi UBOOT access and firmware unpacking, used in the [backup guide](docs/08-backup-and-recovery.md)
- **Zhuhai JieLi Technology**: the AC692x SoC and the BT201 SDK this project builds on

---

**MagicJL** · developed by the **OBM Team** · maintained by **[@Barbaror4](https://github.com/Barbaror4)**

`AC692 UNLOCKED ;)`
