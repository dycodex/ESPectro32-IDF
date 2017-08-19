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

int ESPectro32_Board::readAnalog(adc1_channel_t channel, adc_bits_width_t bitWidth, adc_atten_t atten) {

	adc1_config_width(bitWidth);
	adc1_config_channel_atten(channel, atten);
	int val = adc1_get_voltage(channel);

	return val;
}

float ESPectro32_Board::readAnalogVoltage(adc1_channel_t channel, adc_bits_width_t bitWidth, adc_atten_t atten) {

	int val = readAnalog(channel, bitWidth, atten);

	int bitWidthVal = 0, fullScaleMV = 0;

	switch(bitWidth) {
	case ADC_WIDTH_9Bit:
		bitWidthVal = 511; break;
	case ADC_WIDTH_10Bit:
		bitWidthVal = 1023; break;
	case ADC_WIDTH_11Bit:
		bitWidthVal = 2047; break;
	case ADC_WIDTH_12Bit:
		bitWidthVal = 4095; break;
	default:
		bitWidthVal = 4095; break;
	}

	switch(atten) {
	case ADC_ATTEN_0db:
		fullScaleMV = 1100; break;
	case ADC_ATTEN_2_5db:
		fullScaleMV = 1500; break;
	case ADC_ATTEN_6db:
		fullScaleMV = 2200; break;
	case ADC_ATTEN_11db:
		fullScaleMV = 3300; break;
	default:
		fullScaleMV = 2200; break;
	}

	float voltage = val * ((fullScaleMV * 1.0f/1000) / (bitWidthVal * 1.0f));
	return voltage;
}

int ESPectro32_Board::readPhotoTransistorValue(adc_bits_width_t bitWidth, adc_atten_t atten) {
	return readAnalog(ESPECTRO32_PHOTO_TR_ANALOG_CHANNEL, bitWidth, atten);
}

float ESPectro32_Board::readPhotoTransistorVoltage(adc_bits_width_t bitWidth, adc_atten_t atten) {
	return readAnalogVoltage(ESPECTRO32_PHOTO_TR_ANALOG_CHANNEL, bitWidth, atten);
}

int ESPectro32_Board::readOnBoardHallSensor() {
	adc1_config_width(ADC_WIDTH_12Bit);
	int val = hall_sensor_read();
	return val;
}

void ESPectro32_Board::scanI2C() {
	ESPECTRO32_INFO_PRINT("I2C scanning with SDA=%d, CLK=%d", ESPECTRO32_SDAPIN, ESPECTRO32_SCLPIN);
	Wire.begin(ESPECTRO32_SDAPIN, ESPECTRO32_SCLPIN);
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
