ESPectro32-IDF
==============

This is a getting started project for ESPectro32 development board made with `Espressif IoT Development Framework` ([ESP-IDF](https://github.com/espressif/esp-idf)). Please check ESP-IDF docs for getting started instructions.

This project depends on [ESPectro32](https://github.com/dycodex/ESPectro32) library!

## ESPectro32

[ESPectro32](https://shop.makestro.com/product/espectro32/) is an IoT development board based-on ESP32 MCU, made by [DycodeX](https://dycodex.com).

It's official development board for Republic of IoT ([RIoT](https://r-iot.id)) hackathon. Made possible by awesome collaboration of two countries' IoT and maker communities: [Makestro](https://makestro.com) (Indonesia) and [MyIoTC](http://myiotc.com) (Malaysia).

(...more details to go...)

## Getting Started

* Clone this repo
* Update submodules which it depends on: `git submodule update --init --recursive` 
* There's a symbolic link `arduino-esp32`. Change (recreate) the symbolic link to point to your local [arduino-esp32](https://github.com/espressif/arduino-esp32) folder.
* Try to `make`. Fingers crossed :)
