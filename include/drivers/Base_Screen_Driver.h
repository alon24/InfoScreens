/*
 * Base_Screen_Driver.h
 *
 *  Created on: Dec 6, 2015
 *      Author: iklein
 */

#ifndef INCLUDE_DRIVERS_BASE_SCREEN_DRIVER_H_
#define INCLUDE_DRIVERS_BASE_SCREEN_DRIVER_H_

#include <SmingCore/SmingCore.h>
#include <Adafruit_GFX/Adafruit_GFX.h>

struct textRect {
	int x = -1;
	int y =-1;
	int h = -1;
	int w=-1;
};

class Base_Screen_Drive : public Adafruit_GFX{
public:
	 int16_t getCursorX();
	 int16_t getCursorY();

	virtual textRect* print(const String &s) = 0;
	virtual void printToLoc(const String &s, textRect &t, int textColor) = 0;
	virtual void writeover(textRect &orig, const String &s, bool inverse = false) = 0;
	virtual textRect* getCurrentRect() = 0;
	virtual int getMaxLines() = 0;

	virtual void clearDisplay(void);

	virtual void display();
};

#endif /* INCLUDE_DRIVERS_BASE_SCREEN_DRIVER_H_ */
