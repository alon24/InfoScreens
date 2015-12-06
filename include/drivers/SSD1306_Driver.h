/*
 * SSD1306_Driver.h
 *
 *  Created on: Aug 17, 2015
 *      Author: ilan
 */

#ifndef INCLUDE_SSD1306_DRIVER_H_
#define INCLUDE_SSD1306_DRIVER_H_

#include <Libraries/Adafruit_SSD1306/Adafruit_SSD1306.h>
#include "Base_Screen_Driver.h"
#include <SmingCore/SmingCore.h>

class SSD1306_Driver : public Base_Screen_Drive, public Adafruit_SSD1306 {
public:
	SSD1306_Driver(int8_t RST);

	int16_t getCursorX();
	int16_t getCursorY();

	textRect* print(const String &s);
	void printToLoc(const String &s, textRect &t, int textColor);
	void writeover(textRect &orig, const String &s, bool inverse = false);

	textRect* getCurrentRect();

	int getMaxLines();
};
#endif /* INCLUDE_SSD1306_DRIVER_H_ */
