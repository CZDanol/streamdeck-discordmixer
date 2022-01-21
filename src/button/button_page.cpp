#include "button_page.h"

Button_Page::Button_Page(const Button::CtorData &d) : Button(d) {
	step = (d.action == "cz.danol.discordmixer.nextpage" ? 1 : -1) * d.payload["settings"]["step"].toString().toInt();
}

void Button_Page::onPressed() {
	const int stepAbs = qAbs(step);
	const int newOffset = qBound(0, device.userIxOffset + step, (device.voiceStates.size() + stepAbs - 1) / stepAbs);

	qDebug() << "Pressed page button" << step << stepAbs << device.userIxOffset << newOffset << device.voiceStates.size();

	if(device.userIxOffset == newOffset)
		return;

	state = -1;
	device.userIxOffset = newOffset;
	device.updateAllButtons();
}

void Button_Page::onReleased() {
	state = -1;
	update();
}

void Button_Page::update() {
	const int targetState = (step < 0) ? (device.userIxOffset == 0) : (device.userIxOffset + step >= device.voiceStates.size());

	if(state == targetState)
		return;

	state = targetState;
	device.plugin.deck.setState(targetState, context);
}
