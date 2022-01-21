#pragma once

#include <QTimer>

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
	void trigger();

public:
	int step;
	int state = -1;

	int repeatSkip = 0;
	QTimer repeatTimer;

};

