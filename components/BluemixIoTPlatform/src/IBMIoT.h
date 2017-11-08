#ifndef IBM_IOT_PLATFORM_H
#define IBM_IOT_PLATFORM_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include <Task.h>
#include <esp_log.h>
#include <string>
#include <sstream>
#include <Arduino.h>
#undef max
#undef min
#include <functional>

extern "C" {
    #include "mqtt.h"
}

#define IBMIOT_INFO_PRINT(...) ESP_LOGI("IBM IoT", __VA_ARGS__);

typedef struct {
    const char* event;
    char* message;
    size_t length;
} IBMIoTPayload;

typedef struct {
    const char* orgId;
    const char* deviceId;
    const char* deviceType;
    const char* authToken;
} IBMIoTDevice;

typedef enum {
    IBM_MQTT_DISCONNECTED,
    IBM_MQTT_CONNECTED,
    IBM_MQTT_RECONNECT,
    IBM_MQTT_DATA_ARRIVED
} IBMIoTMqttEvent;

class IBMIoTMqttClient : public Task {
public:
    typedef std::function<void(IBMIoTMqttEvent)> IBMIoTMqttEventCallback;

    IBMIoTMqttClient(IBMIoTDevice& dev);
    ~IBMIoTMqttClient();

    void runAsync(void* taskData = nullptr);
    void begin();
    bool reconnect();
    bool publishEventJson(const char* event, char* message);
    bool publishEventJson(const char* event, std::string message);
    bool publishEventJson(const char* event, String message);

    void onEvent(IBMIoTMqttEventCallback callback) {
        eventCallback = callback;
    }
private:
    bool addToQueue(IBMIoTPayload* payload);

    IBMIoTMqttEventCallback eventCallback = nullptr;

    IBMIoTDevice* device;

    mqtt_client* mqttcli = nullptr;
    mqtt_settings settings = {};

    QueueHandle_t eventQueue;
};

#endif
