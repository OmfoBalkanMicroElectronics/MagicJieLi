# SDK Haritası

| Dizin | Görev | GitHub yaklaşımı |
| --- | --- | --- |
| `apps/` | Uygulama kaynağı, görevler, UI, keymap, board ayarları | Tam MagicJL SDK ağacının parçası olarak tutulur |
| `apps/obj/` | Derleme objeleri | Git'e eklenmez |
| `apps/download/post_build/flash/` | Firmware ve post-build çıktıları | Git'e eklenmez; Release için kullanılır |
| `include_lib/` | Vendor prebuilt library header/API'leri | SDK ile birlikte tutulur |
| `tools/` | Vendor compiler, libc ve platform araçları | SDK ile birlikte tutulur |

MagicJL doğrudan tam BT201 SDK ağacı olarak dağıtılır; patch veya ayrı uygulama katmanı gerekmez. `apps/obj/` ve üretilmiş flash çıktıları depoya alınmaz.
