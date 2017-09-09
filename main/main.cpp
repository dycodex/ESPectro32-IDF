/*
 * Created by Andri Yadi, Aug 8, 2017
*/

#include <esp_log.h>
#include "sdkconfig.h"

#include <Arduino.h>

const static char *TAG ="APP";

extern "C" {
	void app_main(void);
}

#include <ESPectro32_Board.h>
#include <AppSetting.h>

//#include <ex_sdcard.hpp>
//#include "explore/TestI2S2-File.h"
#include "explore/TestWavFile.hpp"
//#include "explore/TestBleUart.hpp"
#include "explore/TestSPIFFS.h"

const float OFFSET = 512;
const float SCALE = 512;

#include "NotificationService.h"

NotificationService notif;

void setup() {
	initArduino();
	Serial.begin(115200);

	Serial.println("It begins!");

	ESPectro32.begin();

	//load examples

//	if(!SD.begin(ESPECTRO32_SDCARD_CSPIN)){
//		ESP_LOGE(TAG, "Card Mount Failed");
//		return;
//	}

//	ESPectro32.ButtonB().onButtonUp([]() {
//		ESP_LOGI(TAG, "Button B up");
//		parseWavFile();
//	});

//	test_ble_uart();

//	playWavFile();

//	testSPIFFS();

	notif.begin();
	notif.notifyLocked();
}

void loop() {
//	if (deviceConnected) {
//		Serial.printf("*** Sent Value: %d ***\n", txValue);
//		pCharacteristic->setValue(&txValue, 1);
//		pCharacteristic->notify();
//		txValue++;
//	}
//	delay(1000);

//	float in, out;
//	for (in = 0; in < 2*PI; in = in + 0.001) {
//		out = sin(in) * SCALE + OFFSET;
//		//ESP_LOGI(TAG, "x = %f, y = %f", in, out);
//		ESPectro32.analogWrite(15, out);
//
//		//just for feeding the dog
//		delay(1);
//	}
}

void app_main(void)
{
	setup();

	for(;;) {
		loop();
	}
}

