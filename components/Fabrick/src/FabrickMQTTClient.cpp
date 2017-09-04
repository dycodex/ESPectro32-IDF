/*
 * NetworkService.cpp
 *
 *  Created on: Jun 12, 2017
 *      Author: andri
 */

#include "FabrickMQTTClient.h"
#include "soc/soc.h"
#include <algorithm>

#define MQTT_CONNECTED_EVT 	BIT0
#define MQTT_STOP_REQ_EVT 	BIT1
#define MQTT_SUBSDATA_EVT 	BIT2

#define HASH_LENGTH 32

static EventGroupHandle_t mqttEventGroup_ = NULL;
static mqtt_subscription_data_t lastSubcriptionData_;

static void connected_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	NET_DEBUG_PRINT("MQTT Connected\n");

//    mqtt_client *client = (mqtt_client *)self;
//    mqtt_subscribe(client, "/test-andri-sub", 0);
//    mqtt_publish(client, "/test-andri", "howdy!", 6, 0, 0);

	xEventGroupSetBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void disconnected_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	NET_DEBUG_PRINT("MQTT Disconnected\n");
	xEventGroupClearBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void reconnect_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	xEventGroupClearBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void subscribe_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	NET_DEBUG_PRINT("MQTT Subscribe OK\n");
}
static void publish_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	NET_DEBUG_PRINT("MQTT payload published\n");
}
static void data_cb(mqtt_client *self, mqtt_event_data_t *params)
{
//	NET_DEBUG_PRINT("Got subs data");

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
//            NET_DEBUG_PRINT("Sub topic: %s\n", topic);
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
		topic[event_data->topic_length] = 0;
//      NET_DEBUG_PRINT("Sub topic: %s\n", topic);
		lastSubcriptionData_.topic.assign(topic, event_data->topic_length + 1);
		free(topic);
	}

	char *data = (char *)malloc(event_data->data_length + 1);
	memcpy(data, event_data->data, event_data->data_length);
	data[event_data->data_length] = 0;
//  subscriptionData.payload = std::string(data);
	lastSubcriptionData_.payload.assign(data, event_data->data_length + 1);
	free(data);

    xEventGroupSetBits(mqttEventGroup_, MQTT_SUBSDATA_EVT);
}

FabrickMQTTClient::FabrickMQTTClient(std::string deviceId, std::string uname, std::string pass):
Task(0, "FabrickMQTTClient", 4096*2, 10), deviceId_(deviceId), mqttUsername_(uname), mqttPassword_(pass) {

}

FabrickMQTTClient::~FabrickMQTTClient() {
	stop();
//	if (requestDataQueue_ != NULL) {
//		vQueueDelete(requestDataQueue_);
//	}
}

bool FabrickMQTTClient::begin() {

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

	NET_DEBUG_PRINT("Starting MQTT...");

	mqttSettings_.connected_cb = connected_cb;
	mqttSettings_.disconnected_cb = disconnected_cb;
	mqttSettings_.reconnect_cb = reconnect_cb;
	mqttSettings_.subscribe_cb = subscribe_cb;
	mqttSettings_.publish_cb = publish_cb;
	mqttSettings_.data_cb = data_cb;
	mqttSettings_.port = FABRICK_MQTT_PORT;
	//const char* address = iotHubHostName_.c_str();
	// std::string mqttUname =  iotHubHostName_ + "/" + deviceId_ + "/DeviceClientType=0.1.0";
	// std::string mqttPassword = "SharedAccessSignature " + sasToken_;
	//strcpy(mqttSettings_.username, mqttUsername_.c_str());
	//strcpy(mqttSettings_.password, mqttPassword_.c_str());
	strcpy(mqttSettings_.host, FABRICK_MQTT_HOST);
	strcpy(mqttSettings_.client_id, deviceId_.c_str());
	strcpy(mqttSettings_.lwt_topic, "/lwt");
	strcpy(mqttSettings_.lwt_msg, "offline");
	mqttSettings_.lwt_qos = 0;
	mqttSettings_.lwt_retain = 0;

	return true;
}

bool FabrickMQTTClient::reconnect() {

	/*
	//at this time, time should be initialized
	if (sasToken_.empty()) {
		NET_DEBUG_PRINT("Creating SAS Token!\n");

		std::string url = iotHubHostName_ + urlEncode(std::string("/devices/" + deviceId_).c_str());
		char *devKey = (char *)deviceKey_.c_str();

		time_t now;
		time(&now);

		long expire = now + (FABRICK_TOKEN_EXPIRE);
		NET_DEBUG_PRINT("SAS Token expire: %ld\n", expire);

		//TODO: Store SAS token? So that no expensive operation for each begin
		//sasToken_ = "sr=dycodex.azure-devices.net%2fdevices%2fespectro%2d01&sig=YJAGohXg6Vl0v9Wmd1LzHUppAFdxaC13CcuITjaH37Y%3d&se=1502115379";//createIotHubSASToken(devKey, url, expire);
		sasToken_ = createIotHubSASToken(devKey, url, expire);
		NET_DEBUG_PRINT("SAS Token: %s\n", sasToken_.c_str());

	}
	*/
	
	//do connect

	mqttClient_ = mqtt_start(&mqttSettings_);

	changeEventTo(FabrickMQTTClientEventConnecting);

	//wait for connection

	EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_CONNECTED_EVT, false, false, portMAX_DELAY);

	if (uxBits & MQTT_CONNECTED_EVT) {
		//NET_DEBUG_PRINT("MQTT Ready!");
	}

	std::string _respTopic = "client/" + mqttUsername_ + "/wifi/dldata";//"devices/" + deviceId_ + "/messages/devicebound/#";

	NET_DEBUG_PRINT("MQTT Subscribe to: %s", _respTopic.c_str());
	mqtt_subscribe(mqttClient_, _respTopic.c_str(), 0);

	return true;
}

bool FabrickMQTTClient::sendEvent(char *payload) {
	if (requestDataQueue_ == 0) {
		return false;
	}

	xQueueSendToBack(requestDataQueue_, &payload, 0);
	return true;
}

void FabrickMQTTClient::start(void* taskData) {

	Task::start(taskData);
}

bool FabrickMQTTClient::stop() {

	mqtt_stop();
	//mqtt_destroy(mqttClient_);

	xEventGroupSetBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);

	return true;
}

void FabrickMQTTClient::run() {

	EventBits_t uxBits;

	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_SUBSDATA_EVT, true, false, 10); //clear on exit
	if (uxBits & MQTT_SUBSDATA_EVT) {
		//NET_DEBUG_PRINT("MQTT Data Ready: %s", lastSubcriptionData_.topic.c_str());
		handleSubscriptionData(lastSubcriptionData_);
	}

	if (this->requestDataQueue_ == 0) {
		return;
	}


	char* jsonString;
	if (xQueueReceive(this->requestDataQueue_, &jsonString, 2000/portTICK_PERIOD_MS) == pdFALSE) {
		//NET_DEBUG_PRINT("NO DATA QUEUED");
		return;
	}

	//EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_CONNECTED_EVT, false, false, portMAX_DELAY);

	if (uxBits & MQTT_CONNECTED_EVT) {
		//NET_DEBUG_PRINT("MQTT Ready!");
	}


	size_t msgLen = std::string(jsonString).length();
	const char *payload = jsonString;
	NET_INFO_PRINT("MQTT payload: %s", payload);

	//std::string _pubpTopic = "devices/" + deviceId_ + "/messages/events/";
	std::string _pubpTopic = "client/" + mqttUsername_ + "/wifi/uldata";
	//NET_INFO_PRINT("MQTT Topic: %s", _pubpTopic.c_str());

	mqtt_publish(mqttClient_, _pubpTopic.c_str(), payload, msgLen, 0, 0);

}

void FabrickMQTTClient::runAsync(void* taskData) {
	doTask(taskData);
}

void FabrickMQTTClient::doTask(void* taskData) {

	//Subscribe to service and get the data queue
//	svc_.subscribeForRequestData(&requestDataQueue_);

	changeEventTo(FabrickMQTTClientEventNTPSyncing);
	//deal with time
	char strftime_buf[64];
	getTime(strftime_buf);
	NET_DEBUG_PRINT("The current date/time: %s", strftime_buf);
	changeEventTo(FabrickMQTTClientEventNTPSynced);

	reconnect();

	//at this point, all is ready
	changeEventTo(FabrickMQTTClientEventConnected);

	while(1) {

		run();

		EventBits_t uxBits;
		uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_STOP_REQ_EVT, false, false, 0);
		if (uxBits & MQTT_STOP_REQ_EVT) {
			NET_DEBUG_PRINT("MQTT stop.");
			// Clear stop event bit
			xEventGroupClearBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);
			break;
		}

		vTaskDelay(10/portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

void FabrickMQTTClient::initializeSntp() {
	NET_DEBUG_PRINT("Initializing SNTP...");
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
}

void FabrickMQTTClient::obtainTime() {
	initializeSntp();

	// wait for time to be set
	time_t now = 0;
	struct tm timeinfo = { 0 };
	int retry = 0;
	const int retry_count = 10;
	while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
		NET_DEBUG_PRINT("Waiting for system time to be set... (%d/%d)", retry, retry_count);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		time(&now);
		localtime_r(&now, &timeinfo);
	}
}

void FabrickMQTTClient::getTime(char *timeStr) {
	//deal with time
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	// Is time set? If not, tm_year will be (1970 - 1900).
	if (timeinfo.tm_year < (2016 - 1900)) {
		NET_DEBUG_PRINT("Time is not set yet. Connecting to WiFi and getting time over NTP.");
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
	//NET_DEBUG_PRINT("The current date/time: %s", strftime_buf);

	strcpy(timeStr, strftime_buf);
}

void FabrickMQTTClient::registerToCloud() {

//	char payload[100];
//	snprintf ( payload, sizeof payload,
//			   "id=%s&ipAddress=%s",
//			   svc_.getAppSetting().stuff.device.id, svc_.getAppSetting().stuff.config.ipAddress
//	);
//
//	NET_DEBUG_PRINT("MQTT payload: %s", payload);
//	std::string _prefTopic = std::string(svc_.getAppSetting().stuff.account.userId) + "/" + std::string(svc_.getAppSetting().stuff.device.id);
//	std::string _pubpTopic = _prefTopic + "/props";
//	NET_DEBUG_PRINT("MQTT Topic: %s", _pubpTopic.c_str());
//
//	mqtt_publish(mqttClient_, _pubpTopic.c_str(), payload, strlen(payload), 0, 0);
}

void FabrickMQTTClient::changeEventTo(FabrickMQTTClientEvent event) {
	currentEvent_ = event;

	if (eventCallback_) {
		eventCallback_(event);
	}
}

void FabrickMQTTClient::handleSubscriptionData(mqtt_subscription_data_t &subsData) {

	if (subsData.client != mqttClient_) {
		NET_DEBUG_PRINT("NOT for me");
		return;
	}

	NET_DEBUG_PRINT("Sub topic: %s", subsData.topic.c_str());
	NET_DEBUG_PRINT("Sub payload: %s", subsData.payload.c_str());

	if (subscriptionDataAvailableCallback_) {
		subscriptionDataAvailableCallback_(subsData);
	}

//	//if (subsData.topic.find_last_of("response") != std::string::npos) {
//		svc_.notifyResponse(subsData.payload);
//	//}
}

String FabrickMQTTClient::addZeros(String data, int len) {
	// Length of the data
	int data_len = data.length();

	// Adding zeros
	String data_ii = data;
	if (data_len < len) {
		for (int ii = 0; ii < len - data_len; ii++) {
			data_ii = "0" + data_ii;
		}
	}

	return data_ii;
}


void FabrickMQTTClient::sendPayload(int frame_cntr, int data_ch, int data_typ, const char *data, int data_len_in_hex) {
	// Please refer to IPSO Smart Objects Document for packet format.  

	uint8_t payloadLen = deviceId_.length() + 2 + 2 + 2 + data_len_in_hex;

	char pDest[data_len_in_hex + 1];
	FabrickUtils::addZerosToChar(pDest, data, data_len_in_hex);
	NET_DEBUG_PRINT("Zero-padding data -> %s, len %d", pDest, payloadLen);

	char payload[payloadLen + 1];
	snprintf(payload, (payloadLen+1), "%s%02x%02x%02x%s", deviceId_.c_str(), frame_cntr, data_ch, (data_typ-3200), pDest);

	NET_DEBUG_PRINT("WiFi send -> %s", payload);
	sendEvent(payload);
	
//	String device_id = String(deviceId_.c_str());
//
//	// Frame counter
//	// Convert to string and ensure length of 2 (in hex) or 1 byte
//	String frame_cntr_s = addZeros(String(frame_cntr, HEX), 2);
//
//	// Data channel
//	// Convert to string and ensure length of 2 (in hex) or 1 byte
//	String data_ch_s = addZeros(String(data_ch, HEX), 2);
//
//	// Data type
//	// Convert to string and ensure length of 2 (in hex) or 1 byte
//	String data_type_s = addZeros(String(data_typ-3200, HEX), 2);
//
//	// Payload data
//	// Ensure length of data_len_in_hex
//	String data_s = addZeros(String(data), data_len_in_hex);
//
//	// Tx packet
//	String tx_packet = device_id + frame_cntr_s + data_ch_s + data_type_s + data_s;
//
//	// Length of the Tx packet
//	int tx_packet_len = int(tx_packet.length());
//
//	// // Initialisation
//	// char tx_packet_c[tx_packet_len+1];
//
//	// // Publish to MQTT server
//	// tx_packet.toCharArray(tx_packet_c,tx_packet_len+1);                    // Convert to char
//
//	txPayloadString_ = std::string(tx_packet.c_str());
//	char *tx_packet_c = (char*)txPayloadString_.c_str();
//
//	sendEvent(tx_packet_c);
//	// Debug mode print
//
//	NET_DEBUG_PRINT("WiFi send -> %s", txPayloadString_.c_str());

}
