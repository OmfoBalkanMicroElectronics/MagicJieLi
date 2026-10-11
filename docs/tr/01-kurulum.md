# Kurulum

## Gerekli dosyalar

- Bu MagicJL deposu (tam SDK ağacıdır; ayrıca stok SDK, patch veya overlay gerekmez)
- Code::Blocks 16.01
- AC692x ile uyumlu Jieli PI32V2 LTO toolchain

## Projenin yeri

Depoyu boşluk içermeyen kısa bir yola klonlayın veya ZIP olarak çıkarın. Örnek:

```text
C:\Jieli\MagicJieLi\
```

`BT_201_AC692x_SDK_release_V2.6.3\` klasörünün içinde `apps\sdk.cbp`, `apps\`, `include_lib\` ve `tools\` bulunmalıdır.

## IDE ve toolchain

1. Code::Blocks'u kurun ve bir kez açıp kapatın.
2. PI32V2 LTO toolchain'i kurun.
3. Toolchain paketiniz lisans istiyorsa paketinin talimatını uygulayın. MagicJL, lisans istemeyen bir toolchain kurulumu ile de derlenmiştir.
4. `apps\sdk.cbp` dosyasını Code::Blocks ile açın.

Detay: [SDK haritası](07-sdk-haritasi.md).
