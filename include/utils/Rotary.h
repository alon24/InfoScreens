/*
 * Rotary.h
 *
 *  Created on: Dec 5, 2015
 *      Author: iklein
 */

#ifndef INCLUDE_UTILS_ROTARY_H_
#define INCLUDE_UTILS_ROTARY_H_

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "utils/MultiFunctionButton.h"

typedef Delegate<void()> RotaryButtonActionDelegate;

class Rotary {
private:
	int encoderCLK =12;
	int encoderDT =13;
	volatile int lastEncoded = 0;
	volatile long encoderValue = 0;
	MultiFunctionButton btn;

public:
	Rotary(){};
	~Rotary(){};

	Rotary(int btnPin, int encoderCLK, int encoderDT) {
		this->init(btnPin, encoderCLK, encoderDT);
	};

	void init(int btnPin, int encoderCLK, int encoderDT) {
		this->encoderCLK = encoderCLK;
		this->encoderDT = encoderDT;

		pinMode(btnPin, INPUT);
		digitalWrite(btnPin, HIGH); //turn pullup resistor on
		btn.initBtn(btnPin, ButtonActionDelegate(&Rotary::btnClicked, this));

		attachInterrupt(encoderCLK, RotaryButtonActionDelegate(&Rotary::updateEncoder, this), CHANGE);
		attachInterrupt(encoderDT, RotaryButtonActionDelegate(&Rotary::updateEncoder, this), CHANGE);
	};

	void updateEncoder() {
		int MSB = digitalRead(encoderCLK); //MSB = most significant bit
		int LSB = digitalRead(encoderDT); //LSB = least significant bit

		int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
		int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

		if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
		if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

		lastEncoded = encoded; //store this value for next time
		debugf("encoderValue=%i", encoderValue);
		//	  handleEncoderInterrupt();
	};

	void btnClicked(MultiFunctionButtonAction event) {
		switch (event) {
			case BTN_CLICK:
				debugf("click");
//				moveRight();
				break;
			case BTN_DOUBLE_CLICK:
				debugf("BTN_DOUBLE_CLICK");
//				moveLeft();
				break;
			case BTN_LONG_CLICK:
				debugf("BTN_LONG_CLICK");

//				if(getCurrent()->getallEditableParams().size() != 0) {
//					setViewMode(ViewMode::EDIT);
//				}
				break;
			case BTN_HOLD_CLICK:
				debugf("BTN_HOLD_CLICK");
				break;
		}
	}
};

#endif /* INCLUDE_UTILS_ROTARY_H_ */
