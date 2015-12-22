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

enum class RotaryAction {
	PREV = 0,
	NEXT = 1,
	NONE = 2
};

typedef Delegate<void()> RotaryButtonActionDelegate;
typedef Delegate<void(RotaryAction)> RotaryWheelActionDelegate;

class Rotary {

private:
	MultiFunctionButton* btn = NULL;
	int encoderCLK =12;
	int encoderDT =13;
	volatile int lastEncoded = 0;
	volatile long encoderValue = 0;
	long lastencoderValue = 0;
	int lastValue =-1000;
	ButtonActionDelegate delegatedActionEvent = NULL;
	RotaryWheelActionDelegate delegatedWheelEvent = NULL;
	Timer rotaryTimer;
	int rotaryChangeCountResolution=4;

	RotaryAction act = RotaryAction::NONE;

	volatile unsigned long threshold = 1000;


	// 'rotaryHalfSteps' is the counter of half-steps. The actual
	// number of steps will be equal to rotaryHalfSteps / 2
	//
	volatile long rotaryHalfSteps = 0;

	// Working variables for the interrupt routines
	//
	volatile unsigned long int0time = 0;
	volatile unsigned long int1time = 0;
	volatile uint8_t int0signal = 0;
	volatile uint8_t int1signal = 0;
	volatile uint8_t int0history = 0;
	volatile uint8_t int1history = 0;
	int lastRotarySteps =0;
	bool rotary = false;

public:
	Rotary(){
//		rotaryTimer.initializeMs(100, TimerDelegate(&Rotary::updateInternalRotaryState, this)).start();
	};

	~Rotary(){
		delete (btn);
		rotaryTimer.stop();
	};

	Rotary(int btnPin, int encoderCLK, int encoderDT) {
		Rotary();
		this->init(encoderCLK, encoderDT);
		this->initBtn(btnPin);
	};

	void init(int encoderCLK, int encoderDT) {
		this->encoderCLK = encoderCLK;
		this->encoderDT = encoderDT;

		attachInterrupt(encoderCLK, RotaryButtonActionDelegate(&Rotary::int0, this), CHANGE);
		attachInterrupt(encoderDT, RotaryButtonActionDelegate(&Rotary::int1, this), CHANGE);
	};

	void int0()
		{
		if ( micros() - int0time < threshold )
			return;
		int0history = int0signal;
		int0signal = digitalRead(encoderCLK);
		if ( int0history==int0signal )
			return;
		int0time = micros();
		if ( int0signal == int1signal )
			rotaryHalfSteps++;
		else
			rotaryHalfSteps--;

		showResult();
//		debugf("%i",rotaryHalfSteps);
		}

	void int1()
		{
		if ( micros() - int1time < threshold )
			return;
		int1history = int1signal;
		int1signal = digitalRead(encoderDT);
		if ( int1history==int1signal )
			return;
		int1time = micros();
//		debugf("%i",rotaryHalfSteps);
		}

	void showResult() {
		int tmp = rotaryHalfSteps;
		if (tmp %2 != 0 || rotary) {
			return;
		}

		if (tmp/2 == lastRotarySteps) {
			return;
		}

//		rotary = true;
//		if (lastRotarySteps > tmp) {
//			act = RotaryAction::PREV;
//		}
//		else {
//			act == RotaryAction::NEXT;
//		}

		lastRotarySteps = rotaryHalfSteps /2;

		debugf("%i",tmp/2);
//		rotary = false;
	}

	MultiFunctionButton* initBtn(int buttonPin, ButtonActionDelegate handler = null, bool pressAndHold = true) {
		if (!btn) {
			btn = new MultiFunctionButton();
		}

		pinMode(buttonPin, INPUT);
		digitalWrite(buttonPin, HIGH); //turn pullup resistor on
		btn->initBtn(buttonPin, handler, pressAndHold);
		return btn;
	};

	//Delegated call when event is triggered
	void setOnWheelEvent(RotaryWheelActionDelegate handler) {
		debugf("set setOnWheelEvent");
		delegatedWheelEvent  = handler;
	}

	MultiFunctionButton* getButton() {
		return btn;
	}

	void IRAM_ATTR updateEncoder() {
//		long current = millis();
////		implement some debounce
//		if (current - lastReadTime < 125) {
//			return;
//		}
//		lastReadTime = current;

		int MSB = digitalRead(encoderCLK); //MSB = most significant bit
		int LSB = digitalRead(encoderDT); //LSB = least significant bit

		int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
		int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

		if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
		if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

		lastEncoded = encoded; //store this value for next time
	};

	void IRAM_ATTR updateInternalRotaryState() {
		int tmp = lastRotarySteps;
		rotary = true;

//		if (tmp > lastencoderValue && (tmp - lastencoderValue >4)) {
//			act = RotaryAction::PREV;
//			debugf("Prev");
//			lastencoderValue = tmp;
//
//		}
//		else if (tmp < lastencoderValue && (lastencoderValue - tmp >4)){
//			act = RotaryAction::NEXT;
//			debugf("Next");
//			lastencoderValue = tmp;
//		}
//
		if (!delegatedWheelEvent) {
			debugf("no delegatedWheelEvent");
			return;
		}

		rotary = false;
		if (act != RotaryAction::NONE) {
			delegatedWheelEvent(act);
		}
		act = RotaryAction::NONE;
		debugf("rotary:none");
		rotary = true;
	}

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
				break;
			case BTN_HOLD_CLICK:
				debugf("BTN_HOLD_CLICK");
				break;
		}
	}
};

#endif /* INCLUDE_UTILS_ROTARY_H_ */
