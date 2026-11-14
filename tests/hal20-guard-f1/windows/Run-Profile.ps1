param([Parameter(Mandatory=$true)][string]$Profile)
$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
$Spec = Get-Content (Join-Path $Root 'MANIFEST.json') -Raw -Encoding UTF8 | ConvertFrom-Json
$Entry = $Spec.profiles | Where-Object { $_.name -eq $Profile }
if (-not $Entry) { throw 'Perfil desconhecido. Consulte README-WINDOWS.md.' }
foreach ($File in $Spec.files) {
    $Path = Join-Path $Root $File.path
    if (-not (Test-Path $Path)) { throw "Arquivo ausente: $($File.path)" }
    if ((Get-FileHash $Path -Algorithm SHA256).Hash.ToLowerInvariant() -ne $File.sha256) { throw "Hash divergente: $($File.path)" }
}
if ($Entry.requires_dual) {
    foreach ($File in $Spec.private_dual_required_locally) {
        $Path = Join-Path $Root $File.path
        if (-not (Test-Path $Path)) { throw "Dual local necessario e nao distribuido: $($File.path)" }
        if ((Get-FileHash $Path -Algorithm SHA256).Hash.ToLowerInvariant() -ne $File.sha256) { throw "Dual local nao corresponde ao PR10 API24: $($File.path)" }
    }
}
$Data = Join-Path $Root "test-data/$Profile"
New-Item -ItemType Directory -Force $Data | Out-Null
$Settings = Join-Path $Data 'settings.txt'
if (-not (Test-Path $Settings)) { Copy-Item (Join-Path $Root "profiles/$Profile.txt") $Settings }
$Logs = Join-Path $Root 'logs'
New-Item -ItemType Directory -Force $Logs | Out-Null
$env:MEMORIES_USER_DIR = $Data
$env:MEMORIES_SETTINGS = $Settings
$env:MEMORIES_MODS_DIR = Join-Path $Root 'mods'
$env:MEMORIES_LOG = Join-Path $Logs ($Profile + '-' + (Get-Date -Format 'yyyyMMdd-HHmmss') + '.log')
$env:MEMORIES_TRACE_MODS = '1'
$env:MEMORIES_TRACE_DUEL_EFFECTS = '1'
$env:MEMORIES_TRACE_STATE = '1'
Write-Host "Perfil: $Profile; $($Entry.expectation)"
Write-Host "Dados isolados: $Data; log: $env:MEMORIES_LOG"
Start-Process -FilePath (Join-Path $Root 'memories-pc.exe') -WorkingDirectory $Root -Wait
