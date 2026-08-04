# Firmware yükleme yöntemleri

MagicJL, doğrudan depodaki tam BT201 SDK ağacıdır. Ayrı bir `apps/` kopyalama
betiği ya da patch uygulanmaz.

> [!WARNING]
> Yükleme yanlış firmware, kesilen güç veya hatalı bağlantı nedeniyle cihazı
> açılmaz hâle getirebilir. Başlamadan önce çalışan firmware'in yedeğini alın.

## 1. USB veya SD kart ile güncelleme

Derleme sonunda `apps\download\post_build\flash\updata.bfu` oluşur.
Kartınız bu yöntemi destekliyorsa bu dosyayı USB disk veya SD kart üzerinden
Jieli'nin update mekanizmasına verin.

- Dosya adı: `updata.bfu`
- USB/SD kart dosya sistemi ve takma sırası kartın stok firmware'ine bağlıdır.
- Güncelleme esnasında gücü kesmeyin.

> [!TIP]
> GitHub Release içindeki `.bfu` dosyası, derleme yapmadan denenebilecek hazır
> firmware paketidir. Yalnızca aynı BT201/AC692x donanım düzeni için kullanın.

## 2. Arduino Nano programlama dongle'ı

Bazı kartlarda `updata.bfu` mekanizması çalışmayabilir. MagicJL'in referans
BT201 kartında bu durumda Arduino Nano tabanlı özel bir dongle kullanılabilir.
Dongle, çip açılır açılmaz USB veri hatları üzerinden `0x16EF` önyükleme
anahtarını gönderir ve çipi programlama moduna geçirir.

Temel sıralama:

1. Çipin bilgisayar USB bağlantısını ayırın.
2. Çip beslemesini neredeyse 0 V olacak şekilde kesin.
3. Beslemeyi tekrar verin.
4. İlk birkaç milisaniye içinde `D+ = clock`, `D- = data` olacak biçimde
   `0x16EF` gönderin.
5. Çip D+ veya D- hattını aşağı çektiğinde bilgisayar USB bağlantısını yapın.

> [!IMPORTANT]
> D+ ve D- renkleri kabloya göre değişebilir. Bu projedeki dongle eşlemesinde
> `D- = beyaz = Arduino D11`, `D+ = yeşil = Arduino D10` kullanılır. Önce
> bağlantıyı ölçerek doğrulayın; ters bağlamak programlama moduna geçmeyi
> engeller.

### Arduino Nano pin eşlemesi

| Arduino Nano | Görev |
| --- | --- |
| D11 | Çip D- / veri |
| D10 | Çip D+ / saat |
| A5 | Çip beslemesini aç/kapatma kontrolü (`0 = açık`) |
| D2 | USB bağlantı rölesi kontrolü |
| D3 | Harici pull-up kontrolü |
| A0 | Mod anahtarı (`1 = normal`, `0 = boot`) |
| D5 | Kırmızı LED: boot/programlama modu |
| D4 | Yeşil LED: normal mod |

Sketch: [MagicJL_Boot_Dongle.ino](../hardware/arduino-nano-dongle/MagicJL_Boot_Dongle.ino)

### Şemalar

Şema görselleri eklendiğinde aşağıdaki adlarla `images/flash/` altına konur:

- `arduino-nano-releli.png` — USB ve çip beslemesini röle ile yöneten sürüm
- `arduino-nano-rolesiz.png` — sadeleştirilmiş, rölesiz sürüm

Direnç değerleri kritik değildir; 2.2 kΩ–10 kΩ aralığı çalışır. Referans
devrede R1 için 1 kΩ tercih edilir. Röle beslemesi, kullanılan rölenin
gerilimine göre ayrıca sağlanmalıdır.

> [!NOTE]
> Dongle; AC6905, AC6925 ve AC6956 ile denenmiş bir yaklaşımdır. Başarısız
> olursa önce güç kesme süresini, anahtarın gönderildiği ilk milisaniyeleri ve
> D+/D- yönünü kontrol edin. Ucuz bir lojik analizör bu aşamada çok faydalıdır.
