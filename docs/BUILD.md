# Building LiteTube for Windows 8.1 x64

## Toolchain (recommended, minimal)
- Qt **5.15.2**, MinGW 8.1.0 64-bit kit (official installer; Qt 5.15 supports Windows 7/8.1). Do not use Qt 6 (needs Windows 10).
- CMake 3.16+ and Ninja or mingw32-make.
- **libmpv development package for x86_64 MinGW** (contains `include/mpv/client.h`, `libmpv.dll.a`, `libmpv-2.dll`).
  - Use an **mpv 0.35.x** build. mpv's `--vo=direct3d` output (the Direct3D 9 path that suits the GMA 3100) exists in 0.35 and was removed in later releases. Check with `mpv --vo=help` that `direct3d` is listed. If your build lacks it, LiteTube's Settings > Video lets you pick `gpu`, but that is untested on this hardware and may need OpenGL/D3D11.
  - Verify the build's DLLs load on 8.1 (Dependencies tool). Builds compiled against newer Windows APIs may not start.
- **yt-dlp.exe** (put next to LiteTube.exe). Use a release that still supports Windows 8.1 (the standard `yt-dlp.exe` build); verify by running `yt-dlp.exe --version` on the target PC.

## Build
```
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qt/5.15.2/mingw81_64 -DMPV_ROOT=C:/libs/mpv-dev
cmake --build build
```
Then `packaging\package_windows.bat` (edit the paths at the top) creates `dist\LiteTube\` with Qt DLLs (windeployqt), libmpv, yt-dlp, licenses.

## Runtime notes
- Video output: Settings > Video. Default Direct3D, software decoding (hwdec off). Keep quality at 360p/480p on the E4400; 720p+ H.264 software decoding will likely drop frames.
- Optional YouTube Data API key: Settings > Advanced. Without it LiteTube uses yt-dlp for search/trending. API quota errors fall back to yt-dlp automatically.
- Logs: `data/logs/litetube.log`.
