# Sign in with Google (optional)
1. Open https://console.cloud.google.com/ and create a project.
2. APIs & Services > Library > enable **YouTube Data API v3**.
3. APIs & Services > OAuth consent screen: User type *External*, fill the app name, add your own Google account under **Test users**.
4. Credentials > Create credentials > **OAuth client ID** > Application type **Desktop app**.
5. In LiteTube: Settings > Account, paste the client ID (and client secret), press **Sign in with Google**, approve in your browser.

What works when signed in: your subscriptions and their latest uploads, your YouTube playlists and Liked videos (Library), like / dislike / subscribe, saving to your playlists, posting comments.
Not offered by the official API: personalised Home recommendations, Watch Later, watch history. Optional workaround: Settings > Account > "Personalised feeds via browser cookies" (yt-dlp reads your browser login; unofficial, may conflict with YouTube's terms; Firefox works best, newer Chrome/Edge versions often block it).
The refresh token is stored in data\settings.ini on this PC; Sign out removes it.
