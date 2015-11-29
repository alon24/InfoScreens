# ESP8266 InfoScreens

## In progress
This will be a sming library for esp8266, for showing multiple info screens with data on them, that refreshes automaticaly for you onscreen (currently ssd1306)

#### Based on sming: https://github.com/SmingHub/Sming

### Pins:

Screen (I2C): </br>
sclPin 5</br>
sdaPin 6

Button 0

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

  //Add a header line (its just a regular line)
  InfoLine* el = p1->createLine("P1Test");
  //add a time paramater (data that can be updated whenever)
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
	//add the time param
	el2->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);

	p2->createLine("sta:")->addParam("station", "0.0.0.0");
	InfoLine* el22 = p2->createLine("2", "apdd: ");
	el22->addParam("ap", "0.0.0.0");

	InfoPage* p3 = infos->createPage("P3");
	p3->createLine("P3Test")->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);
	p3->createLine("ap:")->addParam("ap", "0.0.0.0");

```

#### This is not just an info page, it is also a menu, Add an editable paramater

```
paramStruct* ps1 = p1->createLine("ID: ")->addParam("ssid", "Edit Me", true, 6);
```

### Show the InfoScreen
```
infos->show();
```

#### Now update a value:
```
  void handleUpdateTimer() {
  	currentTime = SystemClock.now().toShortTimeString(true);
  	infos->updateParamValue("time", currentTime);
  }
```

#### Move right (you can also move left) - show a different page
```
infos->moveRight();
infos->moveLeft();
```

### Buttons action handling in InfoScreen (different modes)

#### In info mode
**Click** - move to next Screen </br>
**DoubleClick** - move to previous Screen
#### In edit mode
**Click** - move to next Screen </br>
**DoubleClick** - move to previous Screen
\*A small E will blink
#### In field edit mode
**Click** - move to next field value </br>
**DoubleClick** - return to global edit mode (not just this field)
**Click and hold** - Rapid fore (100ms) of move to next field value
\*A small F will blink
</br>
### MultiFunctionButton
This button implementation can sense:

1. **Click**
2. **Double Click**

If **enablePressAndHold = true** then if button is still pressed after click, it will send a
click event every 100ms

if **enablePressAndHold = false** then lib will detect:
3. **Long click** - no Click events fired while key is kept down
4. **Hold click** - no Click events fired while key is kept down

### Usage:
#### Simplified Constrcutor</br>
MultiFunctionButton() </br>

#### Regular Constructor
MultiFunctionButton(int buttonPin, ButtonActionDelegate handler = null, bool pressAndHold = true)</br>

#### Important functions

// init to set button, handler is optional here
void initBtn(int buttonPin, ButtonActionDelegate handler = null, bool pressAndHold = true)

//set how to handle long press
void enablePressAndHold(bool pressAndHold)


//Delegated call when event is triggered
void setOnButtonEvent(ButtonActionDelegate handler)

[![IMAGE ALT TEXT HERE](http://img.youtube.com/vi/Kv1fsxQzNcM/0.jpg)](https://www.youtube.com/watch?v=Kv1fsxQzNcMo)
