#pragma once

#include "button_user_related.h"

class Button_User : public Button_UserRelated {

public:
	Button_User(const CtorData &d);

public:
	virtual void onPressed() override;

public:
	virtual void update() override;

public:
	QString title;

};

