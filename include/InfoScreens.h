/*
 * InfoScreens.h
 *
 *  Created on: Aug 20, 2015
 *      Author: ilan
 */

#ifndef INCLUDE_INFOSCREENS_H_
#define INCLUDE_INFOSCREENS_H_

#include <SmingCore/SmingCore.h>
#include <drivers/SSD1306_Driver.h>
#include "utils/MultiFunctionButton.h"

#define TIME_BETWEEN_SCREEN_CHANGE 300

struct paramData {
	bool dirty = false;
	String val;
	bool editable = false;
	Vector<String*> values;

	void update(String newVal) {
		val = newVal;
		dirty = true;
	}

	void clearDirty() {
		dirty = false;
	}

	void initEditable(Vector<String*> newVals){
		this->editable = true;
		values.clear();
		values.setSize(newVals.size());
//		values = newVals;
		for (int i=0; i<newVals.size(); ++i){
			values.add(newVals.elementAt(i));
		}
	}

	void setEditable(bool state) {
		this->editable = state;
	}

	void setValues(Vector< String> newVals) {
		this->values = values;
	}

	void addValue(String* data) {
		this->values.add(data);
	}

};

struct paramStruct{
	textRect t;
	String id;

	void init(String id, String text, textRect t) {
		this->id = id;
		this->t = t;
	}
};

class BaseInfoElement
{

protected:
	String id;
	BaseInfoElement* parent;
	SSD1306_Driver* display;

public:

	BaseInfoElement(String id) {
		this->id = id;
	};

	void setDisplay(SSD1306_Driver* disp) {
		this->display = disp;
	}

	String getId() {
		return id;
	};

	void setParent(BaseInfoElement* parent) {
		this->parent = parent;
	};

	BaseInfoElement* getParent() {
		return parent;
	}
	virtual void setCanUpdateDisplay(bool newState) {};
	virtual bool canUpdateDisplay() {};
	virtual void updateParamValue(String id, String newData) {};
	virtual paramData getParamText(String id){};
};

class InfoLine : public BaseInfoElement
{
	String m_text;
	int m_textSize;
	bool initialized = false;
public:

	Vector<paramStruct*> params;
	int mX, mY, mWidth;
	InfoLine(String id, String text, int size);
	int getTextSize();
	String getText();
	paramStruct* addParam(String id, String text);
	paramStruct* addParam(String id, String text, textRect initial);

	//prints the element
	void print();
	paramStruct* getParamById(String id);

	bool canUpdateDisplay(bool newState) {
		return parent->canUpdateDisplay();
	}

	paramData getParamText(String id){
		return parent->getParamText(id);
	}
};

class InfoPage : public BaseInfoElement{
	Vector<InfoLine*> mChildren;
	String m_header;
public:
	InfoPage(String id, String header) : BaseInfoElement(id) {
		m_header = header;
	};

	/**
	 * creates and adds to parent
	 */
	InfoLine* createLine(String id, String text) {
		InfoLine* el =  new InfoLine(id, text, 1);
		el->setParent(this);
		el->setDisplay(&*display);
		addElemenet(el);
		return el;
	}

	void addElemenet(InfoLine* el){
		el->setParent(this);
		el->setDisplay(&*display);
		mChildren.add(el);
	};

	InfoLine* itemAt(int index) {
		return mChildren.get(index);
	};

	Vector<InfoLine*> getItems()
	{
		return mChildren;
	};

	InfoLine* getChildById(String id) {
		for (int i = 0; i < mChildren.size(); ++i) {
			if(mChildren.get(i)->getId() == id) {
				return mChildren.get(i);
			}
		}
		return NULL;
	}

	Vector<paramStruct*> getAllParamsForId(String id) {
		Vector<paramStruct*> ret;
		for (int i = 0; i < mChildren.size(); ++i) {
			InfoLine* l = mChildren.elementAt(i);
			paramStruct* p = l->getParamById(id);
			if ( p != NULL) {
				ret.add(p);
			}
		}

		return ret;
	}

	Vector<paramStruct*> getAllParamsInPage() {
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

	//No screen update
	void updateParamValue(String id, String newData) {
		getParent()->updateParamValue(id, newData);
	}

	void print() {
		display->clearDisplay();
//		debugf("print,3.2 ");
		display->setCursor(0,0);
		for(int i=0; i< mChildren.size(); i++){
//			debugf("print,3.3 - %i ", i);
			InfoLine* child = mChildren.get(i);
			child->print();
//			debugf("print,3.4 - %i ", i);
		}
//		debugf("print,3.6 ");
		display->display();
//		debugf("print,3.7 ");
	}

	bool canUpdateDisplay() {
		return getParent()->canUpdateDisplay();
	}

	paramData getParamText(String id){
		return parent->getParamText(id);
	}
};

typedef Delegate<void()> showScreenUpdateDelegate;
//typedef Delegate<void()> btnUpdateDelegate;

enum class BtnMode {
	None = 0,
	Click = 1,
	DoubleClick = 2,
	ClickAndRun = 3
};

class InfoScreens : public BaseInfoElement{

private:
	Vector<InfoPage*> mChildern;
	HashMap<String, paramData> paramValueMap;

	bool updateDisplay = false;
	bool internalCanUpdateDisplay = true;
	unsigned long lastUpdateTime = 0;
	Timer screenupdate;

	int btnPin=0;
	long lastClickTime = 0;
	BtnMode btnMode = BtnMode::None;
	int waitTimeForClick = 200;

	MultiFunctionButton* btn;

public:

	InfoScreens(String id, SSD1306_Driver *dis, int btnPin) : BaseInfoElement(id)
	{
		this->display = dis;
		setCurrent(0);

//		pinMode(btnPin, INPUT_PULLUP);
//		attachInterrupt(btnPin, showScreenUpdateDelegate(&InfoScreens::buttonPinHandler, this), CHANGE);
		this->btnPin = btnPin;
		btn = new MultiFunctionButton(btnPin, ButtonActionDelegate(&InfoScreens::btnClicked, this));

		screenupdate.setCallback(showScreenUpdateDelegate(&InfoScreens::handleUpdateTimer, this));
		screenupdate.setIntervalMs(80);
		screenupdate.start(true);

		display->print("InfoScreens");
		Serial.print(display->getCursorY());
		display->display();
	}

	void handleUpdateTimer() {
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
				Vector<paramStruct*> params = getCurrent()->getAllParamsInPage();
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

	void setCurrent(int index) {
//		if (index >= mChildern.size()) {
//			return;
//		}
		debugf("setCurrent -cur=%i", index);
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

	void show() {
		lastUpdateTime = millis();
		setCanUpdateDisplay(true);
	}

	void show(int pNum) {
		setCurrent(pNum);
//		debugf("show:%i", pNum);
		show();
	}

	void doMove(boolean ) {

	}

	void moveRight() {
		if (mChildern.size() == 1) {
			return;
		}

		int tmpTime = millis();
		long mils = tmpTime - lastUpdateTime;
		if (mils < TIME_BETWEEN_SCREEN_CHANGE) {
			return;
		}
		lastUpdateTime = tmpTime;
		debugf("moveRight mills=%lu", lastUpdateTime);
		int current = paramValueMap["currentPage"].val.toInt();
		debugf("moveRight mCurrent=%i" , current);
		if (current + 1 < mChildern.size()) {
			current++;
		}
		else {
			current = 0;
		}
		debugf("moveRight mCurrent after=%i" , current);
		paramValueMap["currentPage"].update(String(current));
	}

	void moveLeft() {
		if (mChildern.size() == 1) {
			return;
		}
		int current = paramValueMap["currentPage"].val.toInt();
		debugf("moveLeft mCurrent=%i" , current);

		int tmpTime = millis();
		long mils = tmpTime - lastUpdateTime;
		if (mils < TIME_BETWEEN_SCREEN_CHANGE) {
			return;
		}

		lastUpdateTime = tmpTime;
		debugf("moveLeft mills=%lu", lastUpdateTime);

		if (current - 1 >= 0) {
			current--;
		}
		else {
			current = mChildern.size()-1;
		}

		debugf("moveLeft mCurrent after=%i" , current);
		paramValueMap["currentPage"].update(String(current));
	}

	InfoPage* createPage(String id, String header){
		InfoPage* el = new InfoPage(id, header);
		el->setParent(this);
		el->setDisplay(&*display);
		mChildern.add(el);
		return el;
	}

	void addPage(InfoPage* page) {
		page->setParent(this);
		page->setDisplay(&*display);
		mChildern.add(page);
	}

	InfoPage* get(int index) {
//		debugf("index = %i, children size  %i", index, mChildern.size());
		if (mChildern.size() >= index) {
			return mChildern.get(index);
		}
		return NULL;
	}

	InfoPage* getCurrent() {
		return mChildern.get(paramValueMap["currentPage"].val.toInt());
	}

	paramData getParamText(String id) {
		return paramValueMap[id];
	}

	//no screen update
	void updateParamValue(String id, String newData) {
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

	void setCanUpdateDisplay(bool newState);
	bool canUpdateDisplay();
	int count();

private:
	void print(int pIndex) {
		internalCanUpdateDisplay = false;

		InfoPage* p = get(pIndex);
//		debugf("print,3 %s", p->getId().c_str() );
		p->print();
//		debugf("print, 4");

		internalCanUpdateDisplay = true;
	}

	void buttonPinHandler() {
		int btnState =  digitalRead(btnPin);
		long current = millis();
		if(btnState == 1) {
			debugf( "pin %i, state is %i, elapsed=%d", btnPin, btnState, (current - lastClickTime ));

			if(btnMode == BtnMode::None) {
				btnMode = BtnMode::Click;
			}
//			else if(btnMode == ) {
//
//			}
	//		infos->moveRight();
			moveRight();
			lastClickTime = current;
		}
	}

	void btnClicked(MultiFunctionButtonAction event)
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
};

#endif /* INCLUDE_INFOSCREENS_H_ */
