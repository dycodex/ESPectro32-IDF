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


#include <ex_ledmatrix_anim.hpp>

void setup() {
	initArduino();
	Serial.begin(115200);

	ESP_LOGI(TAG, "It begins!");

	ESPectro32.begin();

	//load examples

	load_ex_ledmatrix_anim();
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

