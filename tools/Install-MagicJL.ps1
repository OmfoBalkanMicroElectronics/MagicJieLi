param(
    [Parameter(Mandatory = $true)]
    [string]$SdkPath,
    [switch]$NoBackup
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$sourceApps = Join-Path $repoRoot 'MagicJL\apps'
$SdkPath = (Resolve-Path -LiteralPath $SdkPath).Path
$targetApps = Join-Path $SdkPath 'apps'

if (-not (Test-Path -LiteralPath $sourceApps)) {
    throw "MagicJL kaynak klasoru bulunamadi: $sourceApps"
}
if (-not (Test-Path -LiteralPath (Join-Path $targetApps 'sdk.cbp'))) {
    throw "Bu yol uyumlu bir SDK degil. Beklenen dosya: $targetApps\sdk.cbp"
}

if (-not $NoBackup) {
    $stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
    $backupRoot = Join-Path $SdkPath ("MagicJL-backup-" + $stamp)
    Write-Host "Stok apps klasoru yedekleniyor: $backupRoot"
    Copy-Item -LiteralPath $targetApps -Destination $backupRoot -Recurse -Force
}

Write-Host "MagicJL apps katmani uygulanıyor..."
Get-ChildItem -LiteralPath $sourceApps -Force | ForEach-Object {
    if ($_.Name -eq 'obj') {
        Write-Host "Atlandi: MagicJL/apps/obj (derleme ciktisi)"
        return
    }
    Copy-Item -LiteralPath $_.FullName -Destination $targetApps -Recurse -Force
}

Write-Host ''
Write-Host 'Tamamlandi.' -ForegroundColor Green
Write-Host "Sonraki adim: $targetApps\sdk.cbp dosyasini Code::Blocks ile acin."
Write-Host 'Hedef: SDK_flash | Yapilandirma: Release | Ardindan Build veya Rebuild.'
