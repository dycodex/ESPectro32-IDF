#include <Arduino.h>
#include <Wire.h>
#include <AloraSensorKit.h>

// include the correct UART interface header
// on ESP32, it should be HardwareSerial.h
#include <HardwareSerial.h>

// Create UART interface object for GPS
HardwareSerial gpsSerial(1);

// RX & TX pin for GPS (with jumper on UART1 on Alora board)
// on ESPectro32 it should be IO12 and IO14
#define GPS_RX 12
#define GPS_TX 14

AloraSensorKit sensorKit;
uint32_t lastQueryMillis = 0;

void setup() {
    // NOTE: in case the process hang right after initializing alora library, uncomment this line below:
    // Wire.begin();

    Serial.begin(9600);
    
    // initialize UART interface for GPS Serial, on ESPectro32 it should be:
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    
    sensorKit.begin();
    sensorKit.initGPS(&gpsSerial);
}

void loop() {

    // query the data every 5 seconds
    if (millis() - lastQueryMillis >= 5000) {
        // update time tracker
        lastQueryMillis = millis();
    
        // read all sensors
        sensorKit.run();

        // get sensor values
        SensorValues sensorData = sensorKit.getLastSensorData();

        // trace GPS fix information
        Serial.print("[GPS FIX] ");
        trace_all(Serial, *(sensorKit.getGPSObject()), sensorData.gpsFix);
    }
}