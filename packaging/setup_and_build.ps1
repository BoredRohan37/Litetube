<#
 LiteTube: download dependencies and build the portable Windows 8.1 x64 folder.
 Run (PowerShell 4+, no admin needed):
   powershell -ExecutionPolicy Bypass -File packaging\setup_and_build.ps1
 Downloads go to %USERPROFILE%\LiteTubeDeps (re-runs skip what is already there).
 Fetcher: .NET System.Net.WebClient with TLS 1.2 forced (works on stock 8.1; Invoke-WebRequest/BITS used as fallbacks).
 Override pinned versions with parameters if a URL has moved.
#>
param(
  [string]$Deps = "$env:USERPROFILE\LiteTubeDeps",
  [string]$MpvDevUrl = "https://master.dl.sourceforge.net/project/mpv-player-windows/libmpv/mpv-dev-x86_64-20230129-git-86093fc.7z",                       # direct .7z URL of an mpv 0.35.x "mpv-dev-x86_64" build; empty = auto-detect
  [string]$CMakeVersion = "3.27.9",
  [string]$QtVersionDir = "qt5_5152",
  [string]$QtPackage = "qt.qt5.5152.win64_mingw81",
  [string]$MingwPackage = "qt.tools.win64_mingw810",
  [string]$OpenSslPackage = "qt.tools.openssl.win_x64"
)
$ErrorActionPreference = "Stop"
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]3072   # TLS 1.2 (8.1 defaults to TLS 1.0)
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
New-Item -ItemType Directory -Force -Path $Deps | Out-Null
Add-Type -AssemblyName System.IO.Compression.FileSystem

function Get-File([string]$url, [string]$out) {
  if ((Test-Path $out) -and (Get-Item $out).Length -gt 0) { Write-Host "  cached: $(Split-Path -Leaf $out)"; return }
  Write-Host "  download: $url"
  for ($i = 1; $i -le 3; $i++) {
    try {
      $wc = New-Object System.Net.WebClient
      $wc.Headers.Add("User-Agent", "LiteTube-setup")
      $wc.DownloadFile($url, "$out.part"); Move-Item "$out.part" $out -Force; return
    } catch {
      Write-Host "    attempt $i failed: $($_.Exception.Message)"
      try { Start-BitsTransfer -Source $url -Destination "$out.part"; Move-Item "$out.part" $out -Force; return } catch {}
      try { Invoke-WebRequest -UseBasicParsing -Uri $url -OutFile "$out.part"; Move-Item "$out.part" $out -Force; return } catch {}
    }
  }
  throw "Could not download $url"
}
function Get-Text([string]$url) {
  $wc = New-Object System.Net.WebClient; $wc.Headers.Add("User-Agent", "LiteTube-setup"); $wc.Encoding = [Text.Encoding]::UTF8
  return $wc.DownloadString($url)
}

# ---- 7-Zip (7zr.exe, standalone) ----
$7zr = "$Deps\7zr.exe"
Write-Host "[1/6] 7zr"; Get-File "https://www.7-zip.org/a/7zr.exe" $7zr
function Expand-7z([string]$archive, [string]$dest) {
  & $7zr x $archive "-o$dest" -y | Out-Null
  if ($LASTEXITCODE -ne 0) { throw "7z extraction failed: $archive" }
}

# ---- CMake ----
Write-Host "[2/6] CMake $CMakeVersion"
$cmakeDir = "$Deps\cmake-$CMakeVersion-windows-x86_64"
if (-not (Test-Path "$cmakeDir\bin\cmake.exe")) {
  $zip = "$Deps\cmake.zip"
  Get-File "https://github.com/Kitware/CMake/releases/download/v$CMakeVersion/cmake-$CMakeVersion-windows-x86_64.zip" $zip
  [IO.Compression.ZipFile]::ExtractToDirectory($zip, $Deps)
}

# ---- Qt 5.15.2 MinGW + MinGW 8.1 from the official Qt repository ----
function Install-QtRepoPackage([string]$base, [string]$pkg, [string]$marker) {
  if (Test-Path $marker) { Write-Host "  already installed: $pkg"; return }
  [xml]$xml = Get-Text "$base/Updates.xml"
  $node = $xml.Updates.PackageUpdate | Where-Object { $_.Name -eq $pkg } | Select-Object -First 1
  if (-not $node) { throw "Package $pkg not found in $base/Updates.xml" }
  foreach ($a in ($node.DownloadableArchives -split ",\s*")) {
    if (-not $a) { continue }
    $file = "$($node.Version)$a"
    $local = "$Deps\dl\$file"; New-Item -ItemType Directory -Force -Path "$Deps\dl" | Out-Null
    Get-File "$base/$pkg/$file" $local
    Expand-7z $local "$Deps\Qt"
  }
}
Write-Host "[3/6] Qt 5.15.2 (MinGW 8.1, 64-bit) - several hundred MB, please wait"
$qtBase = "https://download.qt.io/online/qtsdkrepository/windows_x86/desktop/$QtVersionDir"
Install-QtRepoPackage $qtBase $QtPackage "$Deps\Qt\5.15.2\mingw81_64\bin\Qt5Core.dll"
Write-Host "[4/6] MinGW 8.1 toolchain"
$toolBase = "https://download.qt.io/online/qtsdkrepository/windows_x86/desktop/tools_mingw"
Install-QtRepoPackage $toolBase $MingwPackage "$Deps\Qt\Tools\mingw810_64\bin\g++.exe"
Write-Host "[4b/6] OpenSSL 1.1.x (needed by Qt for thumbnails, API and sign-in)"
$sslBase = "https://download.qt.io/online/qtsdkrepository/windows_x86/desktop/tools_openssl_x64"
Install-QtRepoPackage $sslBase $OpenSslPackage "$Deps\Qt\Tools\OpenSSL\Win_x64\bin\libssl-1_1-x64.dll"
$sslBin = "$Deps\Qt\Tools\OpenSSL\Win_x64\bin"
$qtDir = "$Deps\Qt\5.15.2\mingw81_64"; $mingwBin = "$Deps\Qt\Tools\mingw810_64\bin"
if (-not (Test-Path "$qtDir\bin\windeployqt.exe")) { throw "Qt install incomplete: $qtDir" }

# ---- libmpv dev (mpv 0.35.x: still has --vo=direct3d) ----
Write-Host "[5/6] libmpv + yt-dlp"
$mpvRoot = "$Deps\mpv-dev"
if (-not (Test-Path "$mpvRoot\include\mpv\client.h")) {
  if (-not $MpvDevUrl) {
    Write-Host "  auto-detecting a pre-August-2023 (mpv 0.35.x) build from shinchiro/mpv-winbuild-cmake"
    $best = $null
    for ($page = 1; $page -le 8 -and -not $best; $page++) {
      $rels = (Get-Text "https://api.github.com/repos/shinchiro/mpv-winbuild-cmake/releases?per_page=100&page=$page") | ConvertFrom-Json
      if (-not $rels -or $rels.Count -eq 0) { break }
      foreach ($r in $rels) {
        if ([datetime]$r.published_at -lt [datetime]"2023-07-15") {
          $asset = $r.assets | Where-Object { $_.name -match '^mpv-dev-x86_64-\d+.*\.7z$' -and $_.name -notmatch 'v3|aarch' } | Select-Object -First 1
          if ($asset) { $best = $asset; break }
        }
      }
    }
    if (-not $best) { throw "Could not auto-detect libmpv. Re-run with -MpvDevUrl <url of mpv-dev-x86_64-*.7z>" }
    $MpvDevUrl = $best.browser_download_url
  }
  $a = "$Deps\mpv-dev.7z"; Get-File $MpvDevUrl $a
  New-Item -ItemType Directory -Force -Path $mpvRoot | Out-Null
  Expand-7z $a $mpvRoot
}
$mpvDll = Get-ChildItem $mpvRoot -Filter "*mpv*.dll" -Recurse | Select-Object -First 1
if (-not $mpvDll) { throw "libmpv DLL not found in $mpvRoot" }
$bytes = [IO.File]::ReadAllBytes($mpvDll.FullName)
if (([Text.Encoding]::ASCII.GetString($bytes)).IndexOf("direct3d") -lt 0) {
  Write-Warning "This libmpv does not seem to contain the 'direct3d' video output. Use an mpv 0.35.x build (-MpvDevUrl)."
}
# yt-dlp: prefer the standard exe, fall back to the x86 build if it will not run on this Windows
$yt = "$Deps\yt-dlp.exe"
Get-File "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe" $yt
$ok = $false; try { & $yt --version | Out-Null; $ok = ($LASTEXITCODE -eq 0) } catch {}
if (-not $ok) {
  Write-Warning "yt-dlp.exe does not run here; trying yt-dlp_x86.exe"
  Remove-Item $yt -Force; $yt = "$Deps\yt-dlp_x86.exe"
  Get-File "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp_x86.exe" $yt
  try { & $yt --version | Out-Null } catch { Write-Warning "yt-dlp still will not run; install a compatible build next to LiteTube.exe." }
}

# ---- build + package ----
Write-Host "[6/6] Build and package"
$env:PATH = "$cmakeDir\bin;$mingwBin;$qtDir\bin;$env:PATH"
Push-Location $root
try {
  $mpvInc = (Get-ChildItem $mpvRoot -Filter client.h -Recurse | Select-Object -First 1).Directory.Parent.Parent.FullName
  if (Test-Path build) { Remove-Item build -Recurse -Force }
  cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=$qtDir" "-DMPV_ROOT=$mpvInc"
  if ($LASTEXITCODE -ne 0) { throw "CMake configure failed" }
  cmake --build build -j 2
  if ($LASTEXITCODE -ne 0) { throw "Build failed" }

  $dist = "$root\dist\LiteTube"
  if (Test-Path "$root\dist") { Remove-Item "$root\dist" -Recurse -Force }
  New-Item -ItemType Directory -Force -Path "$dist\data", "$dist\licenses" | Out-Null
  Copy-Item build\LiteTube.exe $dist
  & windeployqt --no-translations --no-opengl-sw --no-angle --no-system-d3d-compiler "$dist\LiteTube.exe"
  if ($LASTEXITCODE -ne 0) { throw "windeployqt failed" }
  Copy-Item $mpvDll.FullName $dist
  foreach ($f in "libssl-1_1-x64.dll", "libcrypto-1_1-x64.dll") {
    if (Test-Path "$sslBin\$f") { Copy-Item "$sslBin\$f" $dist } else { Write-Warning "$f not found - thumbnails and sign-in need it" }
  }
  # optional Roboto font for MX Player-style subtitles (best effort)
  try {
    New-Item -ItemType Directory -Force -Path "$dist\fonts" | Out-Null
    $fz = "$Deps\roboto.zip"
    Get-File "https://github.com/googlefonts/roboto/releases/download/v2.138/roboto-unhinted.zip" $fz
    $tmp = "$Deps\roboto"; if (Test-Path $tmp) { Remove-Item $tmp -Recurse -Force }
    [IO.Compression.ZipFile]::ExtractToDirectory($fz, $tmp)
    Get-ChildItem $tmp -Recurse -Filter "Roboto-Medium.ttf" | Select-Object -First 1 | Copy-Item -Destination "$dist\fonts"
    Get-ChildItem $tmp -Recurse -Filter "Roboto-Bold.ttf" | Select-Object -First 1 | Copy-Item -Destination "$dist\fonts"
  } catch { Write-Warning "Roboto font not downloaded (subtitles will use Arial): $($_.Exception.Message)" }
  Copy-Item $yt "$dist\yt-dlp.exe"
  Copy-Item licenses\NOTICE.txt "$dist\licenses\"
  Copy-Item README.md $dist
  Write-Host ""
  Write-Host "DONE. Portable folder: $dist" -ForegroundColor Green
  Write-Host "Test first: run LiteTube.exe, play a 360p video, then check data\logs\litetube.log if anything fails."
} finally { Pop-Location }
