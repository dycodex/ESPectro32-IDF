/*
 * TestAlora.hpp
 *
 *  Created on: Oct 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTALORA_MAKESTRO_OLED_HPP_
#define MAIN_EXPLORE_TESTALORA_MAKESTRO_OLED_HPP_

#include <Arduino.h>
#include <AloraSensorKit.h>
#include <MakestroCloudClient32.h>

#include <esp_log.h>

#include <ESPectro32_Board.h>

#define ALORA_WIFI_ENABLED 1

#if ALORA_WIFI_ENABLED
#include <WiFiManager.h>
WiFiManager wifiMgr;
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

#include "../Constants.h"

#include <MakestroCloudClient32.h>

#define DEVICE_ID "1234567890"
MakestroCloudClient32 client(DEFAULT_MAKESTRO_USER, DEFAULT_MAKESTRO_KEY, "ALORA_V2", DEVICE_ID);

AloraSensorKit sensorKit;

static void displaySensorValue(SensorValues& vals) {
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0,0);
	display.print("Temp:");
	display.setCursor(128/2, 0);
	display.println("Hum:");

	display.setCursor(0, display.getCursorY() + 2);
	display.setTextSize(2);
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.print(vals.T1);
	display.setCursor(128/2, display.getCursorY());
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.println(vals.H1);

	display.setCursor(0, display.getCursorY() + 2);
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.print("VOC:");
	display.setCursor(128/2, display.getCursorY());
	display.println("CO2:");

	display.setCursor(0, display.getCursorY() + 2);
	display.setTextSize(2);
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.print(vals.gas);
	display.setCursor(128/2, display.getCursorY());
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.println(vals.co2);

	display.display();
}

static void test_alora_makestro_oled() {

    Wire.begin();
    sensorKit.begin();

    delay(1000);

    //sensorKit.scanAndPrintI2C(Serial);

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
	display.println("Connecting");
	display.display();

#if ALORA_WIFI_ENABLED
	wifiMgr.onWiFiConnected([](bool newConn, wifi_config_t *cfg) {
		ESP_LOGI("WIFI", "IP: %s", wifiMgr.getStationIpAddress().c_str());
//		display.clearDisplay();
//		display.println(wifiMgr.getStationIpAddress().c_str());
//		display.display();
	});

	wifiMgr.begin(WIFI_MODE_STA, false);
	wifiMgr.connectToAP(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);
	//wifiMgr.connectToAP("dycodex", "11223344", 30000);
	//wifiMgr.connectToAP("DyWare-AP3", "p@ssw0rd", 30000);

	//Actually start
	wifiMgr.start();
	wifiMgr.waitForConnection();
#endif


	display.setCursor(0, 0);
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.println(wifiMgr.isConnected()? "Connected": "Not Connected");
	display.display();

	delay(1000);

#if ALORA_WIFI_ENABLED
	if (wifiMgr.isConnected()) {
		client.onSubscriptionDataAvailable([](mqtt_subscription_data_t &subsData) {
			ESP_LOGI("MQTT", ">> Topic: %s. Payload: %s", subsData.topic.c_str(), subsData.payload.c_str());
		});

		client.subscribeProperty("state", [](String prop, String val) {
			ESP_LOGI("MQTT", ">> Prop: %s. Val: %s", prop.c_str(), val.c_str());
		});

		client.begin();
		client.start();
	}
#endif

	//int i = 0;

	for(;;) {

		sensorKit.run();

		// get sensor values
		SensorValues sensorData = sensorKit.getLastSensorData();
		displaySensorValue(sensorData);

		// sensorData.T1 and sensorData.T2 are temperature sensor data
		//Serial.printf("Temp: %.2f, Hum: %.2f, Light: %.2f\n", sensorData.T1, sensorData.H1, sensorData.lux);

		char payloadStr[256];
		//sprintf(payloadStr, "T1=%s&P=%s&H1=%s&AX=%s&AY=%s&AZ=%s&GX=%s&GY=%s&GZ=%s&MX=%s&MY=%s&MZ=%s&MH=%s&T2=%s&H2=%s&L=%s&S=%d&G=%d&B=%d",
		sprintf(payloadStr, "{\"T1\":%.2f,\"P\":%.2f,\"H1\":%.2f,\"AX\":%.4f,\"AY\":%.4f,\"AZ\":%.4f,\"GX\":%.4f,\"GY\":%.4f,\"GZ\":%.4f,\"MH\":%.2f,\"T2\":%.2f,\"H2\":%.2f,\"L\":%.2f,\"G\":%d,\"CO2\":%d,\"BT\":%.3f,\"device_id\":\"%s\"}",
				sensorData.T1, sensorData.P, sensorData.H1,
				sensorData.accelX, sensorData.accelY, sensorData.accelZ,
				sensorData.gyroX, sensorData.gyroY, sensorData.gyroZ,
				//mxStr, myStr, mzStr,
				sensorData.magHeading,
				sensorData.T2, sensorData.H2,
				sensorData.lux,
				sensorData.gas,
				sensorData.co2,
				sensorData.BT,
				DEVICE_ID);

#if ALORA_WIFI_ENABLED
		if (wifiMgr.isConnected()) {
			client.publishData(payloadStr);
		}
#endif
		Serial.println(payloadStr);

		//i++;
		delay(3000);

	}
}

#endif /* MAIN_EXPLORE_TESTALORA_MAKESTRO_OLED_HPP_ */
