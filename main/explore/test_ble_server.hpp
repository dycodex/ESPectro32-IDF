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
#include <BLE2902.h>
#include <Task.h>
#include <ESPectro32_Board.h>

BLEDevice ble;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

static char TAG_BLE[] = "SampleNotify";

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;

class MyNotifyTask: public Task {
	void runAsync(void *data) {
		uint8_t value = 0;
		while(1) {
			delay(2000);
			ESP_LOGI(TAG_BLE, "*** NOTIFY: %d ***", value);
			pCharacteristic->setValue(&value, 1);
			pCharacteristic->notify();
			//pCharacteristic->indicate();
			value++;
		} // While 1
	} // run
}; // MyNotifyTask

MyNotifyTask *pMyNotifyTask;

class MyServerCallbacks: public BLEServerCallbacks {
	void onConnect(BLEServer* pServer) {
		ESP_LOGI(TAG_BLE, "*** CONNECTED ***");
		pMyNotifyTask->start();
	};

	void onDisconnect(BLEServer* pServer) {
		ESP_LOGI(TAG_BLE, "*** DISCONNECTED ***");
		pMyNotifyTask->stop();
	}
};

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

  pMyNotifyTask = new MyNotifyTask();
  pMyNotifyTask->setStackSize(8000);

  ble.init("ESPectro32-001");
  BLEServer *pServer = new BLEServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID));

  pCharacteristic = pService->createCharacteristic(
                                         	 BLEUUID(CHARACTERISTIC_UUID),
                                         	 BLECharacteristic::PROPERTY_READ 	|
                                         	 BLECharacteristic::PROPERTY_WRITE	|
											 BLECharacteristic::PROPERTY_NOTIFY 	|
											 BLECharacteristic::PROPERTY_INDICATE
                                       );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  pCharacteristic->setCallbacks(new MyCharCallbackHander());
  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}



#endif /* MAIN_EXPLORE_TESTBLESERVER_HPP_ */
