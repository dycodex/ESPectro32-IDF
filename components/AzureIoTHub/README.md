# AzureIoTHubMQTTClient

It's unofficial Azure IoT Hub client library for ESP32, build with ESP-IDF framework that may be compatible with Arduino framework. It's similar effort as what I've done for ESP8266 as found [here](https://github.com/andriyadi/AzureIoTHubMQTTClient).

I try to write the client library to be as familiar as possible. I name the method sendEvent instead of publish, so it's similar as another Azure IoT Hub client library. If you're familiar with MQTT, you know you should use topic in order to publish payload to MQTT broker. But since the MQTT topic used to publish message to Azure IoT Hub cannot be arbitrary, library sets it for you.

## Why?
You know how hard/cryptic Azure IoT Hub client library for Arduino (and ESP8266) is, right? Well, at least to me. It's written in C (remember [the song](https://www.youtube.com/watch?v=wJ81MZUlrDo)?).

Hey, Azure IoT Hub already supports MQTT protocol since the GA, doesn't it? Why don't we just access it using MQTT? Yes, you can, go ahead and deal with that SAS token :)

I took a liberty to write an easy to use class `AzureIoTHubMQTTClient`, all you need to provide are:

* Azure IoT Hub name --> the name you use to create the IoT Hub
* Device ID --> the ID you use to register a device, either via `DeviceExplorer` or `iothub-explorer`
* Device Key --> the primary key that you can get using DeviceExplorer app or using `iothub-explorer get "[device_id]" --connection-string`

## Dependencies

* [esp32-mqtt](https://github.com/tuanpmt/esp32-mqtt)
* [Task](https://github.com/dycodex/ESPectro32/tree/master/lib/FreeRTOS-Wrapper)

## Getting Started
Take a look the examples
