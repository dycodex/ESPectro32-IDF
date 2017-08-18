/*
 * ESPectro32.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: andri
 */

#include "ESPectro32_Board.h"

ESPectro32_Board::ESPectro32_Board() {
	// TODO Auto-generated constructor stub

}

ESPectro32_Board::~ESPectro32_Board() {
	if (rgbLed_ != NULL) {
		delete rgbLed_;
		rgbLed_ = NULL;
	}

	if (led_ != NULL) {
		delete led_;
		led_ = NULL;
	}

	if (buttonA_ != NULL) {
		delete buttonA_;
		buttonA_ = NULL;
	}

	if (buttonB_ != NULL) {
		delete buttonB_;
		buttonB_ = NULL;
	}

	if (ledMatrix_ != NULL) {
		delete ledMatrix_;
		ledMatrix_ = NULL;
	}
}

ESPectro32_Board ESPectro32;

bool ESPectro32_Board::begin() {
	return true;
}

ESPectro32_RGBLED& ESPectro32_Board::RgbLed() {
	if (rgbLed_ == NULL) {
		rgbLed_ = new ESPectro32_RGBLED();
	}

	return *rgbLed_;
}

void ESPectro32_Board::turnOnLED() {
	LED().turnOn();
}

void ESPectro32_Board::turnOffLED() {
	LED().turnOff();
}

void ESPectro32_Board::blinkLED(uint32_t interval, uint32_t count) {
	LED().blink(interval, count);
}

void ESPectro32_Board::fadeLED(uint32_t duration, uint32_t count) {
	LED().fade(duration);
}

void ESPectro32_Board::stopLEDAnimation() {
	LED().stopAnimation();
}

ESPectro32_LED& ESPectro32_Board::LED() {
	if (led_ == NULL) {
		led_ = new ESPectro32_LED();
		led_->begin();
	}

	return *led_;
}

void ESPectro32_Board::toggleLED() {
	LED().toggle();
}

ESPectro32_Button& ESPectro32_Board::ButtonA() {
	if (buttonA_ == NULL) {
		buttonA_ = new ESPectro32_Button(ESPECTRO32_BUTTON_A_PIN);
		buttonA_->begin();
	}

	return *buttonA_;
}

ESPectro32_Button& ESPectro32_Board::ButtonB() {
	if (buttonB_ == NULL) {
		buttonB_ = new ESPectro32_Button(ESPECTRO32_BUTTON_B_PIN);
		buttonB_->begin();
	}

	return *buttonB_;
}

ESPectro32_LedMatrix& ESPectro32_Board::LedMatrix() {
	if (ledMatrix_ == NULL) {
		ledMatrix_ = new ESPectro32_LedMatrix();

		if (!ledMatrix_->begin()) {
			ESPECTRO32_ERROR_PRINT("Led matrix not found");

//			delete ledMatrix_;
//			ledMatrix_ = NULL;
//
//			return NULL;
		}
	}

	return *ledMatrix_;
}

void ESPectro32_Board::scanI2C() {
	ESPECTRO32_INFO_PRINT("I2C scanning with SDA=%d, CLK=%d", SDAPIN, SCLPIN);
	Wire.begin(SDAPIN, SCLPIN);
	int address;
	int foundCount = 0;
	for (address=1; address<127; address++) {
		Wire.beginTransmission(address);
		uint8_t error = Wire.endTransmission();
		if (error == 0) {
			foundCount++;
			ESPECTRO32_INFO_PRINT("Found device at 0x%.2x", address);
		}
	}
	ESPECTRO32_INFO_PRINT("Found %d I2C devices by scanning.", foundCount);
}
