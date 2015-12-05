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

typedef Delegate<void()> RotaryButtonActionDelegate;

class Rotary {
private:
	int encoderCLK =12;
	int encoderDT =13;
	int btnPin =14;
	volatile int lastEncoded = 0;
	volatile long encoderValue = 0;

public:
	Rotary(){};
	~Rotary(){};

	Rotary(int btnPin, int encoderCLK, int encoderDT) {
		this->init(btnPin, encoderCLK, encoderDT);
	};

	void init(int btnPin, int encoderCLK, int encoderDT) {
		this->btnPin = btnPin;
		this->encoderCLK = encoderCLK;
		this->encoderDT = encoderDT;

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
		//	  handleEncoderInterrupt();
	};
};

#endif /* INCLUDE_UTILS_ROTARY_H_ */
