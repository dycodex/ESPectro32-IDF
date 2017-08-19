/*
 * Created by Andri Yadi, Aug 8, 2017
*/

#include <esp_log.h>
#include <string>
#include "sdkconfig.h"
#include "driver/gpio.h"

#include <Arduino.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

const static char *TAG ="APP";

extern "C" {
	void app_main(void);
}

#include "Constants.h"

//#include "WiFiManager.h"
//WiFiManager wifiMgr;

// #include <FabrickClient.h>

// HardwareSerial loraSerial(1);
// static void testLoRa() {
// 	loraSerial.begin(9600, SERIAL_8N1, 33, 23);
// 	FabrickLoraClient.begin(&loraSerial);

// 	//String temp_data_s = String(2350, HEX);
// 	float temp_f = 23.45;

// 	int temp_int       = int(temp_f * 10);                                 // Scale and convert to int.
// 	long temp_2c       = (temp_int >= 0) ? temp_int : (temp_int + 65536);  // Convert to 2's complement
// 	String temp_data_s = String(temp_2c, HEX);                             // Convert to hex.
// 	temp_data_s        = FabrickLoraClientClass::addZeros(temp_data_s, 4);                     // Ensure consistent length

// 	//FabrickLoraClient.send("81ef46e9", 1, 0, 3303, temp_data_s, 4);
// 	FabrickLoraClient.send("c1a12d25", 1, 0, 3303, temp_data_s, 4);
// }

#include <ESPectro32_Board.h>

//#include <ESPectro32_Button.h>
//ESPectro32_Button buttonA(ESPECTRO32_BUTTON_A_PIN);
//ESPectro32_Button buttonB(ESPECTRO32_BUTTON_B_PIN);

//#include "explore/TestI2S.h"
#include "explore/TestI2S2-SPIFFS.h"
#include "explore/TestMatrix.h"

#include <ESPectro32_RGBLED_Animation.h>

RgbLedColor_t aCol(200, 0, 80);
//ESPectro32_RGBLED_Animation fadeAnim(ESPectro32.RgbLed(), aCol);
ESPectro32_RGBLED_FadeInOutAnimation fadeAnim(ESPectro32.RgbLed(), aCol);

void app_main(void)
{
	initArduino();
	Serial.begin(115200);

	ESP_LOGI(TAG, "It begins!");

//	testSPIFFS();
//
////	if (!mount_fs()) {
////		return;
////	}
////
////	//tryI2SInput(NULL);
////	tryI2SRecord(NULL);
//
//	tryI2SPlay(NULL);
////	xTaskCreate(&tryI2SPlay, "tryI2SPlay", 2048*10, NULL, configMAX_PRIORITIES - 2, NULL);
//	delay(100);


//	testLedMatrix();
//	xTaskCreate(&testLedMatrix, "testLedMatrix", 2048*2, NULL, configMAX_PRIORITIES - 2, NULL);
//	return;

	ESPectro32.begin();
//	ESPectro32.turnOnLed();
//	delay(1000);
//	ESPectro32.turnOffLed();

	ESPectro32.LED().setAnimation(ESPectro_LED_Animation_Fading, 3000, 3);
	//ESPectro32.LED().blink(1000, 3);

//	ESPectro32.RgbLed().setPixel(0, 120, 128, 7);
//	ESPectro32.RgbLed().show();
//	delay(1000);
//	ESPectro32.RgbLed().clear();
//	ESPectro32.RgbLed().show();


	fadeAnim.start(2000, 3);

	/*
	fadeAnim.start([](const WS2812Animator::AnimationParam param) {

		//Triangle function
		//y = (A/P) * (P - abs(x % (2*P) - P))
		float x = param.progress * 100;
		float P = 100/2;
		float b = (100/P) * (P - abs(((int)x % (int)(2*P)) - P));

		//float b = sin(param.progress * PI) * 100;
//		ESP_LOGI(TAG, "Progress %f", b);

		for (uint8_t pixNum = 0; pixNum < fadeAnim.RgbLed().getPixelCount(); pixNum++) {
			fadeAnim.RgbLed().setBrightnessPercent(pixNum, b);
		}

		fadeAnim.RgbLed().show();

	}, []() {

		ESP_LOGI(TAG, "Animation DONE");

	}, 1000, 10);*/


	ESPectro32.ButtonA().onButtonUp([]() {
		ESP_LOGI(TAG, "Button A up");
	});

	ESPectro32.ButtonA().onDoublePressed([]() {
		ESP_LOGI(TAG, "Button A doubled");
	});

	ESPectro32.ButtonB().onButtonUp([]() {
		ESP_LOGI(TAG, "Button B up");
	});

	ESPectro32.LedMatrix().drawBitmap(0, 0, LED_MATRIX_ICON_HEART, 7, 7, 200);

	for(;;) {
		int trVal = ESPectro32.readPhotoTransistorValue();
		ESP_LOGI(TAG, "Photo TR: %d %f", trVal, ESPectro32.readPhotoTransistorVoltage());
		vTaskDelay(400/portTICK_PERIOD_MS);
	}
}

