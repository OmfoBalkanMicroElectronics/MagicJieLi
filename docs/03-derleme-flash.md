# Derleme ve Flash

1. Depodaki `BT_201_AC692x_SDK_release_V2.6.3\apps\sdk.cbp` dosyasını Code::Blocks ile açın.
2. Hedef olarak `SDK_flash`, yapılandırma olarak `Release` seçin.
3. İlk derlemede veya linker/kütüphane ayarı değiştiğinde **Build -> Rebuild** kullanın.
4. Sadece C/header değişikliğinde normal **Build** yeterlidir.
5. Hata yoksa firmware çıktısı `apps\download\post_build\flash\` altında oluşur.

Kart yükleme modunda bağlıysa toolchain otomatik yükleme yapabilir. Değilse USB/SD üzerinden `updata.bfu` veya Arduino Nano dongle yöntemini kullanın. Ayrıntılar: [Firmware yükleme](02-firmware-yukleme.md).

## Önemli

- Flashlamadan önce çalışan firmware yedeği alın.
- `updata.bfu`, `jl_692x.bin` ve benzeri çıktılar derleme ürünüdür; Git'e eklemeyin.
- RAM/simge kullanımı için `apps\download\post_build\flash\Map.txt` dosyasını kontrol edin.
