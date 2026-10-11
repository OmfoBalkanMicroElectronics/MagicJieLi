# Stock firmware backup & recovery

This method has been tested on MagicJL's reference **AC6925 / BT201** board. The goal is a **verified full flash backup** of the chip before you change any firmware.

> [!CAUTION]
> Do **not** use `erase` or `erasechip`. Only use `write` the way the restore section below does, with a **verified full dump taken from the same board**. A wrong address, a wrong file or a power cut can brick the chip.

## Requirements

- The Arduino Nano boot dongle, wired as in the [flashing guide](02-flashing.md)
- Python 3
- [kagaimiq/jl-uboot-tool](https://github.com/kagaimiq/jl-uboot-tool)
- [kagaimiq/jl-misctools](https://github.com/kagaimiq/jl-misctools)

You can put `jl-misctools` inside the `jl-uboot-tool` directory as a subfolder. The examples below assume that layout.

## 1. Prepare the PC

In PowerShell, go to the `jl-uboot-tool` directory and install its dependencies:

```powershell
py -m pip install -r requirements.txt
New-Item -ItemType Directory -Force backups
```

## 2. Put the chip into UBOOT mode

1. Connect the Arduino Nano dongle to the target board using the [relay or relay-less schematic](02-flashing.md).
2. Select boot mode. The red LED means programming mode.
3. When connected to the PC, Windows should show a JieLi UBOOT/UDISK/DEVICE-type device.
4. Note the drive letter Windows assigns in File Explorer. The examples below use `D:`.

On some Windows 11 installs, `jldevfind.py` can't find the UBOOT device automatically. Using the raw device path directly is more reliable:

```powershell
py .\jluboottool.py --device \\.\D: "dump 0x0 0x40"
```

This only prints the first 64 bytes to the screen. It's a safe way to confirm you're talking to the right device. Replace `D:` with the drive letter Windows gave you.

> [!TIP]
> If the device doesn't appear, check three things first: D+/D- orientation, that the target's power is fully cut, and that `0x16EF` is sent right after power-up.

## 3. Take two verifiable flash dumps

On this BT201 configuration the flash is **512 KB = `0x80000`** bytes. Take two separate, read-only dumps:

```powershell
py .\jluboottool.py --device \\.\D: "read 0x0 0x80000 .\backups\BT201-stock-a.bin"
py .\jluboottool.py --device \\.\D: "read 0x0 0x80000 .\backups\BT201-stock-b.bin"
```

The `dump ...` / `read ...` part must be **one double-quoted string**, because the tool passes it to its UBOOT shell as a single command. If the tool shows a device picker, choose the JieLi UBOOT device. On Windows 11 the direct `--device \\.\D:` form is recommended.

Compare the SHA-256 of both files:

```powershell
Get-FileHash .\backups\BT201-stock-a.bin -Algorithm SHA256
Get-FileHash .\backups\BT201-stock-b.bin -Algorithm SHA256
```

> [!IMPORTANT]
> If these two **initial stock dumps** don't have identical hashes, the backup is not valid. Fix the dongle wiring and power sequencing, then take two new dumps. Store one of the matching dumps somewhere safe and read-only.

## 4. Inspect the dump

Unpack the backup with the AC690x/AC691x/AC692x parser from `jl-misctools`:

```powershell
py .\jl-misctools-main\firmware\fwunpack690.py .\backups\BT201-stock-a.bin
```

The tool extracts parts such as `jlfirmware.yaml` (a description of the firmware layout), `uboot.boot`, `ver.bin` and `app_data/` next to the dump. It does not modify the backup file.

## 5. Tested restore procedure

This procedure was tested successfully on MagicJL's reference AC6925 / BT201 with a full 512 KB dump. Only ever use a backup **from the same board**:

```powershell
py .\jluboottool.py --device \\.\D: "write 0x0 .\backups\BT201-stock-a.bin"
```

Don't disconnect power, USB or the Nano until the erase/write progress finishes. Then boot the board in normal mode and check that the firmware runs.

### Post-restore verification

Enter UBOOT mode again and take two fresh reads:

```powershell
py .\jluboottool.py --device \\.\D: "read 0x0 0x80000 .\backups\BT201-verify-a.bin"
py .\jluboottool.py --device \\.\D: "read 0x0 0x80000 .\backups\BT201-verify-b.bin"
Get-FileHash .\backups\BT201-verify-a.bin -Algorithm SHA256
Get-FileHash .\backups\BT201-verify-b.bin -Algorithm SHA256
```

If these two `verify` hashes match, the flash contents are stable after the restore. On the reference BT201, the original dump and the first verify dump differed by only **62 bytes**, all in a small metadata record at `0x740D0–0x74113`. The firmware booted normally, and the Bluetooth MAC and pairing data were preserved.

> [!NOTE]
> So don't use "original dump hash == verify dump hash" as the only success test after a restore. First confirm that the two initial stock dumps matched each other. Then confirm that the two new verify dumps match each other.

## Recovery principle

Keep the working stock dump before installing MagicJL. Only write firmware back to the same board, and only when the backup has been verified and the restore flow has been tested. Never use another BT201's dump, because flash size, Bluetooth settings and board resources can differ.
