#include "button_page.h"

Button_Page::Button_Page(const Button::CtorData &d) : Button(d) {
	step = (d.action == "cz.danol.discordmixer.nextpage" ? 1 : -1) * d.payload["settings"]["step"].toString().toInt();
	(step > 0 ? device.nextPageButtons : device.prevPageButtons)++;
}

Button_Page::~Button_Page() {
	(step > 0 ? device.nextPageButtons : device.prevPageButtons)--;
}

auto computeParams(Button_Page &b) {
	struct R {
		int pageCount;
		int currentPage;
		int maxOffset;
	};

	const int stepAbs = qAbs(b.step);

	R r;
	r.pageCount = static_cast<int>(b.device.voiceStates.size() + stepAbs - 1) / stepAbs;
	r.currentPage = b.device.userIxOffset / stepAbs;
	r.maxOffset = (r.pageCount - 1) * stepAbs;
	return r;
}

void Button_Page::onPressed() {
	const auto p = computeParams(*this);

	int newOffset = device.userIxOffset + step;
	if(newOffset > p.maxOffset)
		newOffset = 0;
	else if(newOffset < 0)
		newOffset = p.maxOffset;

	qDebug() << "Pressed page button" << step << device.userIxOffset << newOffset << device.voiceStates.size();

	if(device.userIxOffset == newOffset)
		return;

	state = -1;
	device.userIxOffset = newOffset;
	device.updateButtons();
}

void Button_Page::onReleased() {
	state = -1;
	update();
}

void Button_Page::update() {
	const auto p = computeParams(*this);

	int hide;
	if(device.prevPageButtons == 0 || device.nextPageButtons == 0)
		hide = (p.pageCount == 0);
	else
		hide = (step < 0) ? (p.currentPage <= 0) : (p.currentPage >= p.pageCount - 1);

	QString targetTitle = hide ? "" : QStringLiteral("%1/%2").arg(p.currentPage + 1).arg(p.pageCount);

	if(state != hide) {
		state = hide;
		device.plugin.deck.setState(hide, context);
	}

	if(title != targetTitle) {
		title = targetTitle;
		device.plugin.deck.setTitle(targetTitle, context, kESDSDKTarget_HardwareAndSoftware);
	}
}
