/*
 * AzureIoTHubMQTTClient.cpp
 *
 *  Created on: Jun 12, 2017
 *      Author: andri
 */

#include "AzureIoTHubMQTTClient.h"
#include "soc/soc.h"
#include <algorithm>
//#include "sha256.h"
#include "Base64.h"
#include "Utils.h"
#include "mbedtls/asn1.h"
#include "mbedtls/bignum.h"
#include "mbedtls/pk.h"

#define MQTT_CONNECTED_EVT 	BIT0
#define MQTT_STOP_REQ_EVT 	BIT1
#define MQTT_SUBSDATA_EVT 	BIT2

#define HASH_LENGTH 32

static EventGroupHandle_t mqttEventGroup_ = NULL;
static AzureIoTHubMQTTClient::iothub_subscription_data_t lastSubcriptionData_;

static void connected_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	AZURE_DEBUG_PRINT("MQTT Connected\n");

//    mqtt_client *client = (mqtt_client *)self;
//    mqtt_subscribe(client, "/test-andri-sub", 0);
//    mqtt_publish(client, "/test-andri", "howdy!", 6, 0, 0);

	xEventGroupSetBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void disconnected_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	AZURE_DEBUG_PRINT("MQTT Disconnected\n");
	xEventGroupClearBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void reconnect_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	xEventGroupClearBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void subscribe_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	AZURE_DEBUG_PRINT("MQTT Subscribe OK\n");
}
static void publish_cb(mqtt_client *self, mqtt_event_data_t *params)
{
	AZURE_DEBUG_PRINT("MQTT payload published\n");
}
static void data_cb(mqtt_client *self, mqtt_event_data_t *params)
{
    mqtt_client *client = (mqtt_client *)self;
    mqtt_event_data_t *event_data = (mqtt_event_data_t *)params;

    //save to lastSubcriptionData_

    lastSubcriptionData_.client = client;

	if (event_data->data_offset == 0) {

		char *topic = (char *)malloc(event_data->topic_length + 1);
		memcpy(topic, event_data->topic, event_data->topic_length);
		topic[event_data->topic_length] = 0;
//      NET_DEBUG_PRINT("Sub topic: %s\n", topic);
		lastSubcriptionData_.topic = String(topic);
		free(topic);
	}

	char *data = (char *)malloc(event_data->data_length + 1);
	memcpy(data, event_data->data, event_data->data_length);
	data[event_data->data_length] = 0;
	lastSubcriptionData_.payload = String(data);
	free(data);

    xEventGroupSetBits(mqttEventGroup_, MQTT_SUBSDATA_EVT);
}

AzureIoTHubMQTTClient::AzureIoTHubMQTTClient(const char* iotHubHostName, const char* deviceId, const char* deviceKey):
Task(0, "AzureIoTHubMQTTClient", 4096*2, configMAX_PRIORITIES - 3) {

	deviceId_ = (char*)malloc(sizeof(char*) * CONFIG_MQTT_MAX_CLIENT_LEN);
	//iotHubHostName_ = (char*)malloc(sizeof(char*) * CONFIG_MQTT_MAX_HOST_LEN);
	deviceKey_ = (char*)malloc(sizeof(char*) * CONFIG_MQTT_MAX_PASSWORD_LEN);

	strcpy(deviceId_, deviceId);
	//strcpy(iotHubHostName_, iotHubHostName);
	strcpy(deviceKey_, deviceKey);

	strcpy(mqttSettings_.host, iotHubHostName);
	strcpy(mqttSettings_.client_id, deviceId);
}

AzureIoTHubMQTTClient::~AzureIoTHubMQTTClient() {

	//free(deviceId_);
	//free(iotHubHostName_);
	free(deviceKey_);

	stop();
//	if (requestDataQueue_ != NULL) {
//		vQueueDelete(requestDataQueue_);
//	}
}

String AzureIoTHubMQTTClient::createIotHubSASToken(char* key,
		String url, long expire) {

	//ESP_LOGD("NET", "URL: %s", url.c_str());
	//std::transform(url.begin(), url.end(), url.begin(), ::tolower);
	url.toLowerCase();
	//ESP_LOGD("NET", "URL lower: %s", url.c_str());
	if (expire == 0) {
		expire = 1737504000; //hardcoded expire
	}

	String stringToSign = url + "\n" + String(expire);

	// START: Create signature
	// https://raw.githubusercontent.com/adamvr/arduino-base64/master/examples/base64/base64.ino

	int keyLength = strlen(key);

	int decodedKeyLength = base64_dec_len(key, keyLength);
	char decodedKey[decodedKeyLength];  //allocate char array big enough for the base64 decoded key

	base64_decode(decodedKey, key, keyLength);  //decode key

//	Sha256.initHmac((const uint8_t*)decodedKey, decodedKeyLength);
//	Sha256.print(stringToSign.c_str());
//	char* sign = (char*) Sha256.resultHmac();

	unsigned char sign[48];
//	int ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
//	                       (const unsigned char *) decodedKey, decodedKeyLength, sign);
	int ret = mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
			(const unsigned char *) decodedKey, decodedKeyLength,
			(const unsigned char *) (stringToSign.c_str()), stringToSign.length(), sign);

	if (ret != 0) {
		ESP_LOGE("NET", "mbedtls_md failed: 0x%x", ret);
	}

	// END: Create signature

	// START: Get base64 of signature
	int encodedSignLen = base64_enc_len(HASH_LENGTH);
	char encodedSign[encodedSignLen];
	base64_encode(encodedSign, (char*)sign, HASH_LENGTH);

	// SharedAccessSignature
	std::string urlEnc = urlEncode(encodedSign);
	String retTok = "sr=" + url + "&sig="+ String(urlEnc.c_str()) + "&se=" + String(expire);
	// END: create SAS

	return retTok;
}

bool AzureIoTHubMQTTClient::begin() {

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

	AZURE_DEBUG_PRINT("Starting MQTT...");

	mqttSettings_.connected_cb = connected_cb;
	mqttSettings_.disconnected_cb = disconnected_cb;
	mqttSettings_.reconnect_cb = reconnect_cb;
	mqttSettings_.subscribe_cb = subscribe_cb;
	mqttSettings_.publish_cb = publish_cb;
	mqttSettings_.data_cb = data_cb;
	mqttSettings_.port = AZURE_IOTHUB_MQTT_PORT;
	//strcpy(mqttSettings_.host, iotHubHostName_);
	//strcpy(mqttSettings_.client_id, deviceId_);
	strcpy(mqttSettings_.lwt_topic, "/lwt");
	strcpy(mqttSettings_.lwt_msg, "offline");
	mqttSettings_.lwt_qos = 0;
	mqttSettings_.lwt_retain = 0;

	return true;
}

bool AzureIoTHubMQTTClient::reconnect() {

	//at this time, time should be initialized
	if (sasToken_.length() == 0) {
		AZURE_DEBUG_PRINT("Creating SAS Token!");

		String path = "/devices/" + String(deviceId_);
		std::string urlEncoded = urlEncode(path.c_str());
		String url = String(mqttSettings_.host) + String(urlEncoded.c_str());

		time_t now;
		time(&now);

		long expire = now + (AZURE_IOTHUB_TOKEN_EXPIRE);
		AZURE_DEBUG_PRINT("SAS Token expire: %ld", expire);

		//TODO: Store SAS token? So that no expensive operation for each begin
		//sasToken_ = "sr=dycodex.azure-devices.net%2fdevices%2fespectro%2d01&sig=YJAGohXg6Vl0v9Wmd1LzHUppAFdxaC13CcuITjaH37Y%3d&se=1502115379";//createIotHubSASToken(devKey, url, expire);
		sasToken_ = createIotHubSASToken(deviceKey_, url, expire);
		AZURE_DEBUG_PRINT("SAS Token: %s\n", sasToken_.c_str());

	}

	//do connect
	//String mqttUname =  String(mqttSettings_.host) + "/" + String(deviceId_) + "/DeviceClientType=0.1.0";
	String mqttUname =  String(mqttSettings_.host) + "/" + String(deviceId_) + "/api-version=2016-11-14";
	String mqttPassword = "SharedAccessSignature " + sasToken_;

	strcpy(mqttSettings_.username, mqttUname.c_str());
	strcpy(mqttSettings_.password, mqttPassword.c_str());

	//AZURE_DEBUG_PRINT("Connecting to: %s, user: %s, pass: %s", mqttSettings_.host, mqttSettings_.username, mqttSettings_.password);

	mqttClient_ = mqtt_start(&mqttSettings_);

	changeEventTo(AzureIoTHubMQTTClientEventConnecting);

	//wait for connection

	EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_CONNECTED_EVT, false, false, portMAX_DELAY);

	if (uxBits & MQTT_CONNECTED_EVT) {
		//AZURE_DEBUG_PRINT("MQTT Ready!");
	}

	String _respTopic = "devices/" + String(deviceId_) + "/messages/devicebound/#";

	AZURE_DEBUG_PRINT("MQTT Subscribe to: %s", _respTopic.c_str());
	mqtt_subscribe(mqttClient_, _respTopic.c_str(), 0);

	return true;
}

bool AzureIoTHubMQTTClient::sendEvent(char *payload) {
	if (requestDataQueue_ == 0) {
		return false;
	}

	xQueueSendToBack(requestDataQueue_, &payload, 0);
	return true;
}

void AzureIoTHubMQTTClient::start(void* taskData) {

	Task::start(taskData);
}

bool AzureIoTHubMQTTClient::stop() {

	mqtt_stop();
	//mqtt_destroy(mqttClient_);

	xEventGroupSetBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);

	return true;
}

void AzureIoTHubMQTTClient::run() {

	EventBits_t uxBits;

	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_SUBSDATA_EVT, true, false, 10); //clear on exit
	if (uxBits & MQTT_SUBSDATA_EVT) {
		//AZURE_DEBUG_PRINT("MQTT Data Ready: %s", lastSubcriptionData_.topic.c_str());
		handleSubscriptionData(lastSubcriptionData_);
	}

	if (this->requestDataQueue_ == 0) {
		return;
	}


	char* jsonString;
	if (xQueueReceive(this->requestDataQueue_, &jsonString, 100/portTICK_PERIOD_MS) == pdFALSE) {
		//AZURE_DEBUG_PRINT("NO DATA QUEUED");
		return;
	}

	//EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_CONNECTED_EVT, false, false, portMAX_DELAY);

	if (uxBits & MQTT_CONNECTED_EVT) {
		//AZURE_DEBUG_PRINT("MQTT Ready!");
	}


	size_t msgLen = String(jsonString).length();
	const char *payload = jsonString;
	AZURE_DEBUG_PRINT("MQTT payload: %s", payload);

	String _pubpTopic = "devices/" + String(deviceId_) + "/messages/events/";
	//AZURE_DEBUG_PRINT("MQTT Topic: %s", _pubpTopic.c_str());

	mqtt_publish(mqttClient_, _pubpTopic.c_str(), payload, msgLen, 0, 0);

}

void AzureIoTHubMQTTClient::runAsync(void* taskData) {
	doTask(taskData);
}

void AzureIoTHubMQTTClient::onCloudCommand(String cmd, ClientCommandCallback callback) {
	parseCommandAsJson_ = true;
	if (commandsHandlerMap_.size() == 0) {
	}

	commandsHandlerMap_[cmd] = callback;
}

void AzureIoTHubMQTTClient::doTask(void* taskData) {

	//Subscribe to service and get the data queue
//	svc_.subscribeForRequestData(&requestDataQueue_);

	changeEventTo(AzureIoTHubMQTTClientEventNTPSyncing);
	//deal with time
	char strftime_buf[64];
	getTime(strftime_buf);
	AZURE_DEBUG_PRINT("The current date/time: %s", strftime_buf);
	changeEventTo(AzureIoTHubMQTTClientEventNTPSynced);

	reconnect();

	//at this point, all is ready
	changeEventTo(AzureIoTHubMQTTClientEventConnected);

	while(1) {

		run();

		EventBits_t uxBits;
		uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_STOP_REQ_EVT, false, false, 0);
		if (uxBits & MQTT_STOP_REQ_EVT) {
			AZURE_DEBUG_PRINT("Azure IoT Hub stop.");
			// Clear stop event bit
			xEventGroupClearBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);
			break;
		}

		vTaskDelay(10/portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

void AzureIoTHubMQTTClient::initializeSntp() {
	AZURE_DEBUG_PRINT("Initializing SNTP...");
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
}

void AzureIoTHubMQTTClient::obtainTime() {
	initializeSntp();

	// wait for time to be set
	time_t now = 0;
	struct tm timeinfo = { 0 };
	int retry = 0;
	const int retry_count = 10;
	while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
		AZURE_DEBUG_PRINT("Waiting for system time to be set... (%d/%d)", retry, retry_count);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		time(&now);
		localtime_r(&now, &timeinfo);
	}
}

void AzureIoTHubMQTTClient::getTime(char *timeStr) {
	//deal with time
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	// Is time set? If not, tm_year will be (1970 - 1900).
	if (timeinfo.tm_year < (2016 - 1900)) {
		AZURE_DEBUG_PRINT("Time is not set yet. Connecting to WiFi and getting time over NTP.");
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

void AzureIoTHubMQTTClient::changeEventTo(AzureIoTHubMQTTClientEvent event) {
	currentEvent_ = event;

	if (eventCallback_) {
		eventCallback_(event);
	}
}

void AzureIoTHubMQTTClient::handleSubscriptionData(AzureIoTHubMQTTClient::iothub_subscription_data_t &subsData) {

	if (subsData.client != mqttClient_) {
		AZURE_DEBUG_PRINT("NOT for me");
		return;
	}

//	AZURE_DEBUG_PRINT("Subscribed topic: %s", subsData.topic.c_str());
	AZURE_DEBUG_PRINT("Subscribed payload: %s", subsData.payload.c_str());

	if (subscriptionDataAvailableCallback_) {
		subscriptionDataAvailableCallback_(subsData);
	}

	if (parseCommandAsJson_ && commandsHandlerMap_.size() > 0) {

		//Command should be in format: {\"Name\":\"ActivateRelay\",\"Parameters\":{\"Activated\":0}}

		//AZURE_DEBUG_PRINT("Process command");

		cJSON *root = cJSON_Parse(subsData.payload.c_str());

		String receivedCommandName = "";
		cJSON *item = cJSON_GetObjectItem(root, "Name"); //Case-insensitive
		if (item == NULL) {
			return;
		}
		else {
			receivedCommandName = String(item->valuestring);
		}

		AZURE_DEBUG_PRINT("Received command name: \"%s\"", receivedCommandName.c_str());

		for (const auto &pair : commandsHandlerMap_) {

			if (!((String)pair.first).equals(receivedCommandName)) {
				continue;
			}

			cJSON *params = cJSON_GetObjectItem(root, "Parameters");
			if (params == NULL) {
				params = cJSON_GetObjectItem(root, "Value");
				if (params == NULL) {
					continue;
				}
			}

			//AZURE_DEBUG_PRINT("Param key: %s", params->string);

			ClientCommandCallback cb = pair.second;
			//cb(pair.first, params->child);
			cb(pair.first, params);
		}
	}
}

