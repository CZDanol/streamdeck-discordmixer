# Stream Deck Discord Volume Mixer
This is a plugin for Stream Deck that allows setting volume for individual users in Discord Voice chat.

* You can adjust the audio mixer panel to your needs, it's quite modular - you can move buttons around etc.
* Tested on W10x64.
* Currently natively support only the 15-key deck. Though it should not be a big issue to create your own deck.

## Donation
If you feel like giving me some money, you can use the button below.

<form action="https://www.paypal.com/donate" method="post" target="_top">
<input type="hidden" name="hosted_button_id" value="QZC5P67TBTRX6" />
<input type="image" src="https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif" border="0" name="submit" title="PayPal - The safer, easier way to pay online!" alt="Donate with PayPal button" />
<img alt="" border="0" src="https://www.paypal.com/en_CZ/i/scr/pixel.gif" width="1" height="1" />
</form>


## Configuration
1. Download and install the plugin.
2. Add the "Discord Volume Mixer" button on your deck.
3. Go to the [Discord developer portal](https://discordapp.com/developers) and create an application.
4. In the Oauth2 settings, set the redirect URI to `http://localhost:1337/callback`
5. Hit "Save changes".
6. From the OAuth2 tab, copy `Client ID` and `Client secret` and paste it in the deck button details.
7. Click on the deck button. Discord will ask you for some permissions & firewall and stuff.
8. Done.

## Third-party libraries & stuff
* Qt 6 (tested on Qt 6.2.1 MSVC 2019 Win x64)
* I copied a bunch of Stream Deck control functionality from [fredemmott](https://github.com/fredemmott/StreamDeck-CPPSDK), though I adjusted it.
* Icons8 icons
* Communicates with Discord via IPC through QLocalSocket.