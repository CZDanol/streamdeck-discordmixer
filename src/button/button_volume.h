#pragma once

#include "button_user_related.h"

class Button_Volume : public Button_UserRelated {

public:
	Button_Volume(const CtorData &d);

public:
	VoiceState *voiceState();

public:
	virtual void update() override;

public:
	virtual void onPressed() override;
	virtual void onReleased() override;

public:
	int step;
	int state = -1;

};

