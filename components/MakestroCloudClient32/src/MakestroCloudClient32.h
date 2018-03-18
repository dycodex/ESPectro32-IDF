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

#include <Arduino.h>
#include <functional>
#include <vector>
#include <map>

#include <time.h>
#include <sys/time.h>
#include "apps/sntp/sntp.h"

#include "Task.h"

#include "sdkconfig.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

extern "C" {
	#include "mqtt.h"
}

#define MAKESTRO_DEBUG_PRINT(...)  ESP_LOGI("MAKESTRO", __VA_ARGS__);
#define MAKESTRO_INFO_PRINT(...)   ESP_LOGI("MAKESTRO", __VA_ARGS__);

#define MAKESTROCLOUD_MQTT_HOST   	"cloud.makestro.com"
#define MAKESTROCLOUD_MQTT_PORT    	1883
#define MAKESTROCLOUD_DEVICE_ID_MAX_LENGTH 127

struct mqtt_subscription_data_t {
		mqtt_client *client;
		String topic;
		String payload;
};

class MakestroCloudClient32: public Task {
public:

	enum MakestroCloudClientEvent {
		MakestroCloudClientEventUnknown,
		MakestroCloudClientEventNTPSyncing,
		MakestroCloudClientEventNTPSynced,
		MakestroCloudClientEventConnecting,
		MakestroCloudClientEventConnected,
		MakestroCloudClientEventDisconnected
	};

	typedef std::function<void(const MakestroCloudClientEvent event)> EventCallback;
	typedef std::function<void(mqtt_subscription_data_t &subsData)> SubscriptionDataAvailableCallback;
	typedef std::function<void(String, String)> SubscribedPropertyCallback;

	typedef std::map<String, SubscribedPropertyCallback> PropertyCallbackMap;

	MakestroCloudClient32(const char* userName, const char* key, const char* projectName, const char* clientId = nullptr);
	virtual ~MakestroCloudClient32();

	bool begin();
	bool reconnect();
	bool isConnected();
	void run();
	void runAsync(void *taskData);

	void start(void *taskData=nullptr);
	bool stop();

	bool publishData(char * payload);
	bool publishData(String payload);

	void onSubscriptionDataAvailable(SubscriptionDataAvailableCallback cb) {
		subscriptionDataAvailableCallback_ = cb;
	}

	void subscribeProperty(String property, SubscribedPropertyCallback callback);
	void subscribePropertyWithTopic(String topic, String property, SubscribedPropertyCallback callback);

	void onEvent(EventCallback cb) {
		eventCallback_ = cb;
	}

private:

	char* projectName_ = nullptr;
	char* deviceId_ = nullptr;

	EventCallback eventCallback_ = NULL;
	MakestroCloudClientEvent currentEvent_ = MakestroCloudClientEventUnknown;
	void changeEventTo(MakestroCloudClientEvent event);

	mqtt_settings mqttSettings_ = {};
	mqtt_client *mqttClient_ = nullptr;

	//EventGroupHandle_t mqttEventGroup_ = NULL;
	xQueueHandle requestDataQueue_ = 0;

	void doTask(void* taskData);

	void initializeSntp();
	void obtainTime();
	void getTime(char *timeStr);
	//void registerToCloud();

	//mqtt_subscription_data_t lastSubcriptionData_;
	SubscriptionDataAvailableCallback subscriptionDataAvailableCallback_ = NULL;
	void handleSubscriptionData(mqtt_subscription_data_t &subdata);

	bool parseMessageAsJson_ = false;
	PropertyCallbackMap subscribedProperties_;
};

#endif /* MAIN_NETWORKSERVICE_H_ */
