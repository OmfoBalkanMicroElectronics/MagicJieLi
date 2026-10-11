# Third-party notice

This repository contains a JieLi AC692x / BT201 SDK tree (`BT_201_AC692x_SDK_release_V2.6.3/`) together with the MagicJL custom firmware work.

## What is covered by which terms

| Material | Terms |
| --- | --- |
| Original MagicJL source additions, documentation (`docs/`, READMEs), the Arduino Nano dongle sketch (`hardware/`) and original artwork/schematics (`images/`) | MIT, see [LICENSE-MAGICJL.md](LICENSE-MAGICJL.md) |
| The vendor SDK: its original source, prebuilt libraries (`*.a`), headers in `include_lib/`, makefiles and libc in `tools/`, resource files and formats | Their respective owners' terms. **No open-source licence is granted or implied for them.** |
| JieLi toolchain/compiler | Not included; obtain it from the vendor under the vendor's terms |
| Vendor datasheets and SDK manuals | Not included; the vendor marks them as non-redistributable |

Where a MagicJL change modifies a vendor source file, the MIT licence covers only the MagicJL changes, not the underlying vendor code. Before redistributing or commercially using any vendor component, make sure you have permission from its rights holder.

## Public repository note

MagicJL is an unofficial community firmware/customization project and is not affiliated with or endorsed by Zhuhai JieLi Technology. "JieLi" and product names are used only to identify compatible hardware.

If a valid rights-holder request requires removal of bundled vendor material, the SDK content may be removed from the public repository while being retained in a private archival backup.
