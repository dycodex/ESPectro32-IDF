/*
 * Created by Andri Yadi, Aug 8, 2017
*/

#include <esp_log.h>
#include "sdkconfig.h"

#include <Arduino.h>
#define _GLIBCXX_USE_C99
#include <string>

const static char *TAG ="APP";

extern "C" {
	void app_main(void);
}

#include <ESPectro32_Board.h>
#include <AppSetting.h>

#include "explore/test_alora_makestro.hpp"

void setup() {
	initArduino();
	Serial.begin(115200);

	ESP_LOGI(TAG, "It begins!");

	ESPectro32.begin();
	// testSPIFFS();
	
//	ESPectro32.scanI2C();
//  delay(500);
//	if (!ESPectro32.beginSDCard()) {
//		return;
//	}

	//load examples
	test_alora_makestro();

}

void loop() {

}














void app_main(void)
{
	setup();

	 for(;;) {
	 	loop();
	 }
}

