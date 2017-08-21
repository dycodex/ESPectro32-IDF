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

#include <ex_wifi_conn_anim.hpp>

void app_main(void)
{
	initArduino();
	Serial.begin(115200);

	ESP_LOGI(TAG, "It begins!");

	ESPectro32.begin();

	AppSetting.begin();
	if (AppSetting.load() == ESP_OK) {
		ESP_LOGI(TAG, "AppSetting available!");
		AppSetting.printVals();
	}
	else {
		ESP_LOGE(TAG, "AppSetting init FAILED!");
	}

	//load examples
	load_ex_wifi_conn_anim();
}

