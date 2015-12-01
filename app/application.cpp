#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <drivers/SSD1306_driver.h>

#include <InfoScreens.h>
#include <utils.h>

#define LED_PIN 2 // GPIO2
#define BTN_PIN 0

#define sclPin 5 //D2
#define sdaPin 4 //D1

//* SSD1306 - I2C
SSD1306_Driver display(4);
//Adafruit_SSD1306 display(4);

Timer procTimer;
bool state = true;

InfoScreens *infos;
String currentTime = "00:00:00";

Timer updater;
Timer updater2;

void blink()
{
	digitalWrite(LED_PIN, state);
	state = !state;
}

int dddd = 0;
bool menuEventLister(paramStruct* data, ViewMode vddd, InfoScreenMenuAction actionType, String newValue) {
	debugf("menuEventLister received viewmode=%i, actiontype=%i, newVal=%s", vddd, actionType, newValue.c_str());

	if (data->id == "ssid" && actionType == InfoScreenMenuAction::InfoNextValue) {
		infos->updateParamValue(data->id, "ilan" + String(dddd++));
		if (dddd>10) {
			dddd =0;
		}
		return true;
	}

	return false;
}

void initInfoScreens() {
	InfoPage* p1 = infos->createPage("Main");
	InfoLine* el = p1->createLine("P1Test");
	//add the time param
	el->addParam("time", currentTime)->t.x = getXOnScreenForString(currentTime, 1);

	InfoLine* el3 = p1->createLine("ap: ");
	el3->addParam("ap", "0.0.0.0");

	InfoLine* el4 = p1->createLine("sta:");
	el4->addParam("station", "Editfff", true, 6);

	paramDataValues* ssidVals = new paramDataValues();
	ssidVals->addValue(new String("s1"));
	ssidVals->addValue(new String("s2"));
	infos->setEditModeValues("station", ssidVals);

	paramStruct* ps1 = p1->createLine("ID: ")->addParam("ssid", "No Vals", true, 6);

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
	p3->createLine("editable")->addParam("aa", "edit-me", true, 6);

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

//	display.print("Testing display");
//	display.display();

	infos = new InfoScreens(&display, BTN_PIN);
	initInfoScreens();
	infos->show();

//	pinMode(LED_PIN, OUTPUT);
//	procTimer.initializeMs(1000, blink).start();

	updater.initializeMs(300, handleUpdateTimer).start();
	updater2.initializeMs(120, handle2UpdateTimer).start();

}
