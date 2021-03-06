/*
 * TestFabrickMQTT.hpp
 *
 *  Created on: Aug 24, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTFABRICKMQTT_HPP_
#define MAIN_EXPLORE_TESTFABRICKMQTT_HPP_

#include <esp_log.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include <ESPectro32_Board.h>

#include "test_wait_wifi.hpp"

#include <ESPectro32_RGBLED_Animation.h>
RgbLedColor_t aCol(200, 0, 80);
ESPectro32_RGBLED_GlowingAnimation glowAnim(ESPectro32.RgbLed(), aCol);

#include <MakestroCloudClient32.h>

#define DEVICE_ID "daf44817"
MakestroCloudClient32 client(DEFAULT_MAKESTRO_USER, DEFAULT_MAKESTRO_KEY, "Espectro32Test", DEVICE_ID);

#include "ClosedCube_HDC1080.h"

ClosedCube_HDC1080 hdc1080;

static void test_makestro_cloud() {

//	hdc1080.begin(0x40);
//
//	Serial.print("Manufacturer ID=0x");
//	Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
//	Serial.print("Device ID=0x");
//	Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device

	//Will be blocking
	test_wait_wifi();

	client.onSubscriptionDataAvailable([](mqtt_subscription_data_t &subsData) {
		ESP_LOGI("MQTT", ">> Topic: %s. Payload: %s", subsData.topic.c_str(), subsData.payload.c_str());
	});

	client.subscribeProperty("state", [](String prop, String val) {
		ESP_LOGI("MQTT", ">> Prop: %s. Val: %s", prop.c_str(), val.c_str());
		if (val.equals("1")) {
			//ESPectro32.LED().turnOff();
			glowAnim.start(3000, UINT16_MAX);
		}
		else {
			//ESPectro32.LED().turnOn();
			glowAnim.stop();
		}
	});

	client.begin();
	client.start();

	int i = 0;

	/*
	for(;;) {

		Serial.print("T=");
		Serial.print(hdc1080.readTemperature());
		Serial.print("C, RH=");
		Serial.print(hdc1080.readHumidity());
		Serial.println("%");

		char payload[50];
		sprintf(payload, "{\"counter\": %d, \"temp\":%.2f}", i, hdc1080.readTemperature());

		client.publishData(payload);
		i++;
		delay(5000);

	}
	*/
}



#endif /* MAIN_EXPLORE_TESTFABRICKMQTT_HPP_ */
