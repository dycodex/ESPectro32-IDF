/*
 * test_azure_iotcentral.hpp
 *
 *  Created on: Mar 25, 2018
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_AZURE_IOTCENTRAL_HPP_
#define MAIN_EXPLORE_TEST_AZURE_IOTCENTRAL_HPP_

#include <Arduino.h>
#include <esp_log.h>

#define USE_REAL_SENSOR  1

#if USE_REAL_SENSOR
#include <Adafruit_HDC1000.h>
Adafruit_HDC1000 hdc = Adafruit_HDC1000();
#endif

#include <AzureIoTHubMQTTClient.h>

const static char* TAG_AZURE = "AZURE";

//#define IOTHUB_HOSTNAME         "[YOUR_IOT_HUB].azure-devices.net"
//#define DEVICE_ID               "[YOUR_DEVICE_ID]"
//#define DEVICE_KEY              "[YOUR_DEVICE_KEY]" //Primary key of the device

#define WIFI_SSID_NAME_1 		"Andri iPhone X"
#define WIFI_SSID_PASS_1 		"11223344"
#define WIFI_SSID_NAME_2 		"GERES10"
#define WIFI_SSID_PASS_2 		"p@ssw0rd"

const static char* IOT_CENTRAL_DEVICE_CONN_STRING = "[IOT_CENTRAL_DEVICE_CONN_STRING]";

AzureIoTHubMQTTClient *azureIoTHubClient;

#include <WiFiMulti.h>
WiFiMulti wifiMulti;

String iotHubPayload;
unsigned long lastDataPublish = 0;

void readSensor(float *temp, float *press, float *hum) {

#if USE_REAL_SENSOR
	*temp = hdc.readTemperature();
	*hum = hdc.readHumidity();
	*press = 0;
#else
    //Randomize sensor value for now
    *temp = 20 + (rand() % 10 + 2);
    *press = 80 + ((rand() % 20 + 2) * 1.0f/6);
    *hum = 0;
#endif

}

static void test_azure_iotcentral() {

#if USE_REAL_SENSOR
    if (!hdc.begin()) {
    		ESP_LOGE(TAG_AZURE, "Couldn't find sensor!");
    }
#endif

	wifiMulti.addAP(WIFI_SSID_NAME_1, WIFI_SSID_PASS_1);
	wifiMulti.addAP(WIFI_SSID_NAME_2, WIFI_SSID_PASS_2);

	ESP_LOGI(TAG_AZURE, "Attempting to connect to WiFi");
	while (wifiMulti.run() != WL_CONNECTED) {
		Serial.print(".");
		delay(500);
	}

	ESP_LOGI(TAG_AZURE, "Connected to WiFi");

	//azureIoTHubClient = new AzureIoTHubMQTTClient(IOTHUB_HOSTNAME, DEVICE_ID, DEVICE_KEY);

	azureIoTHubClient = new AzureIoTHubMQTTClient();
	bool parseRes = azureIoTHubClient->parseDeviceConnectionString(IOT_CENTRAL_DEVICE_CONN_STRING);
	if (!parseRes) {
		ESP_LOGE(TAG_AZURE, "Invalid device's connection string");
		return;
	}

	azureIoTHubClient->begin();
	azureIoTHubClient->start();

	azureIoTHubClient->onEvent([](AzureIoTHubMQTTClient::AzureIoTHubMQTTClientEvent e) {
		ESP_LOGI(TAG_AZURE, "Network event: %d", e);
	});

	/*
	azureIoTHubClient->onSubscriptionDataAvailable([](AzureIoTHubMQTTClient::iothub_subscription_data_t &subsData) {
		ESP_LOGI(TAG_AZURE, "Command Payload: %s", subsData.payload.c_str());
	});
	*/

	for(;;) {

		if ((millis() - lastDataPublish) > 5000) {

			float temp, press, hum;
			readSensor(&temp, &press, &hum);

			//JSON string as payload
			iotHubPayload = "{\"temperature\":" + String(temp) + ", "
							 "\"humidity\":" + String(hum) + ", "
							 "\"pressure\":" + String(press) + "}";

			azureIoTHubClient->sendEvent((char*)iotHubPayload.c_str());

			lastDataPublish = millis();
		}

		delay(1);
	}
}

#endif /* MAIN_EXPLORE_TEST_AZURE_IOTCENTRAL_HPP_ */
