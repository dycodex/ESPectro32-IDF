/*
 * test_ip5209.hpp
 *
 *  Created on: Sep 1, 2018
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_IP5209_HPP_
#define MAIN_EXPLORE_TEST_IP5209_HPP_

#include <Arduino.h>
#include <Wire.h>

#define IP5209_ADDR 						0x75
#define IP5306_REG_READ1      			0x71
#define IP5209_REG_BATIADC_DAT0      	0x73
#define CHARGE_FULL_BIT       			3

float getCurrent() {
	Wire.beginTransmission(IP5209_ADDR);
	Wire.write(0xA4);
	Wire.endTransmission(false);
	Wire.requestFrom(IP5209_ADDR, 2);
	uint8_t data1 = Wire.read();

	Serial.printf("Data: %d\n", data1);
//	delay(100);
//	Wire.beginTransmission(IP5209_ADDR);
//	Wire.write(0xA5);
//	Wire.endTransmission(false);
//	Wire.requestFrom(IP5209_ADDR, 1);
	uint8_t data2 = Wire.read();
	Serial.printf("Data 2: %d\n", data2);

	uint16_t data = ((data2 << 8) | data1);
//	if (data > 2000) {
//		data |=c
//	}

	float ia = data * 0.745985;
	Serial.printf("I: %f\n", ia);

	return ia;
}

float getVoltage() {
	Wire.beginTransmission(IP5209_ADDR);
	Wire.write(0xA2);
	Wire.endTransmission(false);
	Wire.requestFrom(IP5209_ADDR, 2);
	uint8_t data3 = Wire.read();
	uint8_t data4 = Wire.read();

	Serial.printf("Data 3: %d\n", data3);
//	delay(100);
//	Wire.beginTransmission(IP5209_ADDR);
//	Wire.write(0xA3);
//	Wire.endTransmission(false);
//	Wire.requestFrom(IP5209_ADDR, 1);
//	uint8_t data4 = Wire.read();

	Serial.printf("Data 4: %d\n", data4);

	float v = ((data4  << 8) | data3) * 0.26855 * 0.001 + 2.6;
	Serial.printf("V: %f\n", v);
	return v;
}

#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

static void displaySensorValue() {
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0,0);
	display.println("Volt:");
//	display.setCursor(128/2, 0);
//	display.println("Curr:");

	display.setCursor(0, display.getCursorY() + 2);
	display.setTextSize(2);
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.println(getVoltage());
//	display.setCursor(128/2, display.getCursorY());
//	display.setTextColor(BLACK, WHITE); // 'inverted' text
//	display.println(getCurrent());

	display.setCursor(0, display.getCursorY() + 2);
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.println("Curr:");
//	display.setCursor(128/2, display.getCursorY());
//	display.println("CO2:");

	display.setCursor(0, display.getCursorY() + 2);
	display.setTextSize(2);
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.print(getCurrent());
//	display.setCursor(128/2, display.getCursorY());
//	display.setTextColor(BLACK, WHITE); // 'inverted' text
//	display.println(vals.co2);

	display.display();
}

static void test_ip5209() {
	delay(2000);

	Wire.begin();

	/*
	Wire.beginTransmission(IP5209_ADDR);
	Wire.write(0xA4);
	Wire.endTransmission(false);
	Wire.requestFrom(IP5209_ADDR, 2);
	uint8_t data = Wire.read();

	Serial.printf("Data: %d\n", data);
//	delay(100);
//	Wire.beginTransmission(IP5209_ADDR);
//	Wire.write(0xA5);
//	Wire.endTransmission(false);
//	Wire.requestFrom(IP5209_ADDR, 1);
	uint8_t data2 = Wire.read();
	Serial.printf("Data 2: %d\n", data2);
	Serial.printf("I: %f\n", ((data2  << 8) | data) * 0.745985);

	delay(100);

	Wire.beginTransmission(IP5209_ADDR);
	Wire.write(0xA2);
	Wire.endTransmission(false);
	Wire.requestFrom(IP5209_ADDR, 2);
	uint8_t data3 = Wire.read();
	uint8_t data4 = Wire.read();

	Serial.printf("Data 3: %d\n", data3);
//	delay(100);
//	Wire.beginTransmission(IP5209_ADDR);
//	Wire.write(0xA3);
//	Wire.endTransmission(false);
//	Wire.requestFrom(IP5209_ADDR, 1);
//	uint8_t data4 = Wire.read();

	Serial.printf("Data 4: %d\n", data4);
	Serial.printf("V: %f\n", ((data4  << 8) | data3) * 0.26855 * 0.001 + 2.6);

	delay(100);

	Wire.beginTransmission(IP5209_ADDR);
	Wire.write(0xA2);
	Wire.endTransmission(false);
	Wire.requestFrom(IP5209_ADDR, 2);
	uint8_t data5 = Wire.read();
	uint8_t data6 = Wire.read();

	Serial.printf("Data 5: %d\n", data5);
//	delay(100);
//	Wire.beginTransmission(IP5209_ADDR);
//	Wire.write(0xA3);
//	Wire.endTransmission(false);
//	Wire.requestFrom(IP5209_ADDR, 1);
//	uint8_t data6 = Wire.read();

	Serial.printf("Data 6: %d\n", data6);
	Serial.printf("V: %f\n", ((data6  << 8) | data5) * 0.26855 * 0.001 + 2.6);
	*/

	display.begin(SH1106_SWITCHCAPVCC, 0x3C);
	display.display();
	delay(1000);
	display.clearDisplay();

	// display.setTextSize(1);
	display.setCursor(0, 0);
	// display.setTextColor(WHITE);
	// display.println("Connecting...");
	// display.display();

	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.setTextSize(2);
	display.println("Preparing");
	display.display();

	displaySensorValue();

	unsigned long lastUpdated = 0;
	for(;;) {
		if (millis() - lastUpdated > 2000) {
			displaySensorValue();
			lastUpdated = millis();
		}
	}
}

#endif /* MAIN_EXPLORE_TEST_IP5209_HPP_ */
