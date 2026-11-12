# LiteTube test report (honest status)

Environment of every test below: Ubuntu 24.04 container, Qt 5.15 offscreen, libmpv 0.37 with `vo=null ao=null`, no access to youtube.com (DNS blocked).

## v1.1 changes (from your log)
- Fixed: OpenSSL missing (thumbnails/API/sign-in), broken external-audio attach (no sound), Direct3D re-creation on hidden window, crash when clicking a video while one is playing, empty Settings.
- New: YouTube-web style UI, comments, Google sign-in, subtitles styled like MX Player, cookie-based personalised feeds (opt-in).
- All of the above are compiled and self-tested (51 checks) on Linux only; UI checked via screenshots. NOT run on Windows.

## TESTED (automated, 50 checks, `--selftest`, all passing)
- Playlists: create/rename/delete, reserved lists protected, duplicates rejected, reorder, persistence across restart.
- Watch history + search history: ordering, de-duplication, resume position, privacy toggle, clear, persistence. Subscriptions persistence.
- Thumbnail disk cache size limit.
- Stream selection (`pick`): H.264 preferred over AV1, quality limit, fallback to lowest, storyboard filtering, subtitle parsing, bad-output handling.
- Failure handling with missing yt-dlp (search and stream resolve fail cleanly; cancelled requests give no late result).
- Shorts navigation logic (wheel cooldown, key map).
- libmpv control on a generated WAV: init, load, position events, pause, speed, volume, seek, end-of-file, version.
- UI: all pages construct and navigate; search page; library; Watch page failing cleanly on an unplayable video; compact player enter/leave; Shorts page action handling; Settings dialog builds. App starts and idles at ~77 MB RSS (Linux, offscreen, includes Qt libs - not a Windows figure).

## PARTIALLY TESTED
- yt-dlp invocation: real process started, error path observed (DNS failure); success path never run.

## UNTESTED (must be verified on the target PC)
- Anything on Windows 8.1 (build, DLL compatibility, run).
- Direct3D output, Intel GMA 3100, embedding the mpv window in the Qt widget, resizing, fullscreen, window re-parenting when moving video between Watch page / Shorts / Compact player.
- mpv-captured mouse wheel / keys over the video (implemented through mpv key bindings that send `script-message lt <action>`).
- Real YouTube search, trending, stream playback, quality switching, audio-only + video-only merge (`audio-files-append`), subtitles, expired-stream retry.
- Shorts feed quality, scrolling feel, audio overlap behaviour under real network delays.
- CPU and RAM on the E4400/3 GB, long sessions, memory growth.
- YouTube Data API v3 path (needs a key).

## Known limitations (by design / API restrictions)
- No YouTube account sign-in: subscriptions, likes and playlists are local only. "Like" adds to local Favorites; Dislike is omitted (needs OAuth). Comments opens the video on youtube.com.
- No personalised recommendations: Home uses most-popular or a search seeded by the last channel watched.
- Related videos are search-based (the API's relatedToVideoId was removed).
- Trending without an API key is search-based.
- yt-dlp can be blocked or require sign-in/age confirmation for some videos; those show an error with Retry.
- Playback of 1080p on this CPU is not expected to be smooth.
