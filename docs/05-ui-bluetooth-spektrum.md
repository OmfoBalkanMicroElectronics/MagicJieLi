# UI, Bluetooth ve Spektrum

MagicJL; animasyonlu açılış, yükleme halkası, Bluetooth bağlantı ekranları, çağrı/müzik ikonları, AVRCP metadata ve sistem bilgi ekranı ekler.

## AVRCP Now Playing

Bluetooth müzik sırasında AVRCP title/artist bilgisi alınır. Başlık ve sanatçı sırayla, özel piksel marquee ile akar. Telefon metadata göndermezse bağlı cihaz adı kullanılır.

## FFT spektrum

PCM verisi DAC öncesinde örneklenir. 256 noktalı Q15 FFT, LCD yenileme yolunda çalışır; ses kesmesinde tam FFT yapılmaz. On bant, smoothing ve peak-hold ile çizilir.

Bar yüksekliği:

```c
height_limit = 16 + sound.vol.sys_vol_l;
if (height_limit > 48) {
    height_limit = 48;
}
```

Bu değer `apps/cpu/ui/lcd/lcd_disp.c` içindedir. Frekans bandı kazançları `apps/cpu/audio/audio.c` içindeki `audio_visualizer_band_gain_pct` tablosundadır.
