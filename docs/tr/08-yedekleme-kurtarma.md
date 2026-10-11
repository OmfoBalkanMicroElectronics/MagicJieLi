# Stok firmware yedekleme ve kurtarma

Bu yöntem, MagicJL'in referans **AC6925 / BT201** kartında denenmiştir. Amaç,
firmware değiştirmeden önce çipin tüm flash içeriğinin doğrulanmış bir yedeğini
almaktır.

> [!CAUTION]
> `erase` ve `erasechip` komutlarını kullanmayın. `write` yalnızca bu rehberin
> restore testi bölümündeki gibi, **aynı karttan alınmış doğrulanmış tam dump**
> ile kullanılmalıdır. Yanlış adres, yanlış dosya veya güç kesintisi çipi
> kullanılmaz hâle getirebilir.

## Gerekenler

- Arduino Nano flash dongle ve [bağlantı şeması](02-firmware-yukleme.md)
- Python 3
- [kagaimiq/jl-uboot-tool](https://github.com/kagaimiq/jl-uboot-tool)
- [kagaimiq/jl-misctools](https://github.com/kagaimiq/jl-misctools)

`jl-misctools`, `jl-uboot-tool` dizininin içine alt klasör olarak da
konulabilir; aşağıdaki örnek bu düzeni kullanır.

## 1. Bilgisayarı hazırlama

PowerShell ile `jl-uboot-tool` dizinine girin ve bağımlılıkları kurun:

```powershell
py -m pip install -r requirements.txt
New-Item -ItemType Directory -Force backups
```

## 2. Çipi UBOOT moduna alma

1. Arduino Nano dongle'ı, [röleli veya rölesiz şemaya](02-firmware-yukleme.md)
   göre hedef karta bağlayın.
2. Boot modunu seçin; kırmızı LED programlama modunu belirtir.
3. Bilgisayara bağlandığında Windows'ta Jieli UBOOT/UDISK/DEVICE benzeri bir
   aygıt görünmelidir.
4. Windows Gezgini'nden aygıta atanmış sürücü harfini not alın. Aşağıdaki
   örnekte bu harf `D:` olarak kullanılmıştır.

`jldevfind.py` bazı Windows 11 kurulumlarında UBOOT aygıtını otomatik
bulamayabilir. Bu yüzden Windows'ta doğrudan aygıt yolu kullanmak daha
güvenilirdir:

```powershell
py .\jluboottool.py --device \\.\D: "dump 0x0 0x40"
```

Bu komut ilk 64 baytı yalnızca ekrana yazdırır; doğru aygıta erişilebildiğini
doğrulamak için güvenli bir kontroldür. `D:` yerine Windows'un verdiği sürücü
harfini yazın.

> [!TIP]
> Aygıt görünmüyorsa önce D+/D- yönünü, hedef beslemesinin tamamen kesildiğini
> ve `0x16EF` anahtarının açılıştan hemen sonra gönderildiğini kontrol edin.

## 3. İki doğrulanabilir flash dump alma

Bu BT201 yapılandırmasında flash boyutu **512 KB = `0x80000`** bayttır.
Önce iki ayrı, yalnızca-okuma dump alın:

```powershell
py .\jluboottool.py --device \\.\D: "read 0x0 0x80000 .\backups\BT201-stock-a.bin"
py .\jluboottool.py --device \\.\D: "read 0x0 0x80000 .\backups\BT201-stock-b.bin"
```

`D:` yerine Windows'un verdiği sürücü harfini yazın. `dump ...` veya
`read ...` kısmı **tek bir çift tırnaklı ifade** olmalıdır; araç bu ifadeyi
kendi UBOOT shell'ine tek komut olarak iletir. Araç aygıt seçim ekranı
gösterirse doğru Jieli UBOOT aygıtını seçin; ancak Windows 11 için doğrudan
`--device \\.\D:` biçimi önerilir.

İki dosyanın SHA-256 değerini karşılaştırın:

```powershell
Get-FileHash .\backups\BT201-stock-a.bin -Algorithm SHA256
Get-FileHash .\backups\BT201-stock-b.bin -Algorithm SHA256
```

> [!IMPORTANT]
> Bu iki **ilk stok dump**'ın hash değeri birebir aynı değilse yedeği geçerli
> kabul etmeyin. Dongle bağlantısını ve güç sıralamasını düzeltip iki yeni dump
> alın. Eşleşen iki dump'tan birini güvenli, salt-okunur bir yerde saklayın.

## 4. Dump içeriğini inceleme

`jl-misctools` içindeki AC690x/AC691x/AC692x ayrıştırıcısı ile yedeği açın:

```powershell
py .\jl-misctools-main\firmware\fwunpack690.py .\backups\BT201-stock-a.bin
```

Araç, dump yanına firmware yapısını açıklayan `jlfirmware.yaml`, `uboot.boot`,
`ver.bin` ve `app_data/` benzeri parçaları çıkarır. Bu işlem yedek dosyasını
değiştirmez.

## 5. Test edilmiş restore akışı

Bu akış MagicJL'in referans AC6925 / BT201 kartında, 512 KB tam flash dump ile
başarıyla test edilmiştir. Yalnızca **aynı karttan alınan** yedeği kullanın:

```powershell
py .\jluboottool.py --device \\.\D: "write 0x0 .\backups\BT201-stock-a.bin"
```

Yazma/erase ilerlemesi tamamlanmadan güç, USB veya Nano bağlantısını kesmeyin.
Ardından kartı normal modda açıp firmware'in çalıştığını kontrol edin.

### Restore sonrası doğrulama

Tekrar UBOOT moduna alın ve iki yeni okuma yapın:

```powershell
py .\jluboottool.py --device \\.\D: "read 0x0 0x80000 .\backups\BT201-verify-a.bin"
py .\jluboottool.py --device \\.\D: "read 0x0 0x80000 .\backups\BT201-verify-b.bin"
Get-FileHash .\backups\BT201-verify-a.bin -Algorithm SHA256
Get-FileHash .\backups\BT201-verify-b.bin -Algorithm SHA256
```

Bu iki yeni `verify` hash'i eşleşirse restore sonrası flash içeriği kararlıdır.
Referans BT201 testinde orijinal dump ile ilk verify dump arasında yalnızca 62
byte fark oluştu; fark `0x740D0–0x74113` aralığındaki küçük bir metadata
kaydındaydı. Firmware normal açıldı ve Bluetooth MAC/pairing bilgisi korundu.

> [!NOTE]
> Bu nedenle restore sonrasında orijinal dump ile verify dump'ın tüm SHA-256
> hash'inin aynı olmasını tek başına başarı ölçütü kabul etmeyin. Önce ilk iki
> stok dump'ın eşleştiğini, sonra da iki yeni verify dump'ın birbirleriyle
> eşleştiğini doğrulayın.

## Kurtarma ilkesi

MagicJL yüklemeden önce çalışan stok dump'ını saklayın. Firmware geri yazma
işlemi ancak aynı kart üzerinde, yedek başarıyla doğrulanmışsa ve geri yükleme
akışı ayrıca test edilmişse yapılmalıdır. Başka bir BT201 kartının dump'ını
kullanmayın; flash boyutu, Bluetooth ayarları ve board kaynakları farklı
olabilir.
