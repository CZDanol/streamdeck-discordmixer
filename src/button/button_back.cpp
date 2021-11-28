#include "button_back.h"

Button_Back::Button_Back(const Button::CtorData &d) : Button(d) {

}

void Button_Back::onPressed() {
	device.plugin.deck.switchProfile(device.deviceID, "");
}
