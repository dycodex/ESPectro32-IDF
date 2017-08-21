/*
 * Created by Andri Yadi, Aug 8, 2017
*/

#include <esp_log.h>
#include "sdkconfig.h"

#include <Arduino.h>
#include <string>
#include <string.h>

const static char *TAG ="APP";

extern "C" {
	void app_main(void);
}

#include <ESPectro32_Board.h>

#include <ESPectro32_RGBLED_Animation.h>
RgbLedColor_t aCol(200, 0, 80);
ESPectro32_RGBLED_GlowingAnimation glowAnim(ESPectro32.RgbLed(), aCol);

#include <AppSetting.h>

#include "examples/wifi_conn_anim.hpp"

void app_main(void)
{
	initArduino();
	Serial.begin(115200);

	ESP_LOGI(TAG, "It begins!");

//	testSPIFFS();

	ESPectro32.begin();

	AppSetting.begin();
	if (AppSetting.load() == ESP_OK) {
		ESP_LOGI(TAG, "AppSetting available!");
		AppSetting.printVals();
	}
	else {
		ESP_LOGE(TAG, "AppSetting init FAILED!");
	}

	//Turn on/off LED once
//	ESPectro32.turnOnLED();
//	delay(1000);
//	ESPectro32.turnOffLED();

	//LED animation
	ESPectro32.LED().setAnimation(ESPectro_LED_Animation_Fading, 3000, 3);
	//ESPectro32.LED().blink(1000, 3);


//	ESPectro32.RgbLed().setPixel(0, 120, 128, 7);
//	ESPectro32.RgbLed().show();
//	delay(1000);
//	ESPectro32.RgbLed().clear();
//	ESPectro32.RgbLed().show();

	//RGB LED Animation, glowing LED for 3 times, 3 seconds each
	glowAnim.start(3000, 3);

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


	//LED Matrix animation (only support 8 frames)
//	ledMatrixAnim.setLedMatrix(ESPectro32.LedMatrix());
//
//	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_ICON_HEART);
//	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_ICON_HEART_OUTLINE);
//	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_ICON_HEART);
//	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_ICON_HEART_OUTLINE);
//	ledMatrixAnim.addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
//		ledM.drawCircle(3, 3, 3, 200);
//	});
//	ledMatrixAnim.addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
//		ledM.fillCircle(3, 3, 3, 200);
//	});
//
//	ledMatrixAnim.start(3000);

	//Button handlers
	ESPectro32.ButtonA().onButtonUp([]() {
		ESP_LOGI(TAG, "Button A up");
	});

	ESPectro32.ButtonA().onDoublePressed([]() {
		ESP_LOGI(TAG, "Button A doubled");
	});

	ESPectro32.ButtonB().onButtonUp([]() {
		ESP_LOGI(TAG, "Button B up");
	});

	//load example
	connect_wifi();


//	for(;;) {
//		int trVal = ESPectro32.readPhotoTransistorValue();
//		ESP_LOGI(TAG, "Photo TR: %d %f, HALL: %d", trVal, ESPectro32.readPhotoTransistorVoltage(), ESPectro32.readOnBoardHallSensor());
//		vTaskDelay(500/portTICK_PERIOD_MS);
//	}
}

