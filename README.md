# Stream Deck Discord Mixer
This is a plugin for Stream Deck that allows setting volume for individual users in Discord Voice chat.

You can adjust the audio mixer panel to your needs, it's quite smart.

## Configuration
1. Download and install the plugin.
2. Add the "Discord Volume Mixer" button on your deck.
3. Go to the [Discord developer portal](https://discordapp.com/developers) and create an application.
4. In the Oauth2 settings, set the redirect URI to `http://localhost:1337/callback`
5. Hit "Save changes".
6. From the general tab, copy `Client ID` and `Client secret` and paste it in the deck button details.
7. Click on the deck button. Discord will ask you for some permissions & firewall and stuff.
8. Done.