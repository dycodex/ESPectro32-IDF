#ifndef MAIN_EXAMPLES_TEST_OLED_HPP_
#define MAIN_EXAMPLES_TEST_OLED_HPP_

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <Adafruit_BME280.h>

#include <WiFiManager.h>
WiFiManager wifiMgr;

#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

static void test_oled() {
    display.begin(SH1106_SWITCHCAPVCC, 0x3C);
    display.display();
    delay(1000);
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.println("Connecting...");
    display.display();
    delay(2000);

    wifiMgr.onWiFiConnected([](bool newConn, wifi_config_t *conf) {
    		ESP_LOGI("WIFI", "SSID: %s", conf->sta.ssid);
        ESP_LOGI("WIFI", "IP Address: %s", wifiMgr.getStationIpAddress().c_str());

        display.clearDisplay();
        display.println((const char*)conf->sta.ssid);
        display.println(wifiMgr.getStationIpAddress().c_str());
        display.display();
	});

    wifiMgr.begin(WIFI_MODE_STA, false);
    wifiMgr.connectToAP("DyWare-AP3", "p@ssw0rd");

	//Actually start
	wifiMgr.start();
}

#endif
