/*
 * InfoScreens.cpp
 *
 *  Created on: Aug 20, 2015
 *      Author: ilan
 */
#include <InfoScreens.h>

InfoLine::InfoLine(String id, String text, int size = 1) : BaseInfoElement(id)
{
	m_text = text;
	if (size != 1)
	{
		m_textSize = size;
	}
	else
	{
		m_textSize = 1;
	}
}
;

int InfoLine::InfoLine::getTextSize()
{
	return m_textSize;
}
;

String InfoLine::InfoLine::getText()
{
	return m_text;
}

paramStruct* InfoLine::addParam(String id, String text)
{
	paramStruct* ret = new paramStruct();
	getParent()->updateParamValue(id, text);
//	ret->text = text;
	ret->t.x = -1;
	ret->t.y = -1;
	ret->t.h = -1;
	ret->t.w = -1;

	ret->id = id;
	params.add(ret);
	return ret;
}

paramStruct* InfoLine::addParam(String id, String text, textRect initial)
{
	paramStruct* ret = addParam(id, text);
	ret->t.x = initial.x;
	ret->t.y = initial.y;
	ret->t.h = initial.h;
	ret->t.w = initial.w;
	return ret;
}

//prints the element
void InfoLine::print()
{
//	debugf("print,3.3.1 %s, %i", getText().c_str(), this->display->getCursorX());
	textRect *t = this->display->print(getText());
//	debugf("print,3.3.2");
	int y = this->display->getCursorY();
//	debugf("print,3.3.3");
	for (int s = 0; s < params.size(); ++s)
	{
//		debugf("print,3.3.4 - %i", s);
		paramStruct* param = params.get(s);
		paramData str = getParamText(param->id);

		if (param->t.x != -1)
		{
//			debugf("print,3.3.5 - %i", s);
			this->display->setCursor(param->t.x, y);
//			debugf("print,3.3.55 - %i", s);
		}
		textRect* t = this->display->print(str.val);
		str.clearDirty();
		param->t = *t;
//		Serial.printf("x %i, y %i, w %i\n", t->x, t->y, t->w);
	}
//	debugf("print,3.3.6");
	this->display->println();
//	debugf("print,3.3.7");
//	Serial.printf("line end: %s, %s\n", String(t->x).c_str(), String(display.getCursorY()).c_str());
}

paramStruct* InfoLine::getParamById(String id)
{
	for (int i = 0; i < params.size(); ++i)
	{
		if (params.get(i)->id == id)
		{
			return params.get(i);
		}
	}
	return NULL;
}

bool InfoPage::checkEditModeAvailble(){
	Vector<paramStruct*> params(getAllParamsInPage());
	for (int i = 0; i < params.size(); ++i) {
		if (params.elementAt(i)->editable) {
			return true;
		}
	}
	return false;
}

void InfoScreens::handleUpdateTimer() {
//		debugf("can updatedisplay=%i", canUpdateDisplay());

	if(canUpdateDisplay() && internalCanUpdateDisplay) {
		if (mChildern.size() == 0) {
			debugf("I cannot print anything, no Pages declared, setting to NOT update display");
			setCanUpdateDisplay(false);
			return;
		}

		long current = millis();
		//in case of edit mode, flip every x mills the line (blink)
		if(viewMode == ViewMode::EDIT && current >= (lastEditModeBlinkTime + editModeBlinkTime)) {
			lastEditModeBlinkTime = current;
			blinkDrawn = !blinkDrawn;
			display->drawRect(0,0, 120, 100, blinkDrawn ? WHITE : BLACK);
		}

		if (paramValueMap["currentPage"].dirty) {
//				debugf("currentPage = %i, paramValueMap['currentPage'].dirty= %d",paramValueMap["currentPage"].val.toInt(), (int)paramValueMap["currentPage"].dirty);
			display->clearDisplay();
			display->setCursor(0,0);
			print(paramValueMap["currentPage"].val.toInt());
			paramValueMap["currentPage"].clearDirty();
		}
		else {
			internalCanUpdateDisplay = false;
			Vector<paramStruct*> params(getCurrent()->getAllParamsInPage());
//				debugf("params in page = %i", params.size());
			boolean updated = false;

			//need localcopy of params
			Vector<String> tempIds;
			for (int i = 0; i < params.size(); ++i) {
				paramStruct* param = params.get(i);
				if(paramValueMap[param->id].dirty) {
					tempIds.add(param->id);
					paramValueMap[param->id].clearDirty();
				}
			}

			for (int i = 0; i < params.size(); ++i) {
				paramStruct* param = params.get(i);
				if (tempIds.contains(param->id)) {
//						debugf("updating param %s", param->id.c_str());
					display->writeover(param->t, paramValueMap[param->id].val);
					updated = true;
				}
			}
			internalCanUpdateDisplay = true;
			if (updated) {
				display->display();
			}
		}
	}
}

void InfoScreens::setCanUpdateDisplay(bool newState){
		this->updateDisplay = newState;
	}

bool InfoScreens::canUpdateDisplay(){
	return updateDisplay;
}

int InfoScreens::count() {
	return mChildern.size();
}

void InfoScreens::infoModeBtnClicked(MultiFunctionButtonAction event)
{
	switch (event) {
		case BTN_CLICK:
			debugf("click");
			moveRight();
	//				handleClick();
			break;
		case BTN_DOUBLE_CLICK:
			debugf("BTN_DOUBLE_CLICK");
			moveLeft();
	//				handleDoubleClick();
			break;
		case BTN_HOLD_CLICK:
			debugf("BTN_HOLD_CLICK");
	//				handleLongClick();
			break;
		case BTN_LONG_CLICK:
			debugf("BTN_LONG_CLICK");
	//				handleHoldClick();
			break;
	}
}

void InfoScreens::editModeBtnClicked(MultiFunctionButtonAction event)
{
	switch (event) {
		case BTN_CLICK:
			debugf("click");
			moveRight();
//				handleClick();
			break;
		case BTN_DOUBLE_CLICK:
			debugf("BTN_DOUBLE_CLICK");
			moveLeft();
//				handleDoubleClick();
			break;
		case BTN_HOLD_CLICK:
			debugf("BTN_HOLD_CLICK");
//				handleLongClick();
			break;
		case BTN_LONG_CLICK:
			debugf("BTN_LONG_CLICK");
//				handleHoldClick();
			break;
	}
}

void InfoScreens::changeViewMode(ViewMode mode) {
	if (viewMode == mode) {
		return;
	}

	this->viewMode = mode;
	if(mode == ViewMode::INFO) {
		btn.setOnButtonEvent(ButtonActionDelegate(&InfoScreens::infoModeBtnClicked, this));
	} else {
		btn.setOnButtonEvent(ButtonActionDelegate(&InfoScreens::editModeBtnClicked, this));
	}
}

bool InfoScreens::checkEditModeAvailble() {
	return getCurrent()->checkEditModeAvailble();
}
