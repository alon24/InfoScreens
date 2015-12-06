/*
 * ILI9341_Driver.h
 *
 *  Created on: Dec 6, 2015
 *      Author: iklein
 */

#ifndef INCLUDE_DRIVERS_ILI9341_DRIVER_H_
#define INCLUDE_DRIVERS_ILI9341_DRIVER_H_

#include <Libraries/Adafruit_ILI9341/Adafruit_ILI9341.h>
#include "Base_Screen_Driver.h"
#include <SmingCore/SmingCore.h>

class ILI9341_Driver : public Base_Screen_Drive, public Adafruit_ILI9341 {
	int16_t getCursorX();
	int16_t getCursorY();

	textRect* print(const String &s);
	void printToLoc(const String &s, textRect &t, int textColor = ILI9341_WHITE);
	void writeover(textRect &orig, const String &s, bool inverse = false);

	textRect* getCurrentRect();
	int getMaxLines();
};



#endif /* INCLUDE_DRIVERS_ILI9341_DRIVER_H_ */
