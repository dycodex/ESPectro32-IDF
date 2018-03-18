/*
 * test_wait_wifi.hpp
 *
 *  Created on: Oct 18, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_WAIT_WIFI_HPP_
#define MAIN_EXPLORE_TEST_WAIT_WIFI_HPP_

#include <Arduino.h>
#include <esp_log.h>

#include <ESPectro32_Board.h>

#include <WiFiManager.h>
WiFiManager wifiMgr;

#include <ESPectro32_LedMatrix_Animation.h>
//ESPectro32_LedMatrix_Animation ledMatrixAnim;

#include "led_matrix_frames.h"
#include "../Constants.h"

static bool test_wait_wifi() {

//    ledMatrixAnim.setLedMatrix(ESPectro32.LedMatrix());
//    //	ledMatrixTextAnim.setLedMatrix(ESPectro32.LedMatrix());
//
//    	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_1);
//    	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_2);
//    	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_3);
//    	ledMatrixAnim.addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
//    		ledM.clear();
//    	});
//
//    	ledMatrixAnim.start(1800, true);

	ESPectro32_LedMatrix_Animation ledMatrixAnim = ESPectro32_LedMatrix_Animation::startWiFiAnimation();

//    	wifiMgr.onWiFiConnected([](bool newConn, ) {
//    		ESP_LOGI("WIFI", "IP: %s", wifiMgr.getStationIpAddress().c_str());
//    	});

    	wifiMgr.begin(WIFI_MODE_STA, false);
    	wifiMgr.connectToAP(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);
    	//wifiMgr.connectToAP("GERES10", "p@ssw0rd");

    	//Actually start
    	wifiMgr.start();

    	if (wifiMgr.waitForConnection()) {
    		ledMatrixAnim.stop();
    		ESPectro32.LedMatrix().displayFrame(2);

    		return true;
    	}

    	return false;
}

#endif /* MAIN_EXPLORE_TEST_WAIT_WIFI_HPP_ */
