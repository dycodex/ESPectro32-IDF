/*
 *  ThermostatIoTCentral.ino
 *	
 *	This example shows how to publish telemetry to Microsoft IoT Central
 * 
 *  Created on: Mar 25, 2018
 *      Author: andri
 */

#include <Arduino.h>
#include <esp_log.h>

#include <AzureIoTHubMQTTClient.h>

const static char* TAG_AZURE = "AZURE";

//#define IOTHUB_HOSTNAME         "[YOUR_IOT_HUB].azure-devices.net"
//#define DEVICE_ID               "[YOUR_DEVICE_ID]"
//#define DEVICE_KEY              "[YOUR_DEVICE_KEY]" //Primary key of the device

#define WIFI_SSID_NAME_1 		"dycodex"
#define WIFI_SSID_PASS_1 		"11223344"
#define WIFI_SSID_NAME_2 		"GERES10"
#define WIFI_SSID_PASS_2 		"p@ssw0rd"

const static char* IOT_CENTRAL_DEVICE_CONN_STRING = "HostName=[HOSTNAME];DeviceId=[DEVICE_ID];SharedAccessKey=[DEVICE_KEY]";

AzureIoTHubMQTTClient *azureIoTHubClient;

#include <WiFiMulti.h>
WiFiMulti wifiMulti;

String iotHubPayload;
unsigned long lastDataPublish = 0;

void readSensor(float *temp, float *press) {

    //Randomize sensor value for now
    *temp = 20 + (rand() % 10 + 2);
    *press = 80 + ((rand() % 20 + 2) * 1.0f/6);

}

void setup() {

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
}

void loop() {

	if ((millis() - lastDataPublish) > 5000) {

		float temp, press;
		readSensor(&temp, &press);

		//JSON string as payload
		iotHubPayload = "{\"temperature\":" + String(temp) + ", "
						 "\"pressure\":" + String(press) + "}";

		azureIoTHubClient->sendEvent((char*)iotHubPayload.c_str());

		lastDataPublish = millis();
	}

	delay(1);
}
