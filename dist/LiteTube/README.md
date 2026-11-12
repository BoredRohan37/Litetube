# LiteTube 1.0.0

Lightweight YouTube desktop client for older Windows PCs. C++17, Qt 5 Widgets, libmpv (Direct3D output), yt-dlp for stream resolution, optional YouTube Data API v3 key.

**Status: source complete and compiled/tested on Linux (headless). NOT yet built or run on Windows 8.1 or the GMA 3100 PC. See docs/TEST_REPORT.md.**

Pages: Home, Search, Trending, Shorts, Subscriptions (local), Watch, Compact Player, Library/Playlists/Favorites/Watch Later, History (watch + search), Settings (9 groups), About.

Quick start (Windows): read docs/BUILD.md, then run `packaging\package_windows.bat`.
Quick start (Linux, for development): `cmake -S . -B build && cmake --build build`, run `./build/LiteTube`.
Headless self-test: `cmake -S . -B build-test -DLITETUBE_SELFTEST=ON && cmake --build build-test && QT_QPA_PLATFORM=offscreen ./build-test/LiteTube --selftest`

Portable data (settings.ini, playlists.json, history.json, subscriptions.json, thumbs/, logs/) is stored in `data/` next to the exe.
