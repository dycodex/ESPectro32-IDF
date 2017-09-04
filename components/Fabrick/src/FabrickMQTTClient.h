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

#include <time.h>
#include <sys/time.h>
#include "apps/sntp/sntp.h"

#include "Task.h"

#include "sdkconfig.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "FabrickUtils.h"

extern "C" {
	#include "mqtt.h"
}

#define NET_DEBUG_PRINT(...)  ESP_LOGI("NET", __VA_ARGS__);
#define NET_INFO_PRINT(...)   ESP_LOGI("NET", __VA_ARGS__);

#define FABRICK_MQTT_HOST   		"platform.antares.id"//"staging.atilze.com"// "emqs-atilze.giotgateway.com"
#define FABRICK_MQTT_PORT    	1883
#define FABRICK_TOKEN_EXPIRE    	10*24*3600

struct mqtt_subscription_data_t {
		mqtt_client *client;
		std::string topic;
		std::string payload;
};

class FabrickMQTTClient: public Task {
public:

	enum FabrickMQTTClientEvent {
		FabrickMQTTClientEventUnknown,
		FabrickMQTTClientEventNTPSyncing,
		FabrickMQTTClientEventNTPSynced,
		FabrickMQTTClientEventConnecting,
		FabrickMQTTClientEventConnected,
		FabrickMQTTClientEventDisconnected
	};

	typedef std::function<void(const FabrickMQTTClientEvent event)> EventCallback;
	typedef std::function<void(mqtt_subscription_data_t &subsData)> SubscriptionDataAvailableCallback;

	FabrickMQTTClient(std::string deviceId, std::string uname, std::string pass);
	virtual ~FabrickMQTTClient();

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

	void sendPayload(int frame_cntr, int data_ch, int data_typ, const char *data, int data_len_in_hex);

private:

	std::string txPayloadString_;
	
	//std::string iotHubHostName_;
	std::string deviceId_;
	std::string mqttUsername_;
	std::string mqttPassword_;

	EventCallback eventCallback_ = NULL;
	FabrickMQTTClientEvent currentEvent_ = FabrickMQTTClientEventUnknown;
	void changeEventTo(FabrickMQTTClientEvent event);

	mqtt_settings mqttSettings_ = {};
	mqtt_client *mqttClient_ = nullptr;

	//EventGroupHandle_t mqttEventGroup_ = NULL;
	xQueueHandle requestDataQueue_ = 0;

	void doTask(void* taskData);

	void initializeSntp();
	void obtainTime();
	void getTime(char *timeStr);
	void registerToCloud();

	//mqtt_subscription_data_t lastSubcriptionData_;
	SubscriptionDataAvailableCallback subscriptionDataAvailableCallback_ = NULL;
	void handleSubscriptionData(mqtt_subscription_data_t &subdata);
	
	String addZeros(String data, int len);

};

#endif /* MAIN_NETWORKSERVICE_H_ */
