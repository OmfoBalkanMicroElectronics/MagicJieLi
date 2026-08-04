# MagicJL Uygulama Katmanı

Bu klasörün içine, geliştirdiğiniz SDK'deki **tam `apps/` klasörünü** aşağıdaki biçimde kopyalayın:

```text
MagicJL\apps\
├─ cpu\
├─ task_manager\
├─ common\
├─ btstack\
├─ include\
├─ sdk.cbp
└─ Makefile
```

`tools\Install-MagicJL.ps1`, bu `apps/` klasörünü stok SDK'nin `apps/` klasörüne uygular.

Kopyalamayın:

- `apps\obj\`
- `apps\download\post_build\flash\` içindeki `.bin`, `.bfu`, `.app`, `.exe`, `.map` ve benzeri derleme çıktıları
