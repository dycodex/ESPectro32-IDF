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

void setup() {
	initArduino();
	Serial.begin(115200);

	Serial.println("It begins!");

	ESPectro32.begin();

	//load examples
	//load_ex_sdcard();
	if(!SD.begin(ESPECTRO32_SDCARD_CSPIN)){
		ESP_LOGE(TAG, "Card Mount Failed");
		return;
	}
	//tryI2SPlay(NULL);

	ESPectro32.ButtonB().onButtonUp([]() {
		ESP_LOGI(TAG, "Button B up");
		parseWavFile();
	});

	//parseWavFile();
}

void loop() {
	delay(1000);
}

void app_main(void)
{
	setup();

	for(;;) {
		loop();
	}
}

