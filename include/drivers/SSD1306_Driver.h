/*
 * SSD1306_Driver.h
 *
 *  Created on: Aug 17, 2015
 *      Author: ilan
 */

#ifndef INCLUDE_SSD1306_DRIVER_H_
#define INCLUDE_SSD1306_DRIVER_H_

#include <Libraries/Adafruit_SSD1306/Adafruit_SSD1306.h>

struct textRect {
	int x = -1;
	int y =-1;
	int h = -1;
	int w=-1;
};

class SSD1306_Driver : public Adafruit_SSD1306 {
public:
	SSD1306_Driver(int8_t RST);

	int16_t getCursorX();
	int16_t getCursorY();

	textRect* print(const String &s);
	void printToLoc(const String &s, textRect &t);
	void writeover(textRect &orig, const String &s);

	textRect* getCurrentRect();

	void blink(textRect loc, bool showMarker);
	int getMaxLines();
};
#endif /* INCLUDE_SSD1306_DRIVER_H_ */
