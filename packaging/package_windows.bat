@echo off
rem Edit these three paths, run from the project root in a "Qt 5.15.2 MinGW 64-bit" command prompt.
set QT_DIR=C:\Qt\5.15.2\mingw81_64
set MPV_ROOT=C:\libs\mpv-dev
set YTDLP=C:\libs\yt-dlp.exe

set PATH=%QT_DIR%\bin;%PATH%
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_DIR% -DMPV_ROOT=%MPV_ROOT% || exit /b 1
cmake --build build || exit /b 1

rmdir /s /q dist 2>nul
mkdir dist\LiteTube\data dist\LiteTube\licenses
copy build\LiteTube.exe dist\LiteTube\ >nul
windeployqt --release --no-translations --no-opengl-sw --no-angle --no-system-d3d-compiler dist\LiteTube\LiteTube.exe || exit /b 1
for %%f in ("%MPV_ROOT%\*.dll") do copy "%%f" dist\LiteTube\ >nul
copy "%YTDLP%" dist\LiteTube\yt-dlp.exe >nul
copy licenses\NOTICE.txt dist\LiteTube\licenses\ >nul
copy README.md dist\LiteTube\ >nul
echo Portable folder ready: dist\LiteTube
