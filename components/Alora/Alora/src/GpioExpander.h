//
// Created by Andri Yadi on 8/1/16.
// Adapted from: https://github.com/skywodd/pcf8574_arduino_library
//

#ifndef XBOARD_BASE_GPIOEX_H
#define XBOARD_BASE_GPIOEX_H

#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <SparkFunSX1509.h>

#define GPIOEXPANDER_ADDRESS        0x3E
#define GPIOEXPANDER_BUTTON_PIN     0
#define GPIOEXPANDER_LED_PIN        14

#define IOX0                                  0
#define IOX1                                  1
#define IOX2                                  2
#define IOX3                                  3
#define IOX4                                  4
#define IOX5                                  5
#define IOX6                                  6
#define IOX7                                  7
#define IOX8                                  8
#define IOX9                                  9
#define IOX10                                 10
#define IOX11                                 11
#define IOX12                                 12
#define IOX13                                 13
#define IOX14                                 14
#define IOX15                                 15


class GpioExpander: public SX1509 {
public:
    GpioExpander(uint8_t address = GPIOEXPANDER_ADDRESS);
    ~GpioExpander();

    //byte begin();
    byte begin(byte address = GPIOEXPANDER_ADDRESS, byte resetPin = 0xFF);
    void turnOnLED();
    void turnOffLED();
    void blinkLED(unsigned long tOn, unsigned long tOff, byte onIntensity = 255, byte offIntensity = 0);

private:
    byte i2cDeviceAddress_;
};


#endif //XBOARD_BASE_GPIOEX_H
