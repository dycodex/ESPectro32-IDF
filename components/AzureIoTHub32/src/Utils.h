//
// Created by Andri Yadi on 10/29/16.
//

#ifndef PIOMQTTAZUREIOTHUB_UTILS_H
#define PIOMQTTAZUREIOTHUB_UTILS_H

#include <string.h>
#include <string>

//http://hardwarefun.com/tutorials/url-encoding-in-arduino
std::string urlEncode(const char* msg);

// http://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string
std::string splitStringByIndex(std::string data, char separator, int index);

const char *GetValue(const char* value);
const char *GetStringValue(std::string value);


#endif //PIOMQTTAZUREIOTHUB_UTILS_H
