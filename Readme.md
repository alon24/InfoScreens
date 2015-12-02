# ESP8266 InfoScreens - With Menu
------

library code for esp8266, for showing multiple info screens with data on them, that refreshes automatically for you onscreen (currently only ssd1306 is supported), and also a menu system navigated with one button.

#### Based on sming: https://github.com/SmingHub/Sming

### Sample Pins used:

Screen (I2C), SCL=5, SDA=4: </br>
MultiFunctionButton GPIO 0 //Built into Nodemcu devkit

### Usage:

```
Create extended display
//* SSD1306 - I2C
Extended_SSD1306 display(4);

//Declare InfoScreens
InfoScreens *infos;
...

infos = new InfoScreens(&display, BTN_PIN);
```

#### Adding Pages and other parts
```
  // Add a new Page
  InfoPage* p1 = infos->createPage("Main");

  //Add a line item
  InfoLine* el = p1->createLine("P1Test");
  //add a time parameter (data that can be updated whenever)
  el->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);


```

#### You can even add the same param more than 1 time in a page (short version now)
```
  p1->createLine("time:")->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);
```

#### Add more Pages
```
  InfoPage* p2 = infos->createPage("P2");
	InfoLine* el2 = p2->createLine("P2Test");
	//add the time param (again, different page)
	el2->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);

  InfoPage* p3 = infos->createPage("P3");
	p3->createLine("P3Test")->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);
	p3->createLine("ap:")->addParam("ap");
  p3->createLine("param1: ")->addParam("aa", "not edit");

```
---
#### This is not just an info page, its also a menu, Add an editable parameter

```
InfoLine* el4 = p1->createLine("sta:");
el4->addParam("station", "EditXXX", true, 6);

//add a list of static Values
paramDataValues* stationVals = new paramDataValues();
stationVals->addValue(new String("s1"));
stationVals->addValue(new String("s2"));

//assign the values to the id=station parameter
infos->setEditModeValues("station", stationVals);

//add a new parameter without adding a list of parameters
paramStruct* ps1 = p1->createLine("ID: ")->addParam("ssid", "No Vals", true, 6);

p2->createLine("sta:")->addParam("station")->setEditable(true)->setMaxLineSize(6);
InfoLine* el22 = p2->createLine("apdd: ");
el22->addParam("ap");

```
### Handling of parameter values in a dynamic way
set a function to get screen info callback (with the ability to consume events)
```
infos->setOnMenuEventDelegate(menuEventLister);

```
Sample code to handle callbacks, and specifically target the "ssid" parameter which is dynamic
```
int counter = 0;
bool menuEventLister(paramStruct* data, ViewMode vmode, InfoScreenMenuAction actionType, String newValue) {
	debugf("menuEventLister received on id=%s, viewmode=%i, actiontype=%i, newVal=%s", data->id.c_str(), vmode, actionType, newValue.c_str());

	if (data->id == "ssid" && actionType == InfoScreenMenuAction::InfoNextValue) {
		infos->updateParamValue(data->id, "test" + String(counter++));
		if (counter>10) {
			counter =0;
		}
		return true;
	}

	return false;
}

```
Show the InfoScreen

```
infos->show();
```
Now update a value:
```
  void handleUpdateTimer() {
  	currentTime = SystemClock.now().toShortTimeString(true);
  	infos->updateParamValue("time", currentTime);
  }

  updater.initializeMs(300, handleUpdateTimer).start();

```
Hey, why not update another value (different interval)
```
void handle2UpdateTimer() {
	infos->updateParamValue("ap", String(millis()));
}

updater2.initializeMs(120, handle2UpdateTimer).start();

```

#### Move right (you can also move left) - show a different page
```
infos->moveRight();
infos->moveLeft();
```

### Button action handling in InfoScreen (different modes)

#### In info mode
Click - move to next Screen </br>
DoubleClick- move to previous Screen</br>
Click and hold - move to global edit mode </br>

#### In global edit mode *
Click - move to next Screen </br>
DoubleClick - move to previous Screen </br>
Click and hold - move to field edit mode </br>
\*A small **E** will blink
#### In field edit mode *
Click - move to next field value </br>
Click and hold - Rapid fire (100ms) of click event (move to next value) field value </br>
DoubleClick - return to global edit mode (not just this field) </br>
\*A small **F** will blink
</br>
### MultiFunctionButton
This button implementation can sense:

1. Click
2. Double Click

If **enableClickAndHold = true** then if button is still pressed after click, it will send a
click event every 100ms

if **enableClickAndHold = false** then lib will detect: </br>
3. Long click - no Click events fired while key is kept down </br>
4. Hold click - (longer interval) - no Click events fired while key is kept down

### Usage:
#### Simplified Constrcutor</br>
MultiFunctionButton() </br>

#### Regular Constructor
MultiFunctionButton(int buttonPin, ButtonActionDelegate handler = null, bool pressAndHold = true)</br>

#### Important functions

// init to set button, handler is optional here </br>
void initBtn(int buttonPin, ButtonActionDelegate handler = null, bool pressAndHold = true)

//set how to handle long press  </br>
void enablePressAndHold(bool pressAndHold)

//Delegated call when event is triggered </br>
void setOnButtonEvent(ButtonActionDelegate handler)

[![IMAGE ALT TEXT HERE](http://img.youtube.com/vi/Kv1fsxQzNcM/0.jpg)](https://www.youtube.com/watch?v=Kv1fsxQzNcMo)
