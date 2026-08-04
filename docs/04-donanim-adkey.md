# Donanım ve ADKEY

MagicJL, OLED'li BT201 / AC6925 tabanlı kart için hazırlanmıştır. Başka AC692x kartlarda LCD resource, pin, flash, direnç ağı ve çevre birimleri farklı olabilir.

## ADKEY eşlemesi

| Direnç | İşlev |
| --- | --- |
| 100K | Müzikte silme isteği; uzun basışta sistem bilgisi |
| 33K | Tekrar modu |
| 24K | Önceki klasör; tutunca geri sarma |
| 15K | Sonraki klasör; tutunca ileri sarma |
| 9.1K | Mod; uzun basışta EQ |
| 6.2K | Sonraki parça; tutunca ses artır |
| 3K | Önceki parça; tutunca ses azalt |
| 0R | Oynat / duraklat |

Tuş tabloları `apps\task_manager\task_*\*_key.c` altındadır.

## IR ve UART

SDK, IR girişini PA4 olarak tanımlayabilir; uygun 38 kHz IR alıcı ve doğru kumanda eşlemesi gerekir. UART pin seçimi `apps\include\sdk_const_define.h` içindedir. Karttaki gerçek kablolama ve voltage seviyeleri doğrulanmadan bağlantı yapılmamalıdır.
