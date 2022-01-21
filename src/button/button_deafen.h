#pragma once

#include "button_user_related.h"

class Button_Deafen : public Button {

public:
	Button_Deafen(const CtorData &d);
	~Button_Deafen();

public:
	virtual void onReleased() override;

public:
	virtual void update() override;

public:
	int state = -1;

};

