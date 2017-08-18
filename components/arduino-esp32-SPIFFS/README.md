# SPIFFS library for ESP32 Arduino

The SPI Flash File System (SPIFFS) is used to store files in the flash chip. This library provides a virtual file system (VFS) implementation of SPIFFS to Arduino ESP32. In a nutshell it can acts like a SDcard but using the internal flash memory.

It's a unofficial library and will be deleted when an official one be done by EspressIf.

## Installation:
Just download as zip or clone this repository to your Arduino libraries folder, [here is a detailed guide](https://www.arduino.cc/en/Guide/Libraries#toc5)

## Debug:
Anything unexpected?
Enable debug level on Arduino IDE to get a tip of what happened
![Debug](https://image.prntscr.com/image/bft4W6QdQkG3nXedftMQZw.png)

## Known Issues:
- Probably it will not work with Flash encryption.
- It doesn't support file upload (ESP8266 Sketch Data Upload tool) yet.

## Credits:
The example file is based and full compatible with [Official SD_MMC](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD_MMC) and [Official SD SPI](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD) libraries from Arduino ESP32.

VFS is a modified version of [ESP32 SPIFFS example of LoBo.](https://github.com/loboris/ESP32_spiffs_example)

Low level things come from [Lua-RTOS-ESP32 of Jaume Olivé](https://github.com/whitecatboard/Lua-RTOS-ESP32) and [SPIFFS of Peter Andersson.](https://github.com/pellepl/spiffs)
