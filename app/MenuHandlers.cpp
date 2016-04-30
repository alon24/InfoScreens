/*
 * MenuHandlers.cpp
 *
 *  Created on: Apr 30, 2016
 *      Author: iklein
 */

#include <MenuHandlers.h>

ThreeSecondMenuHandler::ThreeSecondMenuHandler(InfoScreens *screen) {
	this->infos = screen;
	threeSecTimer.setIntervalMs(3000);
	threeSecTimer.setCallback(TimerDelegate(&ThreeSecondMenuHandler::threeSecondsPassed, this));
};

void ThreeSecondMenuHandler::threeSecondsPassed() {
	ViewMode tempMode = infos->getViewMode();
	debugf("ThreeSecTimer clicked - current mode == %d", tempMode);

	switch (tempMode) {
		case ViewMode::INFO:
//			infos->show();
			return;
		case ViewMode::EDIT:
			infos->setViewMode(ViewMode::INFO);
			infos->show();
//			threeSecTimer.startOnce();
			break;
		case ViewMode::EDIT_FIELD:
			infos->callMenuEventDelegate();
			infos->setViewMode(ViewMode::EDIT);
			threeSecTimer.startOnce();
			break;
	}
};

void ThreeSecondMenuHandler::infoModeBtnClicked(MultiFunctionButtonAction event) {
		switch (event) {
			case BTN_CLICK:
				debugf("click");
				infos->moveRight();
				break;
			case BTN_DOUBLE_CLICK:
	//			debugf("BTN_DOUBLE_CLICK");
				infos->moveLeft();
				break;
			case BTN_LONG_CLICK:
	//			debugf("BTN_LONG_CLICK, going to edit mode, %i", getCurrent()->getallEditableParams().size());

				if(infos->getCurrent()->getallEditableParams().size() != 0) {
					infos->setViewMode(ViewMode::EDIT);
					threeSecTimer.startOnce();
				}
				break;
			case BTN_HOLD_CLICK:
	//			debugf("BTN_HOLD_CLICK, going to edit mode");
				break;
		}
	};

void ThreeSecondMenuHandler::editModeBtnClicked(MultiFunctionButtonAction event) {
	if (threeSecTimer.isStarted()) {
		threeSecTimer.stop();
	}

	switch (event) {
		case BTN_CLICK:
			debugf("edit - click");
			infos->moveToNextEditParam();
//			moveRight();
//				handleClick();
			break;
		case BTN_DOUBLE_CLICK:
			infos->moveToPrevEditParam();
			break;
			case BTN_LONG_CLICK:
//	//			debugf("edit - BTN_LONG_CLICK");
				infos->setViewMode(ViewMode::EDIT_FIELD);
//				infos->handleHoldClick();
				break;
//			case BTN_HOLD_CLICK:
//				debugf("BTN_HOLD_CLICK");
//				break;
	}

	threeSecTimer.startOnce();
};

void ThreeSecondMenuHandler::editFieldModeBtnClicked(MultiFunctionButtonAction event) {
	if (threeSecTimer.isStarted()) {
		threeSecTimer.stop();
	}

	switch (event) {
		case BTN_CLICK:
//			debugf("editField - click");
			infos->moveToNextValue();
			break;
		case BTN_DOUBLE_CLICK:
			infos->moveToPrevValue();
			break;
	}

	threeSecTimer.startOnce();
};

//void setOnMenuEventDelegate(MenuEventDelegate handler) {
//
//};
