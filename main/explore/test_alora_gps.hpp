/*
 * TestAlora.hpp
 *
 *  Created on: Oct 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTALORA_HPP_
#define MAIN_EXPLORE_TESTALORA_HPP_

#include <Arduino.h>
#include <AloraSensorKit.h>
#include <AloraUtils.h>

#include "test_wait_wifi.hpp"

#include <ESPectro32_LedMatrix_Animation.h>
ESPectro32_LedMatrix_ScrollTextAnimation ledMatrixTextAnim;

// include the correct UART interface header
// on ESP32, it should be HardwareSerial.h
#include <HardwareSerial.h>

// Create UART interface object for GPS
HardwareSerial gpsSerial(1);

// RX & TX pin for GPS (with jumper on UART1 on Alora board)
// on ESPectro32 it should be IO12 and IO14
#define GPS_RX 14
#define GPS_TX 12

AloraSensorKit sensorKit;
uint32_t lastQueryMillis = 0;
double avgLux = 0;

#define DEBUG_STREAM Serial
#define IS_CASE_OPEN(lux, thres) (lux > thres)

#include <MakestroCloudClient32.h>

#define DEVICE_ID "123456789"
MakestroCloudClient32 client(DEFAULT_MAKESTRO_USER, DEFAULT_MAKESTRO_KEY, "logistics", DEVICE_ID);

void gpsFixAvailable(gps_fix &gpsFix) {
    if (gpsFix.valid.location) { //make sure

		printLoc(DEBUG_STREAM, gpsFix.latitudeL()); // prints int like a float
		DEBUG_STREAM.print(',');
		// DEBUG_PORT.print( fix.longitude(), 6 ); // floating-point display
		// DEBUG_PORT.print( fix.longitudeL() );  // integer display
		printLoc(DEBUG_STREAM, gpsFix.longitudeL()); // prints int like a float
		DEBUG_STREAM.println();
    }
}

volatile bool lightInterrupted = false;
void sxInterrupted() {
	//Serial.println("Interrupted!");
	lightInterrupted = true;
}

uint32_t lastPublishMillis = 0;
static void publish_alert_to_cloud(bool opened, uint16_t interval) {
	if (millis() - lastPublishMillis > interval) {
		sensorKit.run();

		// get sensor values
		SensorValues sensorData = sensorKit.getLastSensorData();

		// sensorData.T1 and sensorData.T2 are temperature sensor data
		Serial.printf("[SENSOR] Temp: %.2f, Hum: %.2f, Light: %.2f\n", sensorData.T1, sensorData.H1, sensorData.lux);

		String latStr, lonStr;
		printLocToString(latStr, sensorData.gpsFix.latitudeL());
		printLocToString(lonStr, sensorData.gpsFix.longitudeL());

		char payloadStr[256];
		//sprintf(payloadStr, "T1=%s&P=%s&H1=%s&AX=%s&AY=%s&AZ=%s&GX=%s&GY=%s&GZ=%s&MX=%s&MY=%s&MZ=%s&MH=%s&T2=%s&H2=%s&L=%s&S=%d&G=%d&B=%d",
		sprintf(payloadStr, "{\"alert\":%d, \"thresh\": %.2f, \"T1\":%.2f,\"P\":%.2f,\"H1\":%.2f,\"LAT\":%s,\"LON\":%s,\"MH\":%.2f,\"T2\":%.2f,\"H2\":%.2f,\"L\":%.2f,\"G\":%d,\"CO2\":%d,\"device_id\":\"%s\"}",
				opened? 1: 0,
				avgLux,
				sensorData.T1, sensorData.P, sensorData.H1,
				latStr.c_str(), lonStr.c_str(),
				sensorData.magHeading,
				sensorData.T2, sensorData.H2,
				sensorData.lux,
				sensorData.gas,
				sensorData.co2,
				DEVICE_ID);

		client.publishData(payloadStr);
		Serial.println(payloadStr);

		lastPublishMillis = millis();
	}
	else {
		//Serial.println("Publish ignored");
	}
}

static void test_alora_gps() {

    Wire.begin();

    ledMatrixTextAnim.setLedMatrix(ESPectro32.LedMatrix());

    test_wait_wifi();

    //Makestro
    client.subscribeProperty("warning", [](String prop, String val) {
		ESP_LOGI("MQTT", ">> Prop: %s. Val: %s", prop.c_str(), val.c_str());
		if (val.equals("1") || val.equals("true")) {
			ESP_LOGI("MQTT", "Display text");
			ledMatrixTextAnim.scrollText("Please close!", 6000);
		}
		else {
			ledMatrixTextAnim.stop();
		}
	});

    	client.begin();
	client.start();

//    pinMode(15, INPUT);
//    attachInterrupt(digitalPinToInterrupt(15), sxInterrupted, CHANGE);

    sensorKit.begin();
    sensorKit.setSensingInterval(500);

    sensorKit.scanAndPrintI2C(Serial);
    delay(500);

    //sensorKit.GPIOExpander().pinMode(3, INPUT);
    sensorKit.GPIOExpander().enableInterrupt(3, CHANGE);

    // initialize UART interface for GPS Serial, on ESPectro32 it should be:
	gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
	sensorKit.initGPS(&gpsSerial);
	sensorKit.onGpsFixAvailableCallback(gpsFixAvailable);

	delay(500);
	Serial.println("Sampling LUX");
	//Sampling lux environment
	double luxTotal = 0;
	for(int i = 0; i < 64; i++) {
		double readLux = 0;
		sensorKit.readTSL2591(readLux);
		if (readLux >= 65400) {
			continue;
		}
		luxTotal += readLux;
	}
	avgLux = luxTotal/64;
	Serial.printf("Avg LUX = %.2f\n", avgLux);

	for (;;) {
		// read all sensors
//		sensorKit.runGPS();
		sensorKit.run();

//		if (lightInterrupted) {
//			lightInterrupted = false;
//
//			Serial.print("[Light interrupted] Source: ");
//			Serial.println(sensorKit.GPIOExpander().interruptSource(true), BIN);
//		}

		// query the data every 1 seconds
		if (millis() - lastQueryMillis >= 500) {

//			if (sensorKit.GPIOExpander().checkInterrupt(3)) {
//				Serial.print("[Light interrupted] Source: ");
//				Serial.printf("0x%x\n", sensorKit.GPIOExpander().interruptSource(true));
//			}

			// get sensor values
			SensorValues sensorData = sensorKit.getLastSensorData();
			if (sensorData.lux > 65400) {
				continue;
			}

			//Serial.printf("Temp: %.2f, Hum: %.2f, Light: %.2f\n", sensorData.T1, sensorData.H1, sensorData.lux);
			if (IS_CASE_OPEN(sensorData.lux, avgLux*2)) {
				Serial.println("TRIGERRED!");
				publish_alert_to_cloud(true, 2000);
				lightInterrupted = true;
			}
			else {
				if (lightInterrupted) {
					lightInterrupted = false;
					publish_alert_to_cloud(false, 1000);
				}
				else {
					publish_alert_to_cloud(false, 8000);
				}
			}

//			// trace GPS fix information
//			Serial.print("[GPS FIX] ");
//			trace_all(Serial, *(sensorKit.getGPSObject()), sensorData.gpsFix);

			// update time tracker
			lastQueryMillis = millis();

		}

//		while (gpsSerial.available()) {
//			Serial.write(gpsSerial.read());
//		}
	}
}
#endif /* MAIN_EXPLORE_TESTALORA_HPP_ */
