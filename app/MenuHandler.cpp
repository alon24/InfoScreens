/*
 * MenuHandler.cpp
 *
 *  Created on: Apr 24, 2016
 *      Author: iklein
 */

#include <SmingCore.h>
#include "InfoScreens.h"

class ThreeSecHandler {
public:
	ThreeSecHandler(InfoScreens *screen) {
		this->screen = screen;
		threeSecTimer.setIntervalMs(3000);
		threeSecTimer.setCallback(TimerDelegate(&ThreeSecHandler::threeSecondsPassed, this));
	};

	void threeSecondsPassed() {

	};

	void infoModeBtnClicked(MultiFunctionButtonAction event) {

	};

	void editModeBtnClicked(MultiFunctionButtonAction event) {

	};

	void editFieldModeBtnClicked(MultiFunctionButtonAction event) {

	};

	void setOnMenuEventDelegate(MenuEventDelegate handler) {

	};

private:
	Timer threeSecTimer;
	InfoScreens *screen;
};



