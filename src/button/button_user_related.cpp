#include "button_user_related.h"

Button_UserRelated::Button_UserRelated(const Button::CtorData &d) :
	Button(d),
	userIx(d.payload["settings"]["user_ix"].toString().toInt()) {
}
