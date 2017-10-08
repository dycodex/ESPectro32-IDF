//
// Created by Andri Yadi on 8/1/16.
//

#include "GpioExpander.h"

GpioExpander::GpioExpander(uint8_t address):
SX1509(address), i2cDeviceAddress_(address)
{
}

GpioExpander::~GpioExpander() {

}

byte GpioExpander::begin(byte address, byte resetPin) {

    byte addr = (address != GPIOEXPANDER_ADDRESS)? address: i2cDeviceAddress_;
    byte res = SX1509::begin(addr, resetPin);
    if (res) {
        //SX1508::pinMode(GPIOEXPANDER_LED_PIN, OUTPUT);
        clock(INTERNAL_CLOCK_2MHZ, 4);
    }

    return res;
}

void GpioExpander::turnOnLED() {
	SX1509::digitalWrite(GPIOEXPANDER_LED_PIN, LOW);
}

void GpioExpander::turnOffLED() {
	SX1509::digitalWrite(GPIOEXPANDER_LED_PIN, HIGH);
}

void GpioExpander::blinkLED(unsigned long tOn, unsigned long tOff, byte onIntensity, byte offIntensity) {
    blink(GPIOEXPANDER_LED_PIN, tOn, tOff, onIntensity, offIntensity);
}
