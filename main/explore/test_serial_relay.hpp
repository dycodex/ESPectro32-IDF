/*
 * Test_SerialRelay.hpp
 *
 *  Created on: Oct 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_SERIALRELAY_HPP_
#define MAIN_EXPLORE_TEST_SERIALRELAY_HPP_

#include <Arduino.h>

#define RX_PIN 17
#define TX_PIN 16

HardwareSerial ATMSerial(1);

void serialRelay(void *p) {

	ATMSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

	Serial.println("Waiting...");

	ATMSerial.println("AT");

	for(;;) {
		while (ATMSerial.available() > 0) {
			Serial.write(ATMSerial.read());
		}
		while (Serial.available() > 0) {
			ATMSerial.write(Serial.read());
		}

		vTaskDelay(1/portTICK_PERIOD_MS);
	}

	Serial.println("DONE...");
	vTaskDelete(NULL);
}

static void test_serial_relay() {
	ATMSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
	xTaskCreatePinnedToCore(serialRelay, "serialRelayTask", 4096, NULL, 1, NULL, 0);
}

#endif /* MAIN_EXPLORE_TEST_SERIALRELAY_HPP_ */
