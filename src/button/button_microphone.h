#pragma once

#include "button_user_related.h"

class Button_Microphone : public Button {

public:
	Button_Microphone(const CtorData &d) : Button(d) {}

public:
	virtual void onReleased() override;

public:
	virtual void update() override;

public:
	int state = -1;

};

