#pragma once

#include "button.h"

class Button_OpenMixer : public Button {

public:
	Button_OpenMixer(const CtorData &d);

public:
	virtual void onPressed() override;

};

