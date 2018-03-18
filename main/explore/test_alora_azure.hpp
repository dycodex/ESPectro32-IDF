/*
 * TestAlora.hpp
 *
 *  Created on: Oct 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTALORA_AZURE_HPP_
#define MAIN_EXPLORE_TESTALORA_AZURE_HPP_

#include <Arduino.h>
#include <AloraSensorKit.h>
#include <esp_log.h>
#define _GLIBCXX_USE_C99
#include <string>

#include <ESPectro32_Board.h>

#include <WiFiManager.h>
WiFiManager wifiMgr;

#include <ESPectro32_LedMatrix_Animation.h>
ESPectro32_LedMatrix_Animation ledMatrixAnim;

#include "led_matrix_frames.h"
#include "../Constants.h"

const static char* TAG_AZURE = "AZURE";

#include <ESPectro32_RGBLED_Animation.h>

RgbLedColor_t aCol(200, 0, 80);
ESPectro32_RGBLED_GlowingAnimation glowAnim(ESPectro32.RgbLed(), aCol);

#include <AzureIoTHubMQTTClient.h>

//#define IOTHUB_HOSTNAME         "[YOUR_IOT_HUB].azure-devices.net"
//#define DEVICE_ID               "[YOUR_DEVICE_ID]"
//#define DEVICE_KEY              "[YOUR_DEVICE_KEY]" //Primary key of the device

AzureIoTHubMQTTClient client(IOTHUB_HOSTNAME, DEVICE_ID, DEVICE_KEY);

AloraSensorKit sensorKit;

std::string iotHubPayload;
unsigned long lastDataPublish = 0;

static void test_alora_azure() {

    Wire.begin();

    sensorKit.begin();

    delay(2000);

    sensorKit.scanAndPrintI2C(Serial);

    ledMatrixAnim.setLedMatrix(ESPectro32.LedMatrix());
    //	ledMatrixTextAnim.setLedMatrix(ESPectro32.LedMatrix());

    	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_1);
    	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_2);
    	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_3);
    	ledMatrixAnim.addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
    		ledM.clear();
    	});

    	ledMatrixAnim.start(1800, true);

    	wifiMgr.onWiFiConnected([](bool newConn) {
    		ESP_LOGI("WIFI", "IP: %s", wifiMgr.getStationIpAddress().c_str());
    	});

    	wifiMgr.begin(WIFI_MODE_STA, false);
    	wifiMgr.connectToAP(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

    	//Actually start
    	wifiMgr.start();

    	// Wait for connection, this will block
    	if (wifiMgr.waitForConnection()) {
    		ledMatrixAnim.stop();
    		ESPectro32.LedMatrix().displayFrame(2);

    		ESP_LOGI(TAG_AZURE, "Connected");

    		client.begin();
    		client.start();

    		client.onEvent([](AzureIoTHubMQTTClient::AzureIoTHubMQTTClientEvent e) {
    				ESP_LOGI(TAG_AZURE, "Network event: %d", e);
    		});

    		for(;;) {

    			sensorKit.run();

    			//if ((millis() - lastDataPublish > (ALORA_SENSOR_QUERY_INTERVAL * 2))) {

    				String qs;
    				sensorKit.printSensingTo(qs);
    				Serial.println(F("Sensor JSON:"));
    				Serial.println(qs);

    				SensorValues vals = sensorKit.getLastSensorData();
    				iotHubPayload = "{\"deviceId\":\"" + std::string(DEVICE_ID) + "\", "
    								"\"temperature\":" + std::to_string(vals.T1) + ", "
    								"\"light\":" + std::to_string(vals.lux) + ", "
    								"\"pressure\":" + std::to_string(vals.P) + ", "
    								"\"airQuality\":" + std::to_string(vals.gas) + ", "
    								"\"humidity\":" + std::to_string(vals.H1) + "}";

    				client.sendEvent((char*)iotHubPayload.c_str());

    				lastDataPublish = millis();
    			//}
    			delay(5000);
    		}
    	}
}
#endif /* MAIN_EXPLORE_TESTALORA_HPP_ */
