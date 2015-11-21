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
		paramState str = getParamText(param->id);

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

//void InfoLine::updateData(Extended_SSD1306 display, paramStruct* param,
//		String newData)
//{
//	//		uint32_t free = system_get_free_heap_size();
//	//		int ff = (int)free;
//	//		Serial.printf("free1= %i", ff );
//
//	display.writeover(param->t, newData);
//	//		free = system_get_free_heap_size();
//	//		ff = (int)free;
//	//		Serial.printf(", 2= %i", ff );
//
//	param->text = newData;
//	//		free = system_get_free_heap_size();
//	//		ff = (int)free;
//	//		Serial.printf(", 3= %i\n", ff );
//}

//void InfoLine::updateData(Extended_SSD1306 display, int index,
//		String newData)
//{
//	paramStruct* p = params.get(index);
//	this->updateData(display, p, newData);
//}
//
//void InfoLine::updateDataForId(Extended_SSD1306 display, String id,
//		String newData)
//{
//	paramStruct* p = getParamById(id);
//	if (p != NULL)
//	{
//		this->updateData(display, p, newData);
//	}
//}

//paramStruct* InfoLine::getParam(int index)
//{
//	return params.get(index);
//}

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

void InfoScreens::setCanUpdateDisplay(bool newState){
		this->updateDisplay = newState;
	}

bool InfoScreens::canUpdateDisplay(){
	return updateDisplay;
}

int InfoScreens::count() {
	return mChildern.size();
}
