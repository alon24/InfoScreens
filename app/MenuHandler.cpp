/*
 * MenuHandler.cpp
 *
 *  Created on: Apr 24, 2016
 *      Author: iklein
 */

#include <SmingCore.h>
#include "InfoScreens.h"

class ThreeSecondMenuHandler {
public:
	ThreeSecondMenuHandler(InfoScreens *screen) {
		this->screen = screen;
		threeSecTimer.setIntervalMs(3000);
		threeSecTimer.setCallback(TimerDelegate(&ThreeSecondMenuHandler::threeSecondsPassed, this));
	};

	void threeSecondsPassed() {
		if (screen->getViewMode() == ViewMode::INFO) {
			return;
		}

		if (screen->getViewMode() == ViewMode::EDIT) {
			screen->setViewMode(ViewMode::INFO);
			threeSecTimer.startOnce();
		} else if (screen->getViewMode() == ViewMode::EDIT_FIELD) {

////			debugf("editField - return to View mode");
//			if(delegatedMenuEvent) {
//				String id = getCurrent()->getCurrentEditParam()->id;
//				String newVal = *paramValueMap[id].val;
//				if (delegatedMenuEvent(getCurrent()->getCurrentEditParam(), viewMode, InfoParamDataSet, newVal)) {
////					debugf("InfoParamDataSet delegate consumed");
//					return;
//				}
//			}


			screen->setViewMode(ViewMode::EDIT);
			threeSecTimer.startOnce();
		}
	};

	void infoModeBtnClicked(MultiFunctionButtonAction event) {
		switch (event) {
			case BTN_CLICK:
				debugf("click");
				screen->moveRight();
				break;
			case BTN_DOUBLE_CLICK:
	//			debugf("BTN_DOUBLE_CLICK");
				screen->moveLeft();
				break;
			case BTN_LONG_CLICK:
	//			debugf("BTN_LONG_CLICK, going to edit mode, %i", getCurrent()->getallEditableParams().size());

				if(screen->getCurrent()->getallEditableParams().size() != 0) {
					screen->setViewMode(ViewMode::EDIT);
					threeSecTimer.startOnce();
				}
				break;
			case BTN_HOLD_CLICK:
	//			debugf("BTN_HOLD_CLICK, going to edit mode");
				break;
		}
	};

	void editModeBtnClicked(MultiFunctionButtonAction event) {
		if (threeSecTimer.isStarted()) {
			threeSecTimer.stop();
		}

		switch (event) {
			case BTN_CLICK:
				debugf("edit - click");
				screen->moveToNextEditParam();
	//			moveRight();
	//				handleClick();
				break;
			case BTN_DOUBLE_CLICK:
				screen->moveToPrevEditParam();
				break;
//			case BTN_LONG_CLICK:
//	//			debugf("edit - BTN_LONG_CLICK");
//				setViewMode(ViewMode::EDIT_FIELD);
//	//				handleHoldClick();
//				break;
//			case BTN_HOLD_CLICK:
//				debugf("BTN_HOLD_CLICK");
//				break;
		}

		threeSecTimer.startOnce();
	};

	void editFieldModeBtnClicked(MultiFunctionButtonAction event) {
		if (threeSecTimer.isStarted()) {
			threeSecTimer.stop();
		}

		switch (event) {
			case BTN_CLICK:
	//			debugf("editField - click");
				screen->moveToNextValue();
				break;
			case BTN_DOUBLE_CLICK:
				screen->moveToPrevValue();
				break;
		}

		threeSecTimer.startOnce();
	};

	void setOnMenuEventDelegate(MenuEventDelegate handler) {

	};

private:
	Timer threeSecTimer;
	InfoScreens *screen;
};



