/*
 * Test_SerialRelay.hpp
 *
 *  Created on: Oct 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_SERIALRELAY_HPP_
#define MAIN_EXPLORE_TEST_SERIALRELAY_HPP_

#include <Arduino.h>

#define RX_PIN 25//17
#define TX_PIN 26//16

HardwareSerial theSerial(1);

void serialRelay(void *p) {

	theSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

//	Serial.println("Waiting...");

//	theSerial.println("at+version");

	for(;;) {
		while (theSerial.available() > 0) {
			Serial.write(theSerial.read());
		}
		while (Serial.available() > 0) {
			theSerial.write(Serial.read());
		}

		vTaskDelay(1/portTICK_PERIOD_MS);
	}

	Serial.println("DONE...");
	vTaskDelete(NULL);
}

static void test_serial_relay() {
	//theSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
	xTaskCreatePinnedToCore(serialRelay, "serialRelayTask", 4096, NULL, 1, NULL, 0);
}

#endif /* MAIN_EXPLORE_TEST_SERIALRELAY_HPP_ */
