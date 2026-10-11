# SDK map

| Directory | Purpose | In git? |
| --- | --- | --- |
| `apps/` | Application source: tasks, UI, key maps, board config | Yes. This is where MagicJL work happens |
| `apps/include/` | App configuration and headers (`sdk_cfg.h`) | Yes. Edit with care |
| `apps/obj/` | Build objects | No (ignored) |
| `apps/download/post_build/flash/` | Firmware and post-build outputs | Resources yes; built images no (published through Releases) |
| `include_lib/` | Headers for the vendor's prebuilt libraries (ABI-sensitive) | Yes, as part of the SDK |
| `tools/` | Vendor makefiles and libc | Yes, as part of the SDK |

MagicJL ships as the complete BT201 SDK tree, so no patch or separate application layer is needed.

## Editing rules of thumb

- Do day-to-day development in `apps/`.
- Only change `include_lib/` if you understand the library ABI it describes.
- **Rebuild** after changing library files, linker settings or board configuration.
- Keep a backup of the working firmware and resource package before every flash.
