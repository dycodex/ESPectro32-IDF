/*
 * TestAlora.hpp
 *
 *  Created on: Oct 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTALORA_HPP_
#define MAIN_EXPLORE_TESTALORA_HPP_

#include <Arduino.h>
#include <AloraSensorKit.h>
#include <MakestroCloudClient32.h>

#include <esp_log.h>

#include <ESPectro32_Board.h>

#include <WiFiManager.h>
WiFiManager wifiMgr;

#include <ESPectro32_LedMatrix_Animation.h>
ESPectro32_LedMatrix_Animation ledMatrixAnim;

#include "led_matrix_frames.h"
#include "../Constants.h"

#include <ESPectro32_RGBLED_Animation.h>

RgbLedColor_t aCol(200, 0, 80);
ESPectro32_RGBLED_GlowingAnimation glowAnim(ESPectro32.RgbLed(), aCol);

#include <MakestroCloudClient32.h>

#define DEVICE_ID "123456789"
MakestroCloudClient32 client(DEFAULT_MAKESTRO_USER, DEFAULT_MAKESTRO_KEY, "ALORA_V2", DEVICE_ID);

AloraSensorKit sensorKit;


//#include <Adafruit_GFX.h>
//#include <Adafruit_SH1106.h>
//#define OLED_SDA 21
//#define OLED_SCL 22
//
//Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

static void test_alora_makestro() {

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


    	//Display
//  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
//	display.display();
//	delay(2000);
//	display.clearDisplay();
//    	// text display tests
//	display.setTextSize(1);
//	display.setTextColor(WHITE);
//	display.setCursor(0,0);
//	display.println("Hello, world!");
//	display.setTextColor(BLACK, WHITE); // 'inverted' text
//	display.println(3.141592);
//	display.setTextSize(2);
//	display.setTextColor(WHITE);
//	display.print("0x"); display.println(0xDEADBEEF, HEX);
//	display.display();


    	if (wifiMgr.waitForConnection()) {
    		ledMatrixAnim.stop();
    		ESPectro32.LedMatrix().displayFrame(2);

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

    		//int i = 0;

    		for(;;) {

    			sensorKit.run();

			// get sensor values
			SensorValues sensorData = sensorKit.getLastSensorData();

			// sensorData.T1 and sensorData.T2 are temperature sensor data
			//Serial.printf("Temp: %.2f, Hum: %.2f, Light: %.2f\n", sensorData.T1, sensorData.H1, sensorData.lux);

			char payloadStr[256];
			//sprintf(payloadStr, "T1=%s&P=%s&H1=%s&AX=%s&AY=%s&AZ=%s&GX=%s&GY=%s&GZ=%s&MX=%s&MY=%s&MZ=%s&MH=%s&T2=%s&H2=%s&L=%s&S=%d&G=%d&B=%d",
			sprintf(payloadStr, "{\"T1\":%.2f,\"P\":%.2f,\"H1\":%.2f,\"AX\":%.4f,\"AY\":%.4f,\"AZ\":%.4f,\"GX\":%.4f,\"GY\":%.4f,\"GZ\":%.4f,\"MH\":%.2f,\"T2\":%.2f,\"H2\":%.2f,\"L\":%.2f,\"G\":%d,\"CO2\":%d,\"device_id\":\"%s\"}",
					sensorData.T1, sensorData.P, sensorData.H1,
					sensorData.accelX, sensorData.accelY, sensorData.accelZ,
					sensorData.gyroX, sensorData.gyroY, sensorData.gyroZ,
					//mxStr, myStr, mzStr,
					sensorData.magHeading,
					sensorData.T2, sensorData.H2,
					sensorData.lux,
					sensorData.gas,
					sensorData.co2,
					DEVICE_ID);

    			client.publishData(payloadStr);
			Serial.println(payloadStr);

    			//i++;
    			delay(5000);

    		}
    	}
}
#endif /* MAIN_EXPLORE_TESTALORA_HPP_ */
