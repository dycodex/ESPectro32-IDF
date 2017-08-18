/*
 * Constants.h
 *
 *  Created on: Jun 10, 2017
 *      Author: andri
 */

#ifndef MAIN_CONSTANTS_H_
#define MAIN_CONSTANTS_H_


#define INIT_SETTING                 	0

#define SENSOR_QUERY_INTERVAL 3000
#define IGNORE_LEVEL_PERCENTAGE_DIFF    30 //30%
#define RESPOND_WATERLEVEL_INTERVAL     1.6*SENSOR_QUERY_INTERVAL  //how soon water level change affect display on ledbar


#define DEFAULT_WIFI_SSID           "DyWare-AP3"
#define DEFAULT_WIFI_PASSWORD       "p@ssw0rd"

//#define WIFI_SSID "HAME_F1_0e93"
//#define WPA_PASSWORD "7cd0e938"

//#define WIFI_SSID "Andri's iPhone 6s"
//#define WPA_PASSWORD "11223344"

#define DEFAULT_DEVICE_ID           "x61110a6-4d0a-4382-bb73-534204fef25z"
#define DEFAULT_DEVICE_NAME         "My Smart Gallon"
#define DEFAULT_DEVICE_TYPES        "SmartGallon"
#define DEFAULT_DEVICE_SUBTYPES     "Generic"

#define MQTT_SERVER                 "home-x.cloudapp.net"//"iot.eclipse.org"//"home-x.cloudapp.net"
#define MQTT_PORT                   1883

#define DEFAULT_AP_NAME             "homeX-gallon-02"
#define DEFAULT_AP_PASS             "11223344"
#define DEFAULT_IP_ADDR             "192.168.9.1"

#endif /* MAIN_CONSTANTS_H_ */
