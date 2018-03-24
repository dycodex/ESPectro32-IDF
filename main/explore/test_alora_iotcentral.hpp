/*
 * TestAlora.hpp
 *
 *  Created on: Oct 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_ALORA_IOTCENTRAL_HPP_
#define MAIN_EXPLORE_ALORA_IOTCENTRAL_HPP_

#include <Arduino.h>
#include <AloraSensorKit.h>
#include <esp_log.h>
#define _GLIBCXX_USE_C99
#include <string>

#include <ESPectro32_Board.h>

#define DISPLAY_OLED 1

#if DISPLAY_OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(OLED_SDA, OLED_SCL);
#endif

#include <WiFiManager.h>
WiFiManager wifiMgr;

#include "../Constants.h"

const static char* TAG_AZURE = "AZURE";

#include <ESPectro32_RGBLED_Animation.h>

RgbLedColor_t aCol(200, 0, 80);
ESPectro32_RGBLED_GlowingAnimation glowAnim(ESPectro32.RgbLed(), aCol);

#include <AzureIoTHubMQTTClient.h>

//HostName=saas-iothub-77dfc9bb-86b1-48d8-8e1d-5cca69e14ef7.azure-devices.net;DeviceId=1l9b7qr;SharedAccessKey=9wwC0dus3S4TLHLJLVgZU0/H4/GkVqEGgd2XzhKPwUw=
#define IOTHUB_HOSTNAME         "[YOUR_AZURE_IOT_HUB].azure-devices.net"
#define DEVICE_ID               "[YOUR_DEVICE_ID]"
#define DEVICE_KEY              "[YOUR_DEVICE_KEY]" //Primary key of the device

AzureIoTHubMQTTClient client(IOTHUB_HOSTNAME, DEVICE_ID, DEVICE_KEY);

AloraSensorKit sensorKit;

std::string iotHubPayload;
unsigned long lastDataPublish = 0;

#if DISPLAY_OLED
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
	display.print("LUX:");
	display.setCursor(128/2, display.getCursorY());
	display.println("CO2:");

	display.setCursor(0, display.getCursorY() + 2);
	display.setTextSize(2);
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.print(vals.lux);
	display.setCursor(128/2, display.getCursorY());
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.println(vals.co2);

	display.display();
}
#endif

static void test_alora_iotcentral() {

    Wire.begin();

    sensorKit.begin();

    delay(1000);

    sensorKit.scanAndPrintI2C(Serial);

#if DISPLAY_OLED
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
#endif

    	wifiMgr.onWiFiConnected([](bool newConn, wifi_config_t *cfg) {
    		ESP_LOGI("WIFI", "IP: %s", wifiMgr.getStationIpAddress().c_str());
    	});

    	wifiMgr.begin(WIFI_MODE_STA, false);
    	wifiMgr.connectToAP("Andri's iPhone X", "11223344");
    //	wifiMgr.connectToAP(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

    	//Actually start
    	wifiMgr.start();

    	// Wait for connection, this will block
    	wifiMgr.waitForConnection();

    	ESP_LOGI(TAG_AZURE, "Connected");

#if DISPLAY_OLED
    	display.setCursor(0, 0);
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.println(wifiMgr.isConnected()? "Connected": "Not Connected");
	display.display();
#endif

	client.begin();
	client.start();

	client.onEvent([](AzureIoTHubMQTTClient::AzureIoTHubMQTTClientEvent e) {
			ESP_LOGI(TAG_AZURE, "Network event: %d", e);
	});

	for(;;) {

		sensorKit.run();

		if ((millis() - lastDataPublish > (ALORA_SENSOR_QUERY_INTERVAL * 20))) {

			// get sensor values
			SensorValues vals = sensorKit.getLastSensorData();
			displaySensorValue(vals);

			String qs;
			sensorKit.printSensingTo(qs);
			Serial.println(F("Sensor JSON:"));
			Serial.println(qs);

			iotHubPayload = "{\"T\":" + std::to_string(vals.T1) + ", "
							"\"L\":" + std::to_string(vals.lux) + ", "
							"\"P\":" + std::to_string(vals.P) + ", "
							"\"G\":" + std::to_string(vals.gas) + ", "
							"\"H\":" + std::to_string(vals.H1) + "}";

			client.sendEvent((char*)iotHubPayload.c_str());

			lastDataPublish = millis();
		}
	}

}
#endif /* MAIN_EXPLORE_ALORA_IOTCENTRAL_HPP_ */
