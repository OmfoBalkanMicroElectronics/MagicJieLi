# Building & flashing

1. Open `BT_201_AC692x_SDK_release_V2.6.3\apps\sdk.cbp` in Code::Blocks.
2. Select the **`SDK_flash`** target and the **`Release`** configuration.
3. Use **Build → Rebuild** for the first build, and again whenever linker or library settings change.
4. For plain C/header changes, a normal **Build** is enough.
5. If the build succeeds, the firmware is written to `apps\download\post_build\flash\`.

If the board is connected in programming mode, the toolchain may download the firmware automatically. Look for `download complete` in the build log. Otherwise, flash `updata.bfu` over USB/SD or use the Arduino Nano dongle. See [Flashing](02-flashing.md).

## Important

- Back up the working firmware before flashing.
- `updata.bfu`, `jl_692x.bin` and similar files are build outputs. Don't commit them (`.gitignore` already excludes them).
- Check `apps\download\post_build\flash\Map.txt` for RAM and symbol usage.
