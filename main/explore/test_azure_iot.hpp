/*
 * test_azure_iot.hpp
 *
 *  Created on: Mar 19, 2018
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_AZURE_IOT_HPP_
#define MAIN_EXPLORE_TEST_AZURE_IOT_HPP_

#include <Arduino.h>
#include <esp_log.h>

#include <AzureIoTHubMQTTClient.h>

const static char* TAG_AZURE = "AZURE";

#define IOTHUB_HOSTNAME         "[YOUR_IOT_HUB].azure-devices.net"
#define DEVICE_ID               "[YOUR_DEVICE_ID]"
#define DEVICE_KEY              "[YOUR_DEVICE_KEY]" //Primary key of the device

#define WIFI_SSID_NAME_1 				"dycodex"
#define WIFI_SSID_PASS_1 				"11223344"
#define WIFI_SSID_NAME_2 				"GERES10"
#define WIFI_SSID_PASS_2 				"p@ssw0rd"

AzureIoTHubMQTTClient client(IOTHUB_HOSTNAME, DEVICE_ID, DEVICE_KEY);

#include <WiFiMulti.h>
WiFiMulti wifiMulti;

String iotHubPayload;
unsigned long lastDataPublish = 0;

void readSensor(float *temp, float *press) {

    //Randomize sensor value for now
    *temp = 20 + (rand() % 10 + 2);
    *press = 90 + ((rand() % 10 + 2) * 1.0f/6);

}

static void test_azure_iot() {

	wifiMulti.addAP(WIFI_SSID_NAME_1, WIFI_SSID_PASS_1);
	wifiMulti.addAP(WIFI_SSID_NAME_2, WIFI_SSID_PASS_2);

	ESP_LOGI(TAG_AZURE, "Attempting to connect to WiFi");
	while (wifiMulti.run() != WL_CONNECTED) {
		Serial.print(".");
		delay(500);
	}

	ESP_LOGI(TAG_AZURE, "Connected to WiFi");

	client.begin();
	client.start();

	client.onEvent([](AzureIoTHubMQTTClient::AzureIoTHubMQTTClientEvent e) {
		ESP_LOGI(TAG_AZURE, "Network event: %d", e);
	});

	/*
	client.onSubscriptionDataAvailable([](AzureIoTHubMQTTClient::iothub_subscription_data_t &subsData) {
		ESP_LOGI(TAG_AZURE, "Command Payload: %s", subsData.payload.c_str());
	});
	*/

	// Handle cloud command, in this case for command name "ActivateRelay"
	// Command should be in format similar to: {\"Name\":\"ActivateRelay\",\"Parameters\":{\"Activated\":0}}
	// or: {\"Name\":\"ActivateRelay\",\"Value\":0}
	client.onCloudCommand("ActivateRelay", [](String cmd, cJSON *item) {
		//ESP_LOGI(TAG_AZURE, "Command Payload: %s", cmd.c_str());
		ESP_LOGI(TAG_AZURE, "The param type: %d", item->type);
		if (item->type != cJSON_Object) {
			ESP_LOGI(TAG_AZURE, "The param -> %s = %d", item->string, item->valueint);
		}
		else {
			if (cJSON_HasObjectItem(item, "Activated")) {
				cJSON *itemObj = cJSON_GetObjectItem(item, "Activated");
				ESP_LOGI(TAG_AZURE, "The param -> %s = %d", itemObj->string, itemObj->valueint);
			}
			if (cJSON_HasObjectItem(item, "Speed")) { //Just example, if the parameter has this
				cJSON *itemObj = cJSON_GetObjectItem(item, "Speed");
				ESP_LOGI(TAG_AZURE, "The param -> %s = %d", itemObj->string, itemObj->valueint);
			}
		}
	});

	for(;;) {

		if ((millis() - lastDataPublish) > 5000) {

			float temp, press;
			readSensor(&temp, &press);

			//JSON string as payload
			iotHubPayload = "{\"deviceId\":\"" + String(DEVICE_ID) + "\", "
                              "\"temperature\":" + String(temp) + ", "
							 "\"pressure\":" + String(press) + "}";

			client.sendEvent((char*)iotHubPayload.c_str());

			lastDataPublish = millis();
		}

		delay(1);
	}
}

#endif /* MAIN_EXPLORE_TEST_AZURE_IOT_HPP_ */
