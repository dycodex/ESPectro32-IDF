/*
 * TestBleUart.hpp
 *
 *  Created on: Sep 5, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTBLEMATRIX_HPP_
#define MAIN_EXPLORE_TESTBLEMATRIX_HPP_

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <ESPectro32_Board.h>
#include <ESPectro32_LedMatrix_Animation.h>
ESPectro32_LedMatrix_ScrollTextAnimation ledMatrixTextAnim;

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++)
        		Serial.print(rxValue[i]);

        ledMatrixTextAnim.scrollText(rxValue.c_str(), 6000);

//        for (int i = 0; i < rxValue.length(); i++) {
//			Serial.printf("%x", rxValue[i]);
//		}

        Serial.println();
        Serial.println("*********");
      }
    }
};

static void test_ble_matrix() {
	// Create the BLE Device
	BLEDevice::init("UART Service");

	// Create the BLE Server
	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());

	// Create the BLE Service
	BLEService *pService = pServer->createService(SERVICE_UUID);

	// Create a BLE Characteristic
	pCharacteristic = pService->createCharacteristic(
					  CHARACTERISTIC_UUID_TX,
					  BLECharacteristic::PROPERTY_NOTIFY
					);

	pCharacteristic->addDescriptor(new BLE2902());

	BLECharacteristic *pCharacteristic = pService->createCharacteristic(
										 CHARACTERISTIC_UUID_RX,
										 BLECharacteristic::PROPERTY_WRITE
									   );

	pCharacteristic->setCallbacks(new MyCallbacks());

	// Start the service
	pService->start();

	// Start advertising
	pServer->getAdvertising()->start();
	Serial.println("Waiting a client connection to notify...");

	ledMatrixTextAnim.setLedMatrix(ESPectro32.LedMatrix());
	ledMatrixTextAnim.scrollText("Waiting...", 6000);
}


#endif /* MAIN_EXPLORE_TESTBLEUART_HPP_ */
