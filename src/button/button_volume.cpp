#include "button_volume.h"

Button_Volume::Button_Volume(const Button::CtorData &d) : Button_UserRelated(d) {
	step = d.action == "cz.danol.discordmixer.volumeup" ? 15 : -15;
}

void Button_Volume::onPressed() {

}
