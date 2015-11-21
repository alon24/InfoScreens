/*
 * InfoScreens.h
 *
 *  Created on: Aug 20, 2015
 *      Author: ilan
 */

#ifndef INCLUDE_INFOSCREENS_H_
#define INCLUDE_INFOSCREENS_H_

#include <SmingCore/SmingCore.h>
#include <Extended_SSD1306.h>

#define TIME_BETWEEN_SCREEN_CHANGE 300

struct paramState {
	bool dirty = false;
	String val;

	void update(String newVal) {
		val = newVal;
		dirty = true;
	}

	void clearDirty() {
		dirty = false;
	}
};

struct paramStruct{
	textRect t;
//	String text;
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
	Extended_SSD1306* display;

public:

	BaseInfoElement(String id) {
		this->id = id;
	};

	void setDisplay(Extended_SSD1306* disp) {
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
	virtual paramState getParamText(String id){};
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
//	void updateData(Extended_SSD1306 display, paramStruct* param, String newData);
//	void updateData(Extended_SSD1306 display, int index, String newData);
//	void updateDataForId(Extended_SSD1306 display, String id, String newData);

//	paramStruct* getParam(int index);
	paramStruct* getParamById(String id);

	bool canUpdateDisplay(bool newState) {
		return parent->canUpdateDisplay();
	}

	paramState getParamText(String id){
		return parent->getParamText(id);
	}

//	void updateParamValue(String id, String value) {
//		getParent()->getParent()->updateParamValue(id, value);
//	}
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

//	//Does NOT call display.display();
//	void updateParam( String id, String newVal, Extended_SSD1306 display) {
//		Vector<InfoLine*> ret = getAllLinesParamsForId(id);
//		for (int z = 0; z < ret.size(); ++z) {
//			ret.elementAt(z)->updateDataForId(display, id, newVal);
//		}
//
//		ret.removeAllElements();
//	}
//
//	void updateAllParams( Vector<String> ids, Vector<String> newVals, Extended_SSD1306 display) {
//		for (int i = 0; i < ids.size(); ++i) {
//			updateParam(ids.elementAt(i), newVals.elementAt(i), display);
//		}
//
//		display.display();
//	}

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
//		if(!canUpdateDisplay()) {
//			Serial.println("Cannot Update display, flag is false");
//			return;
//		}
//		debugf("print,3.1 ");
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

	paramState getParamText(String id){
		return parent->getParamText(id);
	}
};

typedef Delegate<void()> showScreenUpdateDelegate;

class InfoScreens : public BaseInfoElement{

private:
//	int mCurrent=0;
	Vector<InfoPage*> mChildern;
//	Vector<paramStruct*> dirtyElements;
	HashMap<String, paramState> paramValueMap;

	bool updateDisplay = false;
	bool internalCanUpdateDisplay = true;
	unsigned long lastUpdateTime = 0;
//	Timer updateNextTimer;
	Timer screenupdate;

public:
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

//	InfoScreens(String id) : BaseInfoElement(id) {
//
//	}

	InfoScreens(String id, Extended_SSD1306 *dis) : BaseInfoElement(id)
	{
		this->display = dis;
		setCurrent(0);

		screenupdate.setCallback(showScreenUpdateDelegate(&InfoScreens::handleUpdateTimer, this));
		screenupdate.setIntervalMs(80);
		screenupdate.start(true);

		display->print("InfoScreens");
		Serial.print(display->getCursorY());
		display->display();
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
			paramState p;
			p.update(String(index));
//			debugf("setCurrent - new param -cur= %i, %s",index, String(index).c_str());
			paramValueMap["currentPage"] = p;
		}
//		mCurrent = index;
	}

	void show() {
		lastUpdateTime = millis();
//		debugf("showCurrent,1");
//		this->updateDisplay = true;
		setCanUpdateDisplay(true);
//		debugf("showCurrent,2");

		//handle printing in timer
//		print(paramValueMap["currentPage"S].val);


//		debugf("showCurrent,3");
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
//		display->clearDisplay();
//		display->setCursor(0,0);
//		showCurrent();
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

//		this->display->clearDisplay();
//		display->setCursor(0,0);
//		showCurrent();
	}

//	//Do not print to screen anymore(so no updates to data)
//	void hide(){
//		this->updateDisplay = false;
//	}

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

	paramState getParamText(String id) {
		return paramValueMap[id];
	}

	//no screen update
	void updateParamValue(String id, String newData) {
		if (paramValueMap.contains(id)) {
			paramValueMap[id].update(newData);
//			paramValueMap.remove(id);
		}

		paramState p;
		p.update(newData);
		paramValueMap[id] = p;

//		if (canUpdateDisplay()) {
//			Vector<paramStruct*> params = getCurrent()->getAllParamsForId(id);
//			for (int i = 0; i < params.size(); ++i) {
//				paramStruct* param = params.get(i);
//				display->writeover(param->t, newData);
//			}
//		}
	}

	void setCanUpdateDisplay(bool newState);
	bool canUpdateDisplay();
	int count();

private:
	void print(int pIndex) {
//		if(!canUpdateDisplay()) {
//			Serial.println("Cannot Update display, flag is false");
//			return;
//		}
//		Serial.println("Printing index = " +String(pIndex));

		internalCanUpdateDisplay = false;

		InfoPage* p = get(pIndex);
//		debugf("print,3 %s", p->getId().c_str() );
		p->print();
//		debugf("print, 4");

//		resetAllParamsDirtyState(pIndex);
		internalCanUpdateDisplay = true;
	}

	void resetAllParamsDirtyState(int pIndex) {
		InfoPage* p = get(pIndex);
		Vector<paramStruct*> pars = p->getAllParamsInPage();
		debugf("resetAllParamsDirtyState, 1");
		for(int i=0; i<pars.size(); i++) {
			paramValueMap[pars.elementAt(i)->id].clearDirty();
			debugf("resetAllParamsDirtyState, 2, %s", pars.elementAt(i)->id.c_str());
		}
	}

};

#endif /* INCLUDE_INFOSCREENS_H_ */
