#pragma once

#include "button_user_related.h"

class Button_Volume : public Button_UserRelated {

public:
	Button_Volume(const CtorData &d);

public:
	virtual void onPressed() override;

public:
	int step;

};

