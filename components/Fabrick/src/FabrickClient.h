/*
 * FabrickClient.h
 *
 *  Created on: Aug 11, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_FABRICK_SRC_FABRICKCLIENT_H_
#define COMPONENTS_FABRICK_SRC_FABRICKCLIENT_H_

#include <Arduino.h>
#include <esp_log.h>
#include <Stream.h>

#define FABRICK_DEBUG_PRINT(...)   	ESP_LOGD("FABRICK", __VA_ARGS__);
#define FABRICK_INFO_PRINT(...)   	ESP_LOGI("FABRICK", __VA_ARGS__);


enum FabrickClientConnectionType {
	FabrickClientConnectionLORA,
	FabrickClientConnectionWIFI
};

class FabrickLoraClientClass {
public:

	FabrickLoraClientClass();
	virtual ~FabrickLoraClientClass();
	void begin(Stream *stream);
	void send(String deviceId, int frameCtr, int dataChannel, int dataType, String data, int dataLenInHex);

	static String addZeros(String data, int len);

private:
	Stream *loraStream_ = NULL;
};

extern FabrickLoraClientClass FabrickLoraClient;
#endif /* COMPONENTS_FABRICK_SRC_FABRICKCLIENT_H_ */
