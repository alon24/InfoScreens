/*
 * ILI9341_Driver.cpp
 *
 *  Created on: Aug 17, 2015
 *      Author: ilan
 */
#include <drivers/ILI9341_Driver.h>
#include <SmingCore/SmingCore.h>

int16_t ILI9341_Driver::getCursorX() {
	return cursor_x;
}

int16_t ILI9341_Driver::getCursorY() {
	return cursor_y;
}

textRect* ILI9341_Driver::print(const String &s){
	textRect *t = new textRect();
	t->x = cursor_x;
	t->y = cursor_y;
	t->h = 8;
	Adafruit_ILI9341::print(s);
	t->w = cursor_x - t->x;
//	Serial.printf("ILI9341_Driver::print %s %s,%s\n", s.c_str(), String(t->x).c_str(), String(t->y).c_str()  );
	return t;
}

void ILI9341_Driver::printToLoc(const String &s, textRect &t, int color){
	setCursor(t.x, t.y);
//	uint32_t free = system_get_free_heap_size();
//	int ff = (int)free;
//	Serial.printf("free printToLoc= %i", ff );

	int tempTextColor = textcolor;
	setTextColor(color);

	textRect* newt = this->print(s);
	t.x = newt->x;
	t.y = newt->y;
	t.h = newt->h;
	t.w = newt->w;

	delete(newt);

	setTextColor(tempTextColor);
//	free = system_get_free_heap_size();
//	ff = (int)free;
//	Serial.printf(", 2= %i\n", ff );
}

void ILI9341_Driver::writeover(textRect &orig, const String &s, bool inverse) {
	int textColor = ILI9341_WHITE;
	int bkColor = ILI9341_BLACK;
	if (inverse) {
		textColor = ILI9341_BLACK;
		bkColor = ILI9341_WHITE;
	}

	this->fillRect( orig.x, orig.y, orig.w, orig.h, bkColor);
	this->printToLoc(s, orig, textColor);
}

textRect* ILI9341_Driver::getCurrentRect() {
	textRect *t = new textRect();
	t->x = cursor_x;
	t->y = cursor_y;
}

int ILI9341_Driver::getMaxLines() {
	return 6;
}

