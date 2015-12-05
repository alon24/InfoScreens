#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <drivers/SSD1306_driver.h>

#include <InfoScreens.h>
#include <utils.h>

#define LED_PIN 2 // GPIO2
#define BTN_PIN 0
#define ialn 222
#define sclPin 5
#define sdaPin 4

//* SSD1306 - I2C
SSD1306_Driver display(4);

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
}

void handleUpdateTimer() {
	currentTime = SystemClock.now().toShortTimeString(true);
//	debugf("handleUpdateTimer time=%s", currentTime.c_str() );
	infos->updateParamValue("time", currentTime);
}

void handle2UpdateTimer() {
	infos->updateParamValue("ap", String(millis()));
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200
	Serial.systemDebugOutput(true); // Debug output to serial
	Wire.pins(sclPin, sdaPin);

	display.begin(SSD1306_SWITCHCAPVCC);
	display.clearDisplay();

	display.setTextSize(1);
	display.setTextColor(WHITE);

	infos = new InfoScreens(&display, BTN_PIN);
	initInfoScreens();
	infos->show();

	updater.initializeMs(300, handleUpdateTimer).start();
	updater2.initializeMs(120, handle2UpdateTimer).start();

}
