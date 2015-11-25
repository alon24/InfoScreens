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

struct paramDataValues {
	//TODO:ilan maybe have Vector<String*>
	Vector<String> data;

	//for adding simple strings (one string)
	void initString(const String& str){
		data.clear();
		data.add(str);
	}

	void addValue(String val) {
		data.add(val);
	}

	//for adding chars like a-z for passwords
	void initChars(int start, int end) {
		data.clear();
		for (int i = 0;  i < (end-start); ++i) {
			data.add(String(i));
		}
	}

	//only the first (one) value)
	String getFirstValue(int index) {
		return data.elementAt(index);
	}

	//get all values
	//TODO:ilan maybe clone the vector
	Vector<String> getAllValues() {
		return data;
	}
};

//class letters {
//
//};

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

	void setEditable(bool state) {
		this->editable = state;
	}
};

struct paramStruct{
	textRect t;
	String id;

	//need to be able to set for each instance or param!!!
	boolean editable = false;

	void init(String id, String text, textRect t, bool edit=false) {
		this->id = id;
		this->t = t;
		this->editable = edit;
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
	paramStruct* addParam(String id, String text, bool editable = false, textRect* initial = NULL);
//	paramStruct* addParam(String id, String text, textRect initial);

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
private:
	Vector<InfoLine*> mChildren;
	String m_header;
	bool editable = false;
	int currentEditedLine = 0;;
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

	void setEditable(bool editable) {
		this->editable = editable;
	}

	int getNextEditLine();

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

	/**
	 * check if there are params on page which are editable
	 */
	bool checkEditModeAvailble();

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

enum class ViewMode {
	INFO = 0,
	EDIT = 1
};

class InfoScreens : public BaseInfoElement{

private:
	Vector<InfoPage*> mChildern;
	HashMap<String, paramData> paramValueMap;
	HashMap<String, paramDataValues> paramEditValueMap;

	bool updateDisplay = false;
	bool internalCanUpdateDisplay = true;
//	unsigned long lastUpdateTime = 0;
	Timer screenUpdateTimer;
	int btnPin=0;
	long lastClickTime = 0;
	BtnMode btnMode = BtnMode::None;
	int waitTimeForClick = 200;
	MultiFunctionButton btn;
	ViewMode viewMode = ViewMode::INFO;
	long lastEditModeBlinkTime = -1;
	long editModeBlinkTime = 800;
	bool blinkDrawn = false;
public:

	InfoScreens(String id, SSD1306_Driver *dis, int btnPin) : BaseInfoElement(id)
	{
		this->display = dis;
		setCurrent(0);

		this->btnPin = btnPin;
		btn.initBtn(btnPin);
		btn.enablePressAndHold(false);
		btn.setOnButtonEvent(ButtonActionDelegate(&InfoScreens::infoModeBtnClicked, this));

		screenUpdateTimer.setCallback(showScreenUpdateDelegate(&InfoScreens::handleScreenUpdateTimer, this));
		screenUpdateTimer.setIntervalMs(80);
		screenUpdateTimer.start(true);

		display->print("InfoScreens");
		Serial.print(display->getCursorY());
	}

	InfoPage* createPage(String id, String header);
	void addPage(InfoPage* page);
	void show() {
//		lastUpdateTime = millis();
		setCanUpdateDisplay(true);
	}

	void show(int pNum) {
		setCurrent(pNum);
//		debugf("show:%i", pNum);
		show();
	}

	void moveRight();
	void moveLeft();

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

	void setEditModeValues(String id, paramDataValues values);
	void updateParamValue(String id, String newData); //no screen update
	void setCanUpdateDisplay(bool newState);
	bool canUpdateDisplay();
	int count();
	void setViewMode(ViewMode mode);
	bool checkEditModeAvailble();

private:
	void handleScreenUpdateTimer();
	void print(int pIndex);
	void infoModeBtnClicked(MultiFunctionButtonAction event);
	void editModeBtnClicked(MultiFunctionButtonAction event);
	void setCurrent(int index);
	void drawEditModeSign(int x, int y);
};



#endif /* INCLUDE_INFOSCREENS_H_ */
