# OLED bağlantısı ve SDK yapılandırması

Bu rehber MagicJL'in OLED'li BT201 / AC6925 referans kartı içindir. Kendi
kartınızda pin, ekran sürücüsü ve çevre birimleri farklı olabilir.

## 128×64 I²C OLED bağlantısı

MagicJL, yazılımsal I²C OLED sürücüsünü kullanır. Kaynakta
[`i2c_oled.c`](../BT_201_AC692x_SDK_release_V2.6.3/apps/cpu/ui/lcd/i2c_oled.c)
içinde açıkça tanımlandığı gibi hatlar şöyledir:

| BT201 / AC6925 pini | Kart üzerindeki UART etiketi | OLED pini | Görev |
| --- | --- | --- | --- |
| PB0 | TX | SCL | I²C saat |
| PB1 | RX | SDA | I²C veri |
| 3.3 V | 3V3 | VCC | OLED beslemesi |
| GND | GND | GND | Ortak toprak |

```text
BT201 UART header                 I²C OLED
─────────────────                 ────────
PB0  (TX)  ─────────────────────► SCL
PB1  (RX)  ─────────────────────► SDA
3.3 V      ─────────────────────► VCC
GND        ─────────────────────► GND
```

> [!IMPORTANT]
> Header üzerinde `TX` ve `RX` yazsa da MagicJL bunları bu kartta UART için
> değil, OLED'in I²C hatları olarak kullanır. OLED'i **3.3 V** ile besleyin ve
> mutlaka ortak GND bağlayın. 5 V I²C seviye kullanan ekran/bağlantı AC6925
> pinlerine zarar verebilir.

> [!CAUTION]
> `sdk_cfg.h` içinde `EQ_UART_DEBUG` değerini `1` yapmayın. Bu özellik varsayılan
> olarak `UART1_TXPB0_RXPB1` kullanır ve OLED'in PB0/PB1 hatlarıyla çakışır.

### Ekran boşsa

1. SCL/SDA'nın ters bağlanmadığını kontrol edin: **PB0 → SCL**, **PB1 → SDA**.
2. OLED VCC'nin 3.3 V, GND'nin ortak olduğundan emin olun.
3. `UI_ENABLE = 1` ve `LCD_128X64_EN = 1` olmalı.
4. `EQ_UART_DEBUG = 0` kalmalı.
5. Kaynakta ekran sürücüsünün
   [`i2c_oled.c`](../BT_201_AC692x_SDK_release_V2.6.3/apps/cpu/ui/lcd/i2c_oled.c)
   olarak projeye dahil edildiğini kontrol edin.

## `sdk_cfg.h` rehberi

Ana yapılandırma dosyası:

[`BT_201_AC692x_SDK_release_V2.6.3/apps/include/sdk_cfg.h`](../BT_201_AC692x_SDK_release_V2.6.3/apps/include/sdk_cfg.h)

Her değişiklikten önce çalışan `.bfu` veya tam UBOOT flash dump yedeği alın.
Birden fazla özellik değiştirildiğinde **Build → Rebuild** kullanın.

### MagicJL için önemli anahtarlar

| Anahtar | Mevcut değer | Etki / dikkat |
| --- | ---: | --- |
| `UI_ENABLE` | `1` | Tüm UI altyapısı. `0` olursa OLED de kapanır. |
| `LCD_128X64_EN` | `1` | 128×64 nokta matris OLED/LCD ekranı etkinleştirir. |
| `LED_7_EN` / `LED_1888_EN` | `0` / `0` | Yedi-segment veya 1888 ekran sürücüleri; OLED kartta kapalı kalır. |
| `LRC_LYRICS_EN` | `0` | Yerel müzik LRC söz desteği. Açmak RAM/UI davranışını etkileyebilir. |
| `EQ_UART_DEBUG` | `0` | Açılırsa varsayılan UART1 PB0/PB1 ile OLED çakışır. |
| `USB_DISK_EN` | `1` | USB bellekten müzik/güncelleme akışı. |
| `USB_PC_EN` | `1` | USB PC modu. USB debug seçilirse SDK bu modları kapatabilir. |
| `SDMMC0_EN` / `SDMMC1_EN` | `0` / `0` | SD kart arayüzleri. `SDMMC1`, aktif UART debug ile pin paylaşır; aşağıdaki adımları izleyin. |
| `ADKEY_SD_MULT_EN` | `0` | ADKEY ve SD pin paylaşımı; direnç bölücüsü/SD uyumluluğu test edilmeden değiştirmeyin. |
| `BT_TWS` | `0` | TWS eş-hoparlör modu; RAM ve EQ ile bağımlılıkları vardır. |
| `BT_CONNTCT_NUM` | `1` | Aynı anda Bluetooth bağlantı sayısı. |
| `BT_BACKGROUND_EN` | `0` | Bluetooth arka plan modu; TWS/echo/FM ile kaynak çakışmaları olabilir. |
| `BT_MUSIC_VOL_SYNC` | `1` | Telefon ile müzik sesini senkronlar. |
| `FM_RADIO_EN` / `FM_INSIDE` | `1` / `1` | SDK'de etkin; referans BT201 kartında FM RF elemanları yoktur. Donanım eklemeden kullanılamaz. |
| `AUX_AD_ENABLE` | `0` | Referans kartta AUX giriş devresi yoktur; kapalı kalmalı. |
| `REC_EN` | `0` | Kayıt altyapısı. Açılması RAM, giriş ve dosya sistemi gerektirir. |
| `ECHO_EN` | `0` | Echo/reverb. Yazılım EQ, BLE ve TWS ile kaynak kısıtları vardır. |
| `UART_UPDATA_EN` | `0` | SDK'nin UART update seçeneği; Arduino Nano UBOOT dongle yöntemi bundan bağımsızdır. |
| `BT_LOW_POWER_MODE` / `SNIFF_MODE_CONF` | `0` / `0` | Açılırsa SDK, UI desteğini otomatik kapatır. MagicJL OLED için kapalı kalmalı. |
| `SYS_Hz`, `MUSIC_DECODE_Hz` | `192000000L` | MagicJL UI ve FFT/spektrum için test edilmiş saat. Düşürmek akıcılığı etkileyebilir. |

### Kaynak kısıtları

`sdk_cfg.h` başındaki SDK notlarına göre aşağıdaki birleşimler dikkat ister:

- Bluetooth arka plan modu ile dahili FM birlikte kullanılmaz.
- Arka plan modu, echo ve TWS aynı anda kullanılamaz.
- Echo açıkken yazılım EQ ve BLE ile RAM çakışması oluşabilir.
- TWS açıkken yazılım EQ desteklenmez.
- FM kaydı desteklenmez.
- Line-in ve mikrofon kaydı aynı anda kullanılamaz.

> [!TIP]
> Bir özelliği açmadan önce yalnızca ilgili `#define` satırını değiştirin,
> Rebuild yapın ve önce kısa fonksiyon testi uygulayın. Birden fazla makroyu
> aynı anda değiştirmek hata kaynağını takip etmeyi zorlaştırır.

### SDMMC1 ve UART debug paylaşımı

MagicJL'in mevcut ayarında sistem debug çıktısı açıktır:

```c
#define __DEBUG

#ifdef __DEBUG
#define DEBUG_UART_SEL  UART2_TXPC4_RXPC5
#define DEBUG_UART_RATE 115200
#endif
```

Bu debug UART, SDK'nin `SDMMC1` arayüzüyle aynı pin kaynağını paylaşır. SD kart
için `SDMMC1` kullanmak isteyenler `sdk_cfg.h` içinde şu iki değişikliği birlikte
yapmalıdır:

```c
//#define __DEBUG        // UART debug çıktısını kapat

#define SDMMC0_EN 0
#define SDMMC1_EN 1      // SDMMC1'i etkinleştir
```

Ardından **Build → Rebuild** yapın. `__DEBUG` kapalıyken `log_printf(...)` boş
makroya dönüşür; UART üzerinden hata/tuş logu gelmez. Yeni özellik geliştirirken
veya key/message eşlemesi izlerken debug'ı tekrar açmak çok faydalıdır.

> [!IMPORTANT]
> Bu işlem OLED'i etkilemez: OLED PB0/PB1 üzerindedir, sistem debug UART ise
> varsayılan MagicJL yapılandırmasında PC4/PC5 kullanır. Ancak `SDMMC1` için
> kart üzerinde uygun SD soketi ve bağlantıların gerçekten bulunması gerekir.

## Debug UART notu

Genel debug çıktısı, mevcut yapılandırmada `UART2_TXPC4_RXPC5` ve `115200`
baud kullanır. Bu, OLED'in PB0/PB1 hatlarından ayrıdır. OLED bağlıyken PB0/PB1
üzerinden UART1 debug/EQ debug kullanmayın.
