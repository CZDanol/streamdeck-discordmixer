#pragma once

#include "button.h"

class Button_Page : public Button {

public:
	Button_Page(const CtorData &d);

public:
	virtual void onPressed() override;
	virtual void onReleased() override;
	virtual void update() override;

public:
	int step = 1;
	int state = -1;

};

