#ifndef INCLUDE_ButtonActions_
#define INCLUDE_ButtonActions_

/*
 * ButtonCLick.cpp
 *
 *  Created on: Jun 1, 2015
 *      Author: ilan
 */

#include <user_config.h>
#include <SmingCore/SmingCore.h>

//This class makes use of:
/* 4-Way Button:  Click, Double-Click, Press+Hold, and Press+Long-Hold Test Sketch

 By Jeff Saltzman
 Oct. 13, 2009

 1) Click:  rapid press and release
 2) Double-Click:  two clicks in quick succession
 3) Press and Hold:  holding the button down
 4) Long Press and Hold:  holding the button for a long time
 */

enum ButtonAction { BTN_CLICK=1, BTN_DOUBLE_CLICK=2, BTN_LONG_CLICK=3, BTN_HOLD_CLICK=4};

struct ButtonUseEvent {
	const ButtonAction &action;
};

typedef Delegate<void(ButtonAction)> btnUpdateDelegate;
//typedef Delegate<void(String commandLine  ,CommandOutput* commandOutput)> commandFunctionDelegate;

//typedef void (*cb_use)(ButtonUseEvent);

class ButtonActions
{

//=================================================
//  MULTI-CLICK:  One Button, Multiple Events

// Button timing variables
		int debounce = 20; // ms debounce period to prevent flickering when pressing or releasing the button
		int DCgap = 250;       // max ms between clicks for a double click event
		int holdTime = 1000; // ms hold period: how long to wait for press+hold event
		int longHoldTime = 3000; // ms long hold period: how long to wait for press+hold event

// Button variables
		boolean buttonVal = HIGH;   // value read from button
		boolean buttonLast = HIGH; // buffered value of the button's previous state
		boolean DCwaiting = false; // whether we're waiting for a double click (down)
		boolean DConUp = false; // whether to register a double click on next release, or whether to wait and click
		boolean singleOK = true;    // whether it's OK to do a single click
		long downTime = -1;         // time the button was pressed down
		long upTime = -1;           // time the button was released
		boolean ignoreUp = false; // whether to ignore the button release because the click+hold was triggered
		boolean waitForUp = false; // when held, whether to wait for the up event
		boolean holdEventPast = false; // whether or not the hold event happened already
		boolean longHoldEventPast = false; // whether or not the long hold event happened already
		int m_buttonPin;
		btnUpdateDelegate buttonUseDelegate;
//		cb_use cb_buttonUse;

		Timer buttonTimer;

	public:
		ButtonActions(int buttonPin, btnUpdateDelegate buttonUseDelegate)
		{
			m_buttonPin = buttonPin;
//			cb_buttonUse = buttonUse;
			this->buttonUseDelegate = buttonUseDelegate;
			buttonTimer.initializeMs(80, TimerDelegate(&ButtonActions::actOnButton, this)).start();
		};

		int checkButton()
		{
			int event = 0;
			buttonVal = digitalRead(m_buttonPin);
			// Button pressed down
			if (buttonVal == LOW && buttonLast == HIGH
					&& (millis() - upTime) > debounce)
			{
				downTime = millis();
				ignoreUp = false;
				waitForUp = false;
				singleOK = true;
				holdEventPast = false;
				longHoldEventPast = false;
				if ((millis() - upTime)
						< DCgap&& DConUp == false && DCwaiting == true)
					DConUp = true;
				else
					DConUp = false;
				DCwaiting = false;
			}
			// Button released
			else if (buttonVal == HIGH && buttonLast == LOW
					&& (millis() - downTime) > debounce)
			{
				if (not ignoreUp)
				{
					upTime = millis();
					if (DConUp == false)
						DCwaiting = true;
					else
					{
						event = 2;
						DConUp = false;
						DCwaiting = false;
						singleOK = false;
					}
				}
			}
			// Test for normal click event: DCgap expired
			if (buttonVal == HIGH && (millis() - upTime) >= DCgap
					&& DCwaiting == true && DConUp == false && singleOK == true
					&& event != 2)
			{
				event = 1;
				DCwaiting = false;
			}
			// Test for hold
			if (buttonVal == LOW && (millis() - downTime) >= holdTime)
			{
				// Trigger "normal" hold
				if (not holdEventPast)
				{
					event = 3;
					waitForUp = true;
					ignoreUp = true;
					DConUp = false;
					DCwaiting = false;
					//downTime = millis();
					holdEventPast = true;
				}
				// Trigger "long" hold
				if ((millis() - downTime) >= longHoldTime)
				{
					if (not longHoldEventPast)
					{
						event = 4;
						longHoldEventPast = true;
					}
				}
			}
			buttonLast = buttonVal;
			return event;
		}

		void actOnButton()
		{
			// Get button event and act accordingly
			int b = checkButton();
			ButtonAction act;
			if (b == 1)
				act = BTN_CLICK;
			if (b == 2)
				act = BTN_DOUBLE_CLICK;
			if (b == 3)
				act = BTN_LONG_CLICK;
			if (b == 4)
				act = BTN_HOLD_CLICK;

			if ((b==1 || b==2 || b==3 || b==4)) {
//				ButtonUseEvent bue = { act };

				buttonUseDelegate(act);
//				cb_buttonUse(bue);
			}
		}
	};

#endif /* INCLUDE_ButtonActions_ */
