#pragma once

#include "button.h"

class Button_Back : public Button {

public:
	Button_Back(const CtorData &d);

public:
	virtual void onPressed() override;

};

