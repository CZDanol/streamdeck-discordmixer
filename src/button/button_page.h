#pragma once

#include "button.h"

class Button_Page : public Button {

public:
	Button_Page(const CtorData &d);
	~Button_Page();

public:
	virtual void onPressed() override;
	virtual void onReleased() override;
	virtual void update() override;

public:
	QString title = "###";
	int step = 1;
	int state = -1;

};

