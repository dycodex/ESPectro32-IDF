/*
 * NetworkService.h
 *
 *  Created on: Jun 12, 2017
 *      Author: andri
 */

#ifndef MAIN_NETWORKSERVICE_H_
#define MAIN_NETWORKSERVICE_H_

#include "esp_event.h"
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#define _GLIBCXX_USE_C99
#include <string.h>
#include <Arduino.h>
#include <functional>
#include <map>

#include <time.h>
#include <sys/time.h>
#include "apps/sntp/sntp.h"
#include <cJSON.h>

#include "Task.h"

#include "sdkconfig.h"
#include "esp_log.h"

extern "C" {
	#include "mqtt.h"
}

#define AZURE_DEBUG_PRINT(...)  ESP_LOGI("AZURE", __VA_ARGS__);
#define AZURE_INFO_PRINT(...)   ESP_LOGI("AZURE", __VA_ARGS__);

#define AZURE_IOTHUB_MQTT_PORT    	8883
#define AZURE_IOTHUB_TOKEN_EXPIRE    10*24*3600 //10 days

/**
 * @brief AzureIoTHubMQTTClient is a class to work with Azure IoT Hub via MQTT protocol for ESP32-based board.
 * Depends on `espmqtt` library. Make sure to activate `Enable MQTT over SSL` on espmqtt config.
 */
class AzureIoTHubMQTTClient: public Task {
public:

	struct iothub_subscription_data_t {
		mqtt_client *client;
		String topic;
		String payload;
	};

	enum AzureIoTHubMQTTClientEvent {
		AzureIoTHubMQTTClientEventUnknown,
		AzureIoTHubMQTTClientEventNTPSyncing,
		AzureIoTHubMQTTClientEventNTPSynced,
		AzureIoTHubMQTTClientEventConnecting,
		AzureIoTHubMQTTClientEventConnected,
		AzureIoTHubMQTTClientEventDisconnected
	};

	typedef std::function<void(const AzureIoTHubMQTTClientEvent event)> EventCallback;
	typedef std::function<void(AzureIoTHubMQTTClient::iothub_subscription_data_t &subsData)> SubscriptionDataAvailableCallback;

	typedef std::function<void(String cmd, cJSON *item)> ClientCommandCallback;
	typedef std::map<String, ClientCommandCallback> CommandsHandlerMap_t;

	AzureIoTHubMQTTClient(const char* iotHubHostName, const char* deviceId, const char* deviceKey);
	virtual ~AzureIoTHubMQTTClient();

	bool begin();
	bool reconnect();
	void run();
	void runAsync(void *taskData);

	void start(void *taskData=nullptr);
	bool stop();

	bool sendEvent(char * payload);

	void onSubscriptionDataAvailable(SubscriptionDataAvailableCallback cb) {
		subscriptionDataAvailableCallback_ = cb;
	}

	void onEvent(EventCallback cb) {
		eventCallback_ = cb;
	}

	// Command should be in format similar to: {\"Name\":\"ActivateRelay\",\"Parameters\":{\"Activated\":0}}
	// or: {\"Name\":\"ActivateRelay\",\"Value\":0}
	void onCloudCommand(String cmd, ClientCommandCallback callback);

private:

	char* iotHubHostName_ = nullptr;
	char* deviceId_ = nullptr;
	char* deviceKey_ = nullptr;
	String sasToken_;

	EventCallback eventCallback_ = NULL;
	AzureIoTHubMQTTClientEvent currentEvent_ = AzureIoTHubMQTTClientEventUnknown;
	void changeEventTo(AzureIoTHubMQTTClientEvent event);

	mqtt_settings mqttSettings_ = {};
	mqtt_client *mqttClient_ = nullptr;

	//EventGroupHandle_t mqttEventGroup_ = NULL;
	xQueueHandle requestDataQueue_ = 0;

	void doTask(void* taskData);

	void initializeSntp();
	void obtainTime();
	void getTime(char *timeStr);
	void registerToCloud();

	String createIotHubSASToken(char *key, String url, long expire);

	SubscriptionDataAvailableCallback subscriptionDataAvailableCallback_ = NULL;
	void handleSubscriptionData(AzureIoTHubMQTTClient::iothub_subscription_data_t &subdata);

	CommandsHandlerMap_t commandsHandlerMap_;
	bool parseCommandAsJson_ = false;
};

#endif /* MAIN_NETWORKSERVICE_H_ */
