/*
 * InfoScreens.cpp
 *
 *  Created on: Aug 20, 2015
 *      Author: ilan
 */
#include <InfoScreens.h>

InfoLine::InfoLine(String text, int size = 1) : BaseInfoElement()
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
};

bool InfoLine::isEditable(){
	for (int i = 0; i < params.size(); ++i) {
		if (params.elementAt(i)->editable) {
			return true;
		}
	}

	return false;
}

paramStruct* InfoLine::getEditableParam(int index){
	return params.elementAt(index);
}

String InfoLine::InfoLine::getText()
{
	return m_text;
}

//Not handling initial yet
paramStruct* InfoLine::addParam(String id, String text, bool editable, int maxLineSize)
{
	paramStruct* ret = new paramStruct();
//	getParent()->updateParamValue(id, text);
	updateParamValue(id, text);
//	ret->text = text;
	ret->t.x = -1;
	ret->t.y = -1;
	ret->t.h = -1;
	ret->t.w = -1;

	ret->id = id;
	ret->editable = editable;
	ret->maxSize = maxLineSize * 8;
	params.add(ret);
	return ret;
}

//paramStruct* InfoLine::addParam(String id, String text, textRect initial)
//{
//	paramStruct* ret = addParam(id, text);
//	ret->t.x = initial.x;
//	ret->t.y = initial.y;
//	ret->t.h = initial.h;
//	ret->t.w = initial.w;
//	return ret;
//}

//prints the element
void InfoLine::print()
{
//	debugf("start mem %d",system_get_free_heap_size());
//	debugf("print,3.3.1 %s, %i", getText().c_str(), this->display->getCursorX());
	textRect *t1 = this->display->print(getText());
	delete(t1);
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
		param->t.x = t->x;
		param->t.y = t->y;
		param->t.h = t->h;
		param->t.w = t->w;
//		param->t = *t;
		delete(t);
//		Serial.printf("x %i, y %i, w %i\n", t->x, t->y, t->w);
	}
//	debugf("print,3.3.6");
	this->display->println();
//	debugf("end mem %d",system_get_free_heap_size());
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

Vector<paramStruct*> InfoPage::getAllParamsInPage() {
	Vector<paramStruct*> ret;
	for (int i = 0; i < mChildren.size(); ++i) {
//			debugf("222");
		InfoLine* l = mChildren.elementAt(i);
		//add all the params in the line
		for (int j = 0; j < l->params.size(); ++j) {
			ret.add(l->params.elementAt(j));
		}
	}

	return ret;
}

Vector<paramStruct*> InfoPage::getallEditableParams(){
	Vector<paramStruct*> v = getAllParamsInPage();
	Vector<paramStruct*> ret;
	for (int i = 0; i < v.size(); ++i) {
		paramStruct* p = v.elementAt(i);
		if (p->editable) {
			ret.add(p);
		}
	}

	return ret;
}

paramStruct* InfoPage::getCurrentEditParam(){
	if (currentEditedParam == -1) {
		return NULL;
	}

	Vector<paramStruct*> v = getallEditableParams();
	return v.elementAt(currentEditedParam);
}

paramStruct* InfoPage::movetoNextEditParam(){
	Vector<paramStruct*> v = getallEditableParams();
	currentEditedParam++;
	if (currentEditedParam>=v.size()){
		currentEditedParam = 0;
	}
	debugf("getNextEditParam next param is %i", currentEditedParam);
	return v.get(currentEditedParam);
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

void InfoPage::initEditMode() {
	currentEditedParam = -1;
}


InfoPage* InfoScreens::createPage(String header){
	InfoPage* el = new InfoPage(header);
	el->setParent(this);
	el->setDisplay(&*display);
	mChildern.add(el);
	return el;
}

//InfoScreens

//no screen update
void InfoScreens::addPage(InfoPage* page) {
	page->setParent(this);
	page->setDisplay(&*display);
	mChildern.add(page);
}

void InfoScreens::setEditModeValues(String id, paramDataValues values){
	paramEditValueMap[id] = values;
}

void InfoScreens::updateParamValue(String id, String newData) {
	if (paramValueMap.contains(id)) {
		paramValueMap[id].update(newData);
//			paramValueMap.remove(id);
	}
	else {
		paramData p;
		p.update(newData);
		paramValueMap[id] = p;
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

void InfoScreens::setViewMode(ViewMode mode) {
	if (viewMode == mode) {
		return;
	}

	this->viewMode = mode;
	if(mode == ViewMode::INFO) {
		btn.enablePressAndHold(false);
		btn.setOnButtonEvent(ButtonActionDelegate(&InfoScreens::infoModeBtnClicked, this));

	} else {
		btn.enablePressAndHold(true);
		btn.setOnButtonEvent(ButtonActionDelegate(&InfoScreens::editModeBtnClicked, this));
		moveToNextEditParam();
	}

}

bool InfoScreens::checkEditModeAvailble() {
	return getCurrent()->checkEditModeAvailble();
}

void InfoScreens::moveRight() {
//		debugf("start mem %d",system_get_free_heap_size());
	if (mChildern.size() == 1) {
		return;
	}

////		debugf("moveRight mills=%lu", lastUpdateTime);
	int current = paramValueMap["currentPage"].val.toInt();

//		debugf("moveRight mCurrent=%i" , current);
	if (current + 1 < mChildern.size()) {
		current++;
	}
	else {
		current = 0;
	}
//		debugf("moveRight mCurrent after=%i" , current);
	paramValueMap["currentPage"].update(String(current));
//		debugf("end mem %d",system_get_free_heap_size());
}

void InfoScreens::moveLeft() {
	if (mChildern.size() == 1) {
		return;
	}
	int current = paramValueMap["currentPage"].val.toInt();
//		debugf("moveLeft mCurrent=%i" , current);

	if (current - 1 >= 0) {
		current--;
	}
	else {
		current = mChildern.size()-1;
	}

//		debugf("moveLeft mCurrent after=%i" , current);
	paramValueMap["currentPage"].update(String(current));
}

paramStruct* InfoScreens::moveToNextEditParam(){
	editModeInfo.setLastSelected(getCurrent()->getCurrentEditParam());
//	editModeInfo.setLastSelected(getCurrent()->getCurrentEditParam());
	paramStruct* ret = getCurrent()->movetoNextEditParam();
	editModeInfo.reset();
	return ret;
}

//InfoScreeens private

void InfoScreens::handleScreenUpdateTimer() {
//		debugf("can updatedisplay=%i", canUpdateDisplay());

	if(canUpdateDisplay() && internalCanUpdateDisplay) {
		if (mChildern.size() == 0) {
			debugf("I cannot print anything, no Pages declared, setting to NOT update display");
			setCanUpdateDisplay(false);
			return;
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

			if (editModeInfo.shouldEraseLast()) {
				drawBlinkParamLine(editModeInfo.lastSelectedParam, BLACK);
				editModeInfo.lastSelectedParam = NULL;
			}

			long current = millis();
			int color = BLACK;
			editModeInfo.handleTimeElapsed(current);

			if (viewMode == ViewMode::EDIT) {
				if (editModeInfo.blinkDrawn) {
					color = WHITE;
				}

				drawEditModeSign(124, 0, color);
				if (getCurrent()->getCurrentEditParam()) {
					paramStruct* p = getCurrent()->getCurrentEditParam();
					drawBlinkParamLine(p, color);
				}
			}
		}
	}
}

void InfoScreens::drawBlinkParamLine(paramStruct* p, int color){
	display->drawFastHLine(p->t.x, p->t.y+6+1, p->maxSize, color);
}

void InfoScreens::drawEditModeSign(int x, int y, int color) {
	display->drawFastHLine(x, y, 3, color);
	display->drawFastHLine(x, y+2, 3, color);
	display->drawFastHLine(x, y+4, 3, color);
	display->drawFastVLine(x, y, y+4, color);
}

void InfoScreens::print(int pIndex) {
	internalCanUpdateDisplay = false;

	InfoPage* p = get(pIndex);
//		debugf("print,3 %s", p->getId().c_str() );
	p->print();
//		debugf("print, 4");
	internalCanUpdateDisplay = true;
}

void InfoScreens::infoModeBtnClicked(MultiFunctionButtonAction event)
{
	switch (event) {
		case BTN_CLICK:
			debugf("click");
			moveRight();
			break;
		case BTN_DOUBLE_CLICK:
			debugf("BTN_DOUBLE_CLICK");
			moveLeft();
			break;
		case BTN_LONG_CLICK:
			debugf("BTN_LONG_CLICK, going to edit mode");
			setViewMode(ViewMode::EDIT);
			break;
		case BTN_HOLD_CLICK:
			debugf("BTN_HOLD_CLICK, going to edit mode");
			break;
	}
}

void InfoScreens::editModeBtnClicked(MultiFunctionButtonAction event)
{
	switch (event) {
		case BTN_CLICK:
			debugf("edit - click");
			moveToNextEditParam();
//			moveRight();
//				handleClick();
			break;
		case BTN_DOUBLE_CLICK:
			debugf("return to View mode");
			setViewMode(ViewMode::INFO);
			show();
//			moveLeft();
//				handleDoubleClick();
			break;
		case BTN_LONG_CLICK:
			debugf("edit - BTN_LONG_CLICK");
//				handleHoldClick();
			break;
		case BTN_HOLD_CLICK:
			debugf("BTN_HOLD_CLICK");
			break;
	}
}

void InfoScreens::setCurrent(int index) {
//		if (index >= mChildern.size()) {
//			return;
//		}
	debugf("setCurrent-cur=%i", index);
	if (paramValueMap.contains("currentPage")) {
		paramValueMap["currentPage"].update(String(index));
	}
	else {
		paramData p;
		p.update(String(index));
//			debugf("setCurrent - new param -cur= %i, %s",index, String(index).c_str());
		paramValueMap["currentPage"] = p;
	}
//		mCurrent = index;
}
