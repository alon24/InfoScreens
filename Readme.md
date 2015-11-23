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
```

#### Adding Pages and other parts
```
  // Add a new Page
  InfoPage* p1 = infos->createPage("Main", "Main");

  //Add a header line (its just a regular line)
  InfoLine* el = p1->createLine("header1", "P1Test");
  //add a time paramater (data that can be updated whenever)
  el->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);
```

#### You can even add the same param more than 1 time in a page (short version now)
```
  p1->createLine("time2", "time:")->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);
```

#### Add more Pages
```
  InfoPage* p2 = infos->createPage("P2", "P2");
	InfoLine* el2 = p2->createLine("header2", "P2Test");
	//add the time param
	el2->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);

	p2->createLine("4", "sta:")->addParam("station", "0.0.0.0");
	InfoLine* el22 = p2->createLine("2", "apdd: ");
	el22->addParam("ap", "0.0.0.0");

	InfoPage* p3 = infos->createPage("P3", "P3");
	p3->createLine("header3", "P3Test")->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);
	p3->createLine("4", "ap:")->addParam("ap", "0.0.0.0");

```

### Show the menu
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

### Use of InfoScreen
Click - move to next Screen
DoubleClick - move to previous Screen
Click and hold - Rapid fore (100ms) of move to next screen

#### MultiFunctionButton
This button implementation can sense:

1. **Click**
2. **Double Click**

If **enablePressAndHold = true** then if button is still pressed after click, it will send a
click event every 100ms

if **enablePressAndHold = false** then lib will detect:
3. **Long click** - no Click events fired
4. **Hold click** - no Click events fired

### Usage:
#### Regular Constrcutor
MultiFunctionButton(int buttonPin, ButtonActionDelegate handler) </br>

#### Simplified Constrcutor</br>
MultiFunctionButton(int buttonPin) </br>

#### Set a new handler for click events
void onButtonEvent(ButtonActionDelegate handler) </br>

[![IMAGE ALT TEXT HERE](http://img.youtube.com/vi/Kv1fsxQzNcM/0.jpg)](https://www.youtube.com/watch?v=Kv1fsxQzNcMo)
