/*
 * SSD1306_Driver.cpp
 *
 *  Created on: Aug 17, 2015
 *      Author: ilan
 */
#include <drivers/SSD1306_Driver.h>

// initializer for I2C - we only indicate the reset pin!
SSD1306_Driver::SSD1306_Driver(int8_t reset) :
Adafruit_SSD1306(reset) {
}

int16_t SSD1306_Driver::getCursorX() {
	return cursor_x;
}

int16_t SSD1306_Driver::getCursorY() {
	return cursor_y;
}

textRect* SSD1306_Driver::print(const String &s){
	textRect *t = new textRect();
	t->x = cursor_x;
	t->y = cursor_y;
	t->h = 8;
	Adafruit_SSD1306::print(s);
	t->w = cursor_x - t->x;
//	Serial.printf("SSD1306_Driver::print %s %s,%s\n", s.c_str(), String(t->x).c_str(), String(t->y).c_str()  );
	return t;
}

void SSD1306_Driver::printToLoc(const String &s, textRect &t, int color){
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

void SSD1306_Driver::writeover(textRect &orig, const String &s, bool inverse) {
	int textColor = WHITE;
	int bkColor = BLACK;
	if (inverse) {
		textColor = BLACK;
		bkColor = WHITE;
	}

	this->fillRect( orig.x, orig.y, orig.w, orig.h, bkColor);
	this->printToLoc(s, orig, textColor);
}

textRect* SSD1306_Driver::getCurrentRect() {
	textRect *t = new textRect();
	t->x = cursor_x;
	t->y = cursor_y;
}

int SSD1306_Driver::getMaxLines() {
	return 6;
}
