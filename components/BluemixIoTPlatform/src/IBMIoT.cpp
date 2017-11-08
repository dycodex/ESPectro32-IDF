#include "IBMIoT.h"

static const int IBM_MQTT_CONNECTED_EVENT = BIT0;
static const int IBM_MQTT_DISCONNECTED_EVENT = BIT1;
static const int IBM_MQTT_RECONNECT_EVENT = BIT2;
static EventGroupHandle_t ibm_mqtt_event;

static void ibm_mqtt_connected_cb(mqtt_client* self, mqtt_event_data_t *event) {
    IBMIOT_INFO_PRINT("Connected");
    xEventGroupSetBits(ibm_mqtt_event, IBM_MQTT_CONNECTED_EVENT);
}

static void ibm_mqtt_disconnected_cb(mqtt_client* self, mqtt_event_data_t* event) {
    IBMIOT_INFO_PRINT("Disconnected!");
    xEventGroupSetBits(ibm_mqtt_event, IBM_MQTT_DISCONNECTED_EVENT);
}

static void ibm_mqtt_reconnect_cb(mqtt_client* self, mqtt_event_data_t* event) {
    IBMIOT_INFO_PRINT("Reconnecting");
    xEventGroupSetBits(ibm_mqtt_event, IBM_MQTT_RECONNECT_EVENT);
}

static void ibm_mqtt_subscribe_cb(mqtt_client* self, mqtt_event_data_t* event) {
    IBMIOT_INFO_PRINT("Subscribe acknowledged");
}

static void ibm_mqtt_publish_cb(mqtt_client* self, mqtt_event_data_t* event) {
    IBMIOT_INFO_PRINT("Publish acknowledged");
}

static void ibm_mqtt_data_cb(mqtt_client* self, mqtt_event_data_t* event) {
    IBMIOT_INFO_PRINT("Data arrived");
}

IBMIoTMqttClient::IBMIoTMqttClient(IBMIoTDevice& dev):
 Task("ibm_iot_task", 4096),
 device(&dev),
 eventQueue(0) {}

IBMIoTMqttClient::~IBMIoTMqttClient() {}

void IBMIoTMqttClient::begin() {
    std::stringstream cloudHostnameStream;
    cloudHostnameStream << device->orgId << ".messaging.internetofthings.ibmcloud.com";

    std::stringstream deviceIdStream;
    deviceIdStream << "d:" << device->orgId
        << ":" << device->deviceType
        << ":" << device->deviceId;

    strcpy(settings.client_id, deviceIdStream.str().c_str());
    strcpy(settings.username, "use-token-auth");
    strcpy(settings.password, device->authToken);
    strcpy(settings.host, cloudHostnameStream.str().c_str());
    settings.port = 8883;

    // the callbacks
    settings.connected_cb = ibm_mqtt_connected_cb;
    settings.disconnected_cb = ibm_mqtt_disconnected_cb;
    settings.reconnect_cb = ibm_mqtt_reconnect_cb;
    settings.subscribe_cb = ibm_mqtt_subscribe_cb;
    settings.publish_cb = ibm_mqtt_publish_cb;
    settings.data_cb = ibm_mqtt_data_cb;

    // last will and testament
    strcpy(settings.lwt_topic, "iot-2/evt/last_will/fmt/json");
    strcpy(settings.lwt_msg, "{\"message\":\"offline\"}");
    settings.lwt_qos = 0;
    settings.lwt_retain = 0;

    eventQueue = xQueueCreate(10, sizeof(IBMIoTPayload));
    ibm_mqtt_event = xEventGroupCreate();
}

bool IBMIoTMqttClient::reconnect() {
    mqttcli = mqtt_start(&settings);

    BaseType_t waitResult = xEventGroupWaitBits(ibm_mqtt_event, IBM_MQTT_CONNECTED_EVENT, false, false, portMAX_DELAY);

    if (waitResult == pdFALSE) {
        IBMIOT_INFO_PRINT("Failed to connect.");
        return false;
    }

    if (eventCallback) {
        eventCallback(IBM_MQTT_CONNECTED);
    }

    return true;
}

void IBMIoTMqttClient::runAsync(void* taskData) {
    reconnect();

    while (true) {
        IBMIoTPayload payload;
        if (xQueueReceive(eventQueue, &payload, 100 / portTICK_PERIOD_MS) == pdTRUE) {
            std::stringstream topic;
            topic << "iot-2/evt/" << payload.event << "/fmt/json";

            IBMIOT_INFO_PRINT("Topic: %s", topic.str().c_str());
            IBMIOT_INFO_PRINT("Message: %s. Length: %d", payload.message, payload.length);

            mqtt_publish(mqttcli, topic.str().c_str(), (const char*)payload.message, payload.length, 0, 0);
        }

        EventBits_t waitBits = IBM_MQTT_CONNECTED_EVENT | IBM_MQTT_DISCONNECTED_EVENT | IBM_MQTT_RECONNECT_EVENT;
        BaseType_t waitResult = xEventGroupWaitBits(ibm_mqtt_event, IBM_MQTT_CONNECTED_EVENT | IBM_MQTT_DISCONNECTED_EVENT, false, false, 100 / portTICK_PERIOD_MS);

        if (waitResult == pdTRUE) {
            EventBits_t bits = xEventGroupGetBits(ibm_mqtt_event);

            if (bits & IBM_MQTT_DISCONNECTED_EVENT) {
                if (eventCallback) {
                    eventCallback(IBM_MQTT_DISCONNECTED);
                }
            }

            if (bits & IBM_MQTT_RECONNECT_EVENT) {
                if (eventCallback) {
                    eventCallback(IBM_MQTT_RECONNECT);
                }
            }
        }
    }
}

bool IBMIoTMqttClient::addToQueue(IBMIoTPayload* payload) {
    return xQueueSendToBack(eventQueue, payload, 1000 / portTICK_PERIOD_MS) == pdTRUE;
}

bool IBMIoTMqttClient::publishEventJson(const char* event, char* message) {
    IBMIoTPayload payload;
    payload.event = event;
    payload.message = message;
    payload.length = strlen(message);

    return addToQueue(&payload);
}

bool IBMIoTMqttClient::publishEventJson(const char* event, std::string message) {
    IBMIoTPayload payload;
    payload.event = event;
    payload.message = (char*)message.c_str();
    payload.length = message.length();

    return addToQueue(&payload);
}

bool IBMIoTMqttClient::publishEventJson(const char* event, String message) {
    IBMIoTPayload payload;
    payload.event = event;
    payload.message = (char*)message.c_str();
    payload.length = message.length();

    return addToQueue(&payload);
}
