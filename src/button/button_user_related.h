#pragma once

#include "button.h"

class Button_UserRelated : public Button {

public:
	Button_UserRelated(const CtorData &d);
	~Button_UserRelated();

public:
	inline int effectiveIx() const {
		return userIx + device.userIxOffset;
	}

public:
	const int userIx;

};

