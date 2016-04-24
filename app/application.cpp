//#include <SmingCore/SmingCore.h>
#include <drivers/SSD1306_driver.h>
//#include <drivers/ILI9341_driver.h>
//#include <drivers/PCD8544_driver.h>

#include <InfoScreens.h>
#include <utils.h>

#define LED_PIN 2 // GPIO2
#define BTN_PIN 0

//#define sclPin 5
//#define sdaPin 4

#define sclPin 13
#define sdaPin 14

#define rotaryBtnPin 13
#define rotaryClkPin 14
#define rotaryDtPin 12

//pins for spi
//#define clk 15

//* SSD1306 - I2C
Base_Display_Driver* display;

Timer procTimer;
bool state = true;

InfoScreens *infos;
String currentTime = "00:00:00";

Timer updater;
Timer updater2;

//Sample code to handle callbacks, and specifically target the "ssid" parameter which is dynamic
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

void initInfoScreens() {
	// Add a new Page
	InfoPage* p1 = infos->createPage("Main");

	//Add a line item
	InfoLine* el = p1->createLine("P1Test");

	//add a time parameter (data that can be updated whenever)
	el->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);

	InfoLine* el3 = p1->createLine("ap: ");
	el3->addParam("ap", "0.0.0.0");

	InfoLine* el4 = p1->createLine("sta:");
	el4->addParam("station", "EditXXX", true, 6);

	//add a list of static Values
	paramDataValues* stationVals = new paramDataValues();
	stationVals->addValue(new String("s1"));
	stationVals->addValue(new String("s2"));

	//assign the values to the id=station parameter
	infos->setEditModeValues("station", stationVals);

	//add a new parameter without adding a list of parameter
	paramStruct* ps1 = p1->createLine("ID: ")->addParam("ssid", "No Vals", true, 6);

	//You can even add the same param more than 1 time in a page (short version now)
	p1->createLine("time:")->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);

	InfoPage* p2 = infos->createPage("P2");
	InfoLine* el2 = p2->createLine("P2Test");
	//add the time param
	el2->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);

	p2->createLine("sta:")->addParam("station")->setEditable(true)->setMaxLineSize(6);
	InfoLine* el22 = p2->createLine("apdd: ");
	el22->addParam("ap");

	InfoPage* p3 = infos->createPage("P3");
	p3->createLine("P3Test")->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);
	p3->createLine("ap:")->addParam("ap");
    p3->createLine("param1: ")->addParam("aa", "not edit");

    // set a function to get screen info callback (with the ability to consume events)
    infos->setOnMenuEventDelegate(menuEventLister);

//	// Add a new Page
//		InfoPage* p1 = infos->createPage("Main");
//		//Add a line item
//		p1->createLine("Network")->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);
//		InfoLine *apLine = p1->createLine("ap ");
//		apLine->addParam("apState", "on: ", true, 4);
//		apLine->addParam("apIp", "Not Connected");
//
//		InfoLine* staLine = p1->createLine("ssid: ");
//		staLine->addParam("staState", "off:");
//		InfoLine* lsta = p1->createLine("sta:");
////		lsta->addParam("stationIP1", "");
//		lsta->addParam("stationIP1", "", false, 8);
//
//		InfoPage* p2 = infos->createPage("P2");
//		InfoLine* el2 = p2->createLine("P2Test");
//		//add the time param
//		el2->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);
//
//		p2->createLine("sta:")->addParam("station")->setEditable(true)->setMaxLineSize(6);
//		InfoLine* el22 = p2->createLine("apdd: ");
//		el22->addParam("ap");
}

void handleUpdateTimer() {
	currentTime = SystemClock.now().toShortTimeString(true);
//	debugf("handleUpdateTimer time=%s", currentTime.c_str() );
	infos->updateParamValue("time", currentTime);
}

void handle2UpdateTimer() {
	infos->updateParamValue("ap", String(millis()));
}
Timer DemoTimer;

void Demo2()
{
	Serial.println("Display: some text");
	display->clearDisplay();
	// text display tests
	display->setTextSize(1);
	display->setTextColor(WHITE);
	display->setCursor(0,0);
	display->print("Sming Framework");
//	display->setTextColor(BLACK, WHITE); // 'inverted' text
	display->setCursor(104, 7);
	display->print("v1.0");
	//----
	display->setTextColor(WHITE);
	display->print("Let's do smart things");
	display->setTextSize(3);
	display->print("IoT");
	display->display();
	DemoTimer.stop();      // Finish demo
}
Timer btnTimer;

void doCheckBtn() {
	if (digitalRead(BTN_PIN) == HIGH) {
		debugf("btn High");
	} else {
		debugf("btn LOW");
	}
}

void init()
{
	Serial.begin(74880); // 115200
	Serial.systemDebugOutput(true); // Debug output to serial
	Wire.pins(sclPin, sdaPin);
	display = new SSD1306_Driver(16);
//	display = new ILI9341_Driver();
//	display = new PCD8544_Driver(13,14,15,4,5);

	display->init();

//	infos = new InfoScreens(&displayA, BTN_PIN);
	infos = new InfoScreens(display);
	initInfoScreens();
	infos->initMFButton(BTN_PIN);
//	pinMode(BTN_PIN, INPUT);
//	btnTimer.initializeMs(80, doCheckBtn).start(true);

//	infos->initRotary(rotaryBtnPin, rotaryClkPin, rotaryDtPin );

	infos->show();
	WifiStation.enable(false);
//	WifiAccessPoint.enable(false);

//	DemoTimer.initializeMs(2000, Demo2).start();
//
//	updater.initializeMs(300, handleUpdateTimer).start();
//	updater2.initializeMs(120, handle2UpdateTimer).start();

}
