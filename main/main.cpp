/*
 * Created by Andri Yadi, Aug 8, 2017
*/

#include <esp_log.h>
#include "sdkconfig.h"

#include <Arduino.h>
#define _GLIBCXX_USE_C99
#include <string>

#include <ESPectro32_Board.h>
#include <AppSetting.h>

const static char *TAG ="APP";

extern "C" {
	void app_main(void);
}

//#include "explore/test_alora_makestro_oled.hpp"
//#include "explore/test_alora.hpp"
//#include "explore/test_alora_gps.hpp"
//#include "explore/test_alora_makestro.hpp"
//#include "testcode/test_eic_oled_makestro.hpp"

//#include "testcode/test_eic.hpp"
//#include "../components/ESPectro32/examples-idf/ex_ledmatrix_anim.hpp"
//#include "../components/ESPectro32/examples-idf/ex_wifi_conn_anim.hpp"
//#include "explore/test_makestro_cloud.hpp"

//#include "explore/test_oled.hpp"
//#include "explore/test_imu_lsm6_u8g2.hpp"
//#include "explore/test_imu_adxl_u8g2.hpp"
//#include "explore/test_serial_relay.hpp"
//#include "explore/test_ble_matrix.hpp"
//#include "explore/test_i2s_file.h"
//#include "explore/test_http_download.hpp"
//#include "explore/test_play_wav_file.hpp"
//#include "explore/test_alora_iotcentral.hpp"
#include "explore/test_azure_iot.hpp"

void setup() {
	initArduino();
	Serial.begin(115200);

	ESP_LOGI(TAG, "It begins!");

	ESPectro32.begin();

//	ESPectro32.scanI2C();
//  	delay(1000);

	// SD-Card Example
//	ESPectro32.SDCardBegin();
//	ESPectro32.SDCardPrintInfo();
//	ESPectro32.SDCardListDirectory();

	// Load examples
//	test_play_wav_file_audio_player();
//	test_http_download();
//	test_alora_makestro_oled();
//	test_alora();
//  	test_alora_gps();
//	test_alora_makestro();
//	test_alora_iotcentral();
	test_azure_iot();
//  	test_eic_oled_makestro();
//	test_imu_lsm6();
//	test_imu_adxl_u8g2();
//	test_ble_matrix();
}


void loop() {
	delay(1);
}














void app_main(void)
{
	setup();

//	for(;;) {
//		loop();
//	}
}

