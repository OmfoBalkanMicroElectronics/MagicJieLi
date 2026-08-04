# MagicJL SDK paketi ve isteğe bağlı overlay

## Normal kullanıcılar

Normal kullanıcılar için önerilen yöntem GitHub Releases'tan tam `MagicJL-BT201-SDK-vX.Y.zip` paketini indirip doğrudan kullanmaktır. SDK zaten MagicJL değişikliklerini içerir; stok SDK üzerine manuel dosya uygulamak gerekmez.

## Geliştiriciler için overlay yöntemi

Bu repo, değiştirilen tüm uygulama katmanını `MagicJL\apps\` altında tutar. Bu klasör, stok SDK'deki `apps\` klasörünün üstüne uygulanır.

## Klasör yapısı

```text
MagicJL-BT201\
├─ MagicJL\apps\          <- Bu depodan gelen tam uygulama katmanı
├─ tools\Install-MagicJL.ps1
└─ docs\

C:\Jieli\BT_201_AC692x_SDK_release_V2.6.3\
├─ apps\                   <- Betiğin güncelleyeceği klasör
├─ include_lib\
└─ tools\
```

## PowerShell ile uygulama

PowerShell'i açın ve depo kökünde çalıştırın:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\tools\Install-MagicJL.ps1 -SdkPath "C:\Jieli\BT_201_AC692x_SDK_release_V2.6.3"
```

Betiğin yaptığı işlemler:

1. SDK yolunu ve `apps\sdk.cbp` dosyasını kontrol eder.
2. Stok `apps\` klasörünün zaman damgalı yedeğini oluşturur.
3. `MagicJL\apps\` içeriğini stok `apps\` klasörüne kopyalar.
4. Hiçbir dosyayı silmez; yalnızca aynı isimli MagicJL dosyalarının üstüne yazar.

Yedek almak istemiyorsanız:

```powershell
.\tools\Install-MagicJL.ps1 -SdkPath "C:\Jieli\BT_201_AC692x_SDK_release_V2.6.3" -NoBackup
```

> Betik yalnızca `apps/` katmanını uygular. `include_lib/` ve vendor `tools/` stok SDK'de kalmalıdır.
