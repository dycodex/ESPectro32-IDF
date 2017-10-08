/*
 * NetworkService.cpp
 *
 *  Created on: Jun 12, 2017
 *      Author: andri
 */

#include "MakestroCloudClient32.h"
#include "soc/soc.h"

#include <cJSON.h>

extern "C" {
#include "str_replace.h"
}

#define MQTT_CONNECTED_EVT 	BIT0
#define MQTT_STOP_REQ_EVT 	BIT1
#define MQTT_SUBSDATA_EVT 	BIT2

#define HASH_LENGTH 32

static EventGroupHandle_t mqttEventGroup_ = NULL;
static mqtt_subscription_data_t lastSubcriptionData_;

static void connected_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	MAKESTRO_DEBUG_PRINT("MQTT Connected\n");

//    mqtt_client *client = (mqtt_client *)self;
//    mqtt_subscribe(client, "/test-andri-sub", 0);
//    mqtt_publish(client, "/test-andri", "howdy!", 6, 0, 0);

	xEventGroupSetBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void disconnected_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	MAKESTRO_DEBUG_PRINT("MQTT Disconnected\n");
	xEventGroupClearBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void reconnect_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	xEventGroupClearBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void subscribe_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	MAKESTRO_DEBUG_PRINT("MQTT Subscribe OK\n");
}
static void publish_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	MAKESTRO_DEBUG_PRINT("MQTT payload published\n");
}
static void data_cb(mqtt_client *self, mqtt_event_data_t *params)
{
//	MAKESTRO_DEBUG_PRINT("Got subs data");

    mqtt_client *client = (mqtt_client *)self;
    mqtt_event_data_t *event_data = (mqtt_event_data_t *)params;

    /*
    if (subscriptionDataQueue_) {

    	mqtt_subscription_data_t subscriptionData = {};
    	subscriptionData.client = client;

        if (event_data->data_offset == 0) {

            char *topic = (char *)malloc(event_data->topic_length + 1);
            memcpy(topic, event_data->topic, event_data->topic_length);
            topic[event_data->topic_length] = 0;
//            MAKESTRO_DEBUG_PRINT("Sub topic: %s\n", topic);
            subscriptionData.topic.assign(topic, event_data->topic_length + 1);
            free(topic);
        }

        char *data = malloc(event_data->data_length + 1);
        memcpy(data, event_data->data, event_data->data_length);
        data[event_data->data_length] = 0;
//        subscriptionData.payload = std::string(data);
        subscriptionData.payload.assign(data, event_data->data_length + 1);
        free(data);

        xQueueSend(subscriptionDataQueue_, &subscriptionData, 0);
    }
    */

    //save to lastSubcriptionData_

    lastSubcriptionData_.client = client;

	if (event_data->data_offset == 0) {

		char *topic = (char *)malloc(event_data->topic_length + 1);
		memcpy(topic, event_data->topic, event_data->topic_length);
		topic[event_data->topic_length] = '\0';
//      MAKESTRO_DEBUG_PRINT("Sub topic: %s\n", topic);
		//lastSubcriptionData_.topic.assign(topic, event_data->topic_length + 1);
		lastSubcriptionData_.topic = String(topic);
		free(topic);
	}

	char *data = (char *)malloc(event_data->data_length + 1);
	memcpy(data, event_data->data, event_data->data_length);
	data[event_data->data_length] = '\0';
//  subscriptionData.payload = std::string(data);
	//lastSubcriptionData_.payload.assign(data, event_data->data_length + 1);
	lastSubcriptionData_.payload = String(data);
	free(data);

    xEventGroupSetBits(mqttEventGroup_, MQTT_SUBSDATA_EVT);
}

MakestroCloudClient32::MakestroCloudClient32(const char* userName, const char* key, const char* projectName, const char* clientId):
Task(0, "MakestroCloudClient32", 4096*2, 10) {

	deviceId_ = (char*)malloc(sizeof(char*) * MAKESTROCLOUD_DEVICE_ID_MAX_LENGTH);
	if (!clientId) {
		sprintf(deviceId_, "%s-%s-default", userName, projectName);
	} else {
		strcpy(deviceId_, clientId);
	}

	strcpy(mqttSettings_.client_id, deviceId_);
	strcpy(mqttSettings_.username, userName);
	strcpy(mqttSettings_.password, key);

	if (projectName) {
		char *proj = str_replace((char*)projectName, " ", "");
		int strlength = (strlen(projectName) + 1);
		projectName_ = (char*)malloc(strlength);
		strcpy(projectName_, proj);
	}
}

MakestroCloudClient32::~MakestroCloudClient32() {
	stop();
//	if (requestDataQueue_ != NULL) {
//		vQueueDelete(requestDataQueue_);
//	}

	free(deviceId_);
	free(projectName_);
}

bool MakestroCloudClient32::begin() {

	if (mqttEventGroup_ == NULL) {
		mqttEventGroup_ = xEventGroupCreate();
	}

	if (this->requestDataQueue_ == 0) {
		this->requestDataQueue_ = xQueueCreate(10, sizeof(char *));
	}

	xEventGroupClearBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);
	xEventGroupClearBits(mqttEventGroup_, MQTT_SUBSDATA_EVT);

//	if (subscriptionDataQueue_ == 0) {
//		subscriptionDataQueue_ = xQueueCreate(10, sizeof(mqtt_subscription_data_t));
//	}

	MAKESTRO_DEBUG_PRINT("Starting MQTT...");

	mqttSettings_.connected_cb = connected_cb;
	mqttSettings_.disconnected_cb = disconnected_cb;
	mqttSettings_.reconnect_cb = reconnect_cb;
	mqttSettings_.subscribe_cb = subscribe_cb;
	mqttSettings_.publish_cb = publish_cb;
	mqttSettings_.data_cb = data_cb;
	mqttSettings_.port = MAKESTROCLOUD_MQTT_PORT;
	strcpy(mqttSettings_.host, MAKESTROCLOUD_MQTT_HOST);
	//strcpy(mqttSettings_.client_id, deviceId_);
	strcpy(mqttSettings_.lwt_topic, "/lwt");
	strcpy(mqttSettings_.lwt_msg, "offline");
	mqttSettings_.lwt_qos = 0;
	mqttSettings_.lwt_retain = 0;

	return true;
}

bool MakestroCloudClient32::reconnect() {

	
	//Do connect

	mqttClient_ = mqtt_start(&mqttSettings_);

	changeEventTo(MakestroCloudClientEventConnecting);

	//wait for connection

	EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_CONNECTED_EVT, false, false, portMAX_DELAY);

	if (uxBits & MQTT_CONNECTED_EVT) {
		//MAKESTRO_DEBUG_PRINT("MQTT Ready!");
	}

	String _respTopic = String(mqttSettings_.username) + "/" + String(projectName_) + "/control";

	MAKESTRO_DEBUG_PRINT("MQTT Subscribe to: %s", _respTopic.c_str());
	mqtt_subscribe(mqttClient_, _respTopic.c_str(), 0);

	return true;
}

bool MakestroCloudClient32::publishData(char *payload) {
	if (requestDataQueue_ == 0) {
		return false;
	}

	xQueueSendToBack(requestDataQueue_, &payload, 0);
	return true;
}

bool MakestroCloudClient32::publishData(String payload) {
	return publishData(payload.c_str());
}

void MakestroCloudClient32::start(void* taskData) {

	Task::start(taskData);
}

bool MakestroCloudClient32::stop() {

	mqtt_stop();
	//mqtt_destroy(mqttClient_);

	xEventGroupSetBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);

	return true;
}

void MakestroCloudClient32::run() {

	EventBits_t uxBits;

	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_SUBSDATA_EVT, true, false, 10); //clear on exit
	if (uxBits & MQTT_SUBSDATA_EVT) {
		//MAKESTRO_DEBUG_PRINT("MQTT Data Ready: %s", lastSubcriptionData_.topic.c_str());
		handleSubscriptionData(lastSubcriptionData_);
	}

	if (this->requestDataQueue_ == 0) {
		return;
	}


	char* jsonString;
	if (xQueueReceive(this->requestDataQueue_, &jsonString, 100/portTICK_PERIOD_MS) == pdFALSE) {
		//MAKESTRO_DEBUG_PRINT("NO DATA QUEUED");
		return;
	}

	//EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_CONNECTED_EVT, false, false, portMAX_DELAY);

	if (uxBits & MQTT_CONNECTED_EVT) {
		//MAKESTRO_DEBUG_PRINT("MQTT Ready!");
	}


	unsigned int msgLen = String(jsonString).length();
	const char *payload = jsonString;
	MAKESTRO_DEBUG_PRINT("MQTT payload: %s", payload);

	//std::string _pubpTopic = "devices/" + deviceId_ + "/messages/events/";
	String _pubpTopic = String(mqttSettings_.username) + "/" + String(projectName_) + "/data";
	//NET_INFO_PRINT("MQTT Topic: %s", _pubpTopic.c_str());

	mqtt_publish(mqttClient_, _pubpTopic.c_str(), payload, msgLen, 0, 0);
}

void MakestroCloudClient32::runAsync(void* taskData) {
	doTask(taskData);
}

void MakestroCloudClient32::doTask(void* taskData) {

	//Subscribe to service and get the data queue
//	svc_.subscribeForRequestData(&requestDataQueue_);

	changeEventTo(MakestroCloudClientEventNTPSyncing);
	//deal with time
	char strftime_buf[64];
	getTime(strftime_buf);
	MAKESTRO_DEBUG_PRINT("The current date/time: %s", strftime_buf);
	changeEventTo(MakestroCloudClientEventNTPSynced);

	reconnect();

	//at this point, all is ready
	changeEventTo(MakestroCloudClientEventConnected);

	while(1) {

		run();

		EventBits_t uxBits;
		uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_STOP_REQ_EVT, false, false, 0);
		if (uxBits & MQTT_STOP_REQ_EVT) {
			MAKESTRO_DEBUG_PRINT("MQTT stop.");
			// Clear stop event bit
			xEventGroupClearBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);
			break;
		}

		vTaskDelay(10/portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

void MakestroCloudClient32::initializeSntp() {
	MAKESTRO_DEBUG_PRINT("Initializing SNTP...");
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
}

void MakestroCloudClient32::obtainTime() {
	initializeSntp();

	// wait for time to be set
	time_t now = 0;
	struct tm timeinfo = { 0 };
	int retry = 0;
	const int retry_count = 10;
	while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
		MAKESTRO_DEBUG_PRINT("Waiting for system time to be set... (%d/%d)", retry, retry_count);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		time(&now);
		localtime_r(&now, &timeinfo);
	}
}

void MakestroCloudClient32::getTime(char *timeStr) {
	//deal with time
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	// Is time set? If not, tm_year will be (1970 - 1900).
	if (timeinfo.tm_year < (2016 - 1900)) {
		MAKESTRO_DEBUG_PRINT("Time is not set yet. Connecting to WiFi and getting time over NTP.");
		obtainTime();
		// update 'now' variable with current time
		time(&now);
	}

	char strftime_buf[64];
	// Set timezone to Eastern Standard Time and print local time
	//setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
	//setenv("TZ", "Etc/GMT-7", 1);
	setenv("TZ", "UTC", 1);
	tzset();
	localtime_r(&now, &timeinfo);
	//strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%dT%H:%M:%S.000Z", &timeinfo);
	//MAKESTRO_DEBUG_PRINT("The current date/time: %s", strftime_buf);

	strcpy(timeStr, strftime_buf);
}

/*
void MakestroCloudClient32::registerToCloud() {

	char payload[100];
	snprintf ( payload, sizeof payload,
			   "id=%s&ipAddress=%s",
			   svc_.getAppSetting().stuff.device.id, svc_.getAppSetting().stuff.config.ipAddress
	);

	MAKESTRO_DEBUG_PRINT("MQTT payload: %s", payload);
	std::string _prefTopic = std::string(svc_.getAppSetting().stuff.account.userId) + "/" + std::string(svc_.getAppSetting().stuff.device.id);
	std::string _pubpTopic = _prefTopic + "/props";
	MAKESTRO_DEBUG_PRINT("MQTT Topic: %s", _pubpTopic.c_str());

	mqtt_publish(mqttClient_, _pubpTopic.c_str(), payload, strlen(payload), 0, 0);
}
*/

void MakestroCloudClient32::changeEventTo(MakestroCloudClientEvent event) {
	currentEvent_ = event;

	if (eventCallback_) {
		eventCallback_(event);
	}
}

void MakestroCloudClient32::subscribeProperty(String property,
		SubscribedPropertyCallback callback) {

	subscribePropertyWithTopic("control", property, callback);
}

void MakestroCloudClient32::subscribePropertyWithTopic(String topic,
		String property, SubscribedPropertyCallback callback) {

	parseMessageAsJson_ = true;

	if (subscribedProperties_.size() == 0 && !topic.equals("control")) {

		String _respTopic = String(mqttSettings_.username) + "/" + String(projectName_) + topic;

	    	MAKESTRO_DEBUG_PRINT("MQTT Subscribe to: %s", _respTopic.c_str());
	    	mqtt_subscribe(mqttClient_, _respTopic.c_str(), 0);
	}

	subscribedProperties_[property] = callback;
}

void MakestroCloudClient32::handleSubscriptionData(mqtt_subscription_data_t &subsData) {

	if (subsData.client != mqttClient_) {
		MAKESTRO_DEBUG_PRINT("NOT for me");
		return;
	}

	MAKESTRO_DEBUG_PRINT("Sub topic: %s", subsData.topic.c_str());
	MAKESTRO_DEBUG_PRINT("Sub payload: %s", subsData.payload.c_str());

	if (subscriptionDataAvailableCallback_) {
		subscriptionDataAvailableCallback_(subsData);
	}

//	//if (subsData.topic.find_last_of("response") != std::string::npos) {
//		svc_.notifyResponse(subsData.payload);
//	//}

	if (parseMessageAsJson_ && subscribedProperties_.size() > 0) {

		cJSON *root = cJSON_Parse(subsData.payload.c_str());
		for (const auto &pair : subscribedProperties_) {

			cJSON *item = cJSON_GetObjectItem(root, ((String)pair.first).c_str());
			if (!item) {
				continue;
			}

			String val;
			if (item->type == cJSON_Number) {
				val = String(item->valueint);
			}
			else {
				val = String(item->valuestring);
			}

			SubscribedPropertyCallback cb = pair.second;
		    cb(pair.first, val);

		}
	}
}

