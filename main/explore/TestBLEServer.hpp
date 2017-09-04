/*
 * TestBLEServer.hpp
 *
 *  Created on: Aug 24, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTBLESERVER_HPP_
#define MAIN_EXPLORE_TESTBLESERVER_HPP_

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <sys/time.h>
#include <sstream>

#include <ESPectro32_Board.h>

BLEDevice ble;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "914b"
#define CHARACTERISTIC_UUID "26a8"

class MyCharCallbackHander: public BLECharacteristicCallbacks {
	void onRead(BLECharacteristic* pCharacteristic) {
		struct timeval tv = {0};
		struct timezone *tz = {0};
		gettimeofday(&tv, &tz);

		std::ostringstream os;
		os << "Time: " << tv.tv_sec;
		pCharacteristic->setValue(os.str());
	}

	void onWrite(BLECharacteristic* pCharacteristic) {
		std::string value = pCharacteristic->getValue();

		if (value.length() > 0) {
			Serial.println("*********");
			Serial.print("New value: ");
			for (int i = 0; i < value.length(); i++) {
				Serial.printf("%x", value[i]);
			}

			Serial.println();
			Serial.println("*********");

			if (value[0] == 0x00) {
				ESPectro32.turnOffLED();
			}
			else {
				ESPectro32.turnOnLED();
			}
		}
	}
};

void test_ble_server() {
  Serial.println("Starting BLE work!");

  ble.init("ESPectro32-001");
  BLEServer *pServer = new BLEServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCharCallbackHander());
  pCharacteristic->setValue("Hello World");
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}



#endif /* MAIN_EXPLORE_TESTBLESERVER_HPP_ */
