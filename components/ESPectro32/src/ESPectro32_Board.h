/*
 * ESPectro32.h
 *
 *  Created on: Aug 8, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BOARD_H_
#define COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BOARD_H_

#include <esp_log.h>
#include <WS2812.h>
#include "ESPectro32_Constants.h"
#include "ESPectro32_LED.h"
#include "ESPectro32_RGBLED.h"
#include "ESPectro32_Button.h"
#include "ESPectro32_LedMatrix.h"
#include <Arduino.h>
#include <Wire.h>

#define ESPECTRO32_DEBUG_PRINT(...)  ESP_LOGD("ESPECTRO32", __VA_ARGS__);
#define ESPECTRO32_INFO_PRINT(...)   ESP_LOGI("ESPECTRO32", __VA_ARGS__);
#define ESPECTRO32_ERROR_PRINT(...)  ESP_LOGE("ESPECTRO32", __VA_ARGS__);

#define SDAPIN (GPIO_NUM_21)
#define SCLPIN (GPIO_NUM_22)

class ESPectro32_Board {
public:
	ESPectro32_Board();
	virtual ~ESPectro32_Board();

	bool begin();
	ESPectro32_RGBLED& RgbLed();

	//LED convenient methods
	ESPectro32_LED &LED();
	void turnOnLED();
	void turnOffLED();
	void blinkLED(uint32_t interval = 500, uint32_t count = UINT16_MAX);
	void fadeLED(uint32_t duration = 1000, uint32_t count = UINT16_MAX);
	void stopLEDAnimation();
	void toggleLED();

	//Buttons
	ESPectro32_Button &ButtonA();
	ESPectro32_Button &ButtonB();

	//LED Matrix
	ESPectro32_LedMatrix &LedMatrix();

	void scanI2C();

private:
	ESPectro32_RGBLED *rgbLed_ = NULL;
	ESPectro32_LED *led_ = NULL;
	ESPectro32_Button *buttonA_ = NULL, *buttonB_ = NULL;
	ESPectro32_LedMatrix *ledMatrix_ = NULL;
};

extern ESPectro32_Board ESPectro32;

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BOARD_H_ */
