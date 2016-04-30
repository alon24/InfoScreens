/*
 * MenuHandlers.h
 *
 *  Created on: Apr 30, 2016
 *      Author: iklein
 */

#ifndef INCLUDE_MENUHANDLERS_H_
#define INCLUDE_MENUHANDLERS_H_

#include <SmingCore.h>
#include "MenuHandlerInterface.h"
#include "InfoScreens.h"

class InfoScreens; //this is called forward declaration
//class MenuEventDelegate;
enum class ViewMode;

class ThreeSecondMenuHandler : public MenuHandlerInterface{
public:
	ThreeSecondMenuHandler(InfoScreens *screen);
//{
//		this->infos = screen;
//		threeSecTimer.setIntervalMs(3000);
////		threeSecTimer.setCallback(TimerDelegate(&ThreeSecondMenuHandler::threeSecondsPassed, this));
//	};

	virtual void infoModeBtnClicked(MultiFunctionButtonAction event);
	virtual void editModeBtnClicked(MultiFunctionButtonAction event);
	virtual void editFieldModeBtnClicked(MultiFunctionButtonAction event);

	void threeSecondsPassed();
//
//	void infoModeBtnClicked(MultiFunctionButtonAction event) {
//		switch (event) {
//			case BTN_CLICK:
//				debugf("click");
//				infos->moveRight();
//				break;
//			case BTN_DOUBLE_CLICK:
//	//			debugf("BTN_DOUBLE_CLICK");
//				infos->moveLeft();
//				break;
//			case BTN_LONG_CLICK:
//	//			debugf("BTN_LONG_CLICK, going to edit mode, %i", getCurrent()->getallEditableParams().size());
//
//				if(infos->getCurrent()->getallEditableParams().size() != 0) {
//					infos->setViewMode(ViewMode::EDIT);
//					threeSecTimer.startOnce();
//				}
//				break;
//			case BTN_HOLD_CLICK:
//	//			debugf("BTN_HOLD_CLICK, going to edit mode");
//				break;
//		}
//	};
//
//	void editModeBtnClicked(MultiFunctionButtonAction event) {
//		if (threeSecTimer.isStarted()) {
//			threeSecTimer.stop();
//		}
//
//		switch (event) {
//			case BTN_CLICK:
//				debugf("edit - click");
//				infos->moveToNextEditParam();
//	//			moveRight();
//	//				handleClick();
//				break;
//			case BTN_DOUBLE_CLICK:
//				infos->moveToPrevEditParam();
//				break;
////			case BTN_LONG_CLICK:
////	//			debugf("edit - BTN_LONG_CLICK");
////				setViewMode(ViewMode::EDIT_FIELD);
////	//				handleHoldClick();
////				break;
////			case BTN_HOLD_CLICK:
////				debugf("BTN_HOLD_CLICK");
////				break;
//		}
//
//		threeSecTimer.startOnce();
//	};
//
//	void editFieldModeBtnClicked(MultiFunctionButtonAction event) {
//		if (threeSecTimer.isStarted()) {
//			threeSecTimer.stop();
//		}
//
//		switch (event) {
//			case BTN_CLICK:
//	//			debugf("editField - click");
//				infos->moveToNextValue();
//				break;
//			case BTN_DOUBLE_CLICK:
//				infos->moveToPrevValue();
//				break;
//		}
//
//		threeSecTimer.startOnce();
//	};
//
//	void setOnMenuEventDelegate(MenuEventDelegate handler) {
//
//	};

private:
	Timer threeSecTimer;
	InfoScreens *infos;
};

#endif /* INCLUDE_MENUHANDLERS_H_ */
