# Kurulum

## Gerekli dosyalar

- Bu MagicJL deposu
- OLED destekli, dokunulmamış BT201 stok SDK paketi
- Code::Blocks 16.01
- AC692x ile uyumlu Jieli PI32V2 LTO toolchain

## Stok SDK'nin yeri

Stok SDK'yi GitHub Releases'tan veya kendi arşivinizden çıkarın. Örnek:

```text
C:\Jieli\BT_201_AC692x_SDK_release_V2.6.3\
```

Bu klasörün içinde `apps\sdk.cbp`, `apps\`, `include_lib\` ve `tools\` bulunmalıdır.

## IDE ve toolchain

1. Code::Blocks'u kurun ve bir kez açıp kapatın.
2. PI32V2 LTO toolchain'i kurun.
3. Toolchain paketiniz lisans istiyorsa paketinin talimatını uygulayın. MagicJL, lisans istemeyen bir toolchain kurulumu ile de derlenmiştir.
4. `apps\sdk.cbp` dosyasını Code::Blocks ile açın.

Detay: [SDK haritası](07-sdk-haritasi.md).
