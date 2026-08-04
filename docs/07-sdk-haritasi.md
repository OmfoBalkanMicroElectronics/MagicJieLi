# SDK Haritası

| Dizin | Görev | GitHub yaklaşımı |
| --- | --- | --- |
| `apps/` | Uygulama kaynağı, görevler, UI, keymap, board ayarları | MagicJL içinde tam kopya olarak tutulur |
| `apps/obj/` | Derleme objeleri | Git'e eklenmez |
| `apps/download/post_build/flash/` | Firmware ve post-build çıktıları | Git'e eklenmez; Release için kullanılır |
| `include_lib/` | Vendor prebuilt library header/API'leri | Stok SDK'de kalır |
| `tools/` | Vendor compiler, libc ve platform araçları | Stok SDK'de kalır |

Çoğu MagicJL düzenlemesi `apps/` altında olduğundan, tüm `apps/` ağacını paylaşmak patch yönetiminden daha kolaydır. Ancak `apps/obj/` ve üretilmiş flash çıktıları depoya alınmamalıdır.
