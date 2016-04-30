/*
 * MenuHandlerInterface.h
 *
 *  Created on: Apr 30, 2016
 *      Author: iklein
 */

#ifndef INCLUDE_MENUHANDLERINTERFACE_H_
#define INCLUDE_MENUHANDLERINTERFACE_H_

#include "InfoScreens.h"

class MenuHandlerInterface {
public:
	MenuHandlerInterface(){}
//	virtual ~MenuHandlerInterface();

	virtual void infoModeBtnClicked(MultiFunctionButtonAction event);
	virtual void editModeBtnClicked(MultiFunctionButtonAction event);
	virtual void editFieldModeBtnClicked(MultiFunctionButtonAction event);
//	virtual void setOnMenuEventDelegate(MenuEventDelegate handler);
};


#endif /* INCLUDE_MENUHANDLERINTERFACE_H_ */
