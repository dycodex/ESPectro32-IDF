/*
 * BLE.cpp
 *
 *  Created on: Mar 16, 2017
 *      Author: kolban
 */
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <bt.h>              // ESP32 BLE
#include <esp_bt_main.h>     // ESP32 BLE
#include <esp_gap_ble_api.h> // ESP32 BLE
// ESP32 BLE
#include <esp_gatts_api.h>   // ESP32 BLE
#include <esp_err.h>         // ESP32 ESP-IDF
#include <esp_log.h>         // ESP32 ESP-IDF
#include <map>               // Part of C++ STL
#include <sstream>
#include <iomanip>

#include "BLEDevice.h"
#include "BLEClient.h"
#include "BLEUtils.h"
#include "GeneralUtils.h"

static const char* LOG_TAG = "BLEDevice";

BLEServer *BLEDevice::m_bleServer = nullptr;
BLEScan   *BLEDevice::m_pScan     = nullptr;
BLEClient *BLEDevice::m_pClient   = nullptr;

#include <esp_gattc_api.h>


BLEClient* BLEDevice::createClient() {
	m_pClient = new BLEClient();
	return m_pClient;
} // createClient


/**
 * @brief Handle GATT server events.
 *
 * @param [in] event
 * @param [in] gatts_if
 * @param [in] param
 */
void BLEDevice::gattServerEventHandler(
   esp_gatts_cb_event_t      event,
   esp_gatt_if_t             gatts_if,
   esp_ble_gatts_cb_param_t *param
) {
	ESP_LOGD(LOG_TAG, "gattServerEventHandler [esp_gatt_if: %d] ... %s",
		gatts_if,
		BLEUtils::gattServerEventTypeToString(event).c_str());
	BLEUtils::dumpGattServerEvent(event, gatts_if, param);
	if (BLEDevice::m_bleServer != nullptr) {
		BLEDevice::m_bleServer->handleGATTServerEvent(event, gatts_if, param);
	}
} // gattServerEventHandler


/**
 * @brief Handle GATT client events.
 *
 * Handler for the GATT client events.
 * * `ESP_GATTC_OPEN_EVT` – Invoked when a connection is opened.
 * * `ESP_GATTC_PREP_WRITE_EVT` – Response to write a characteristic.
 * * `ESP_GATTC_READ_CHAR_EVT` – Response to read a characteristic.
 * * `ESP_GATTC_REG_EVT` – Invoked when a GATT client has been registered.
 *
 * @param [in] event
 * @param [in] gattc_if
 * @param [in] param
 */
void BLEDevice::gattClientEventHandler(
	esp_gattc_cb_event_t event,
	esp_gatt_if_t gattc_if,
	esp_ble_gattc_cb_param_t *param) {

	ESP_LOGD(LOG_TAG, "gattClientEventHandler [esp_gatt_if: %d] ... %s",
		gattc_if, BLEUtils::gattClientEventTypeToString(event).c_str());
	BLEUtils::dumpGattClientEvent(event, gattc_if, param);

	switch(event) {
		default: {
			break;
		}
	} // switch

	// If we have a client registered, call it.
	if (BLEDevice::m_pClient != nullptr) {
		BLEDevice::m_pClient->gattClientEventHandler(event, gattc_if, param);
	}

} // gattClientEventHandler


/**
 * @brief Handle GAP events.
 */
void BLEDevice::gapEventHandler(
	esp_gap_ble_cb_event_t event,
	esp_ble_gap_cb_param_t *param) {

	BLEUtils::dumpGapEvent(event, param);

	switch(event) {
		case ESP_GAP_BLE_SEC_REQ_EVT: {
			esp_err_t errRc = ::esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
			if (errRc != ESP_OK) {
				ESP_LOGE(LOG_TAG, "esp_ble_gap_security_rsp: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
			}
			break;
		}

		default: {
			break;
		}
	} // switch

	if (BLEDevice::m_bleServer != nullptr) {
		BLEDevice::m_bleServer->handleGAPEvent(event, param);
	}

	if (BLEDevice::m_pScan != nullptr) {
		BLEDevice::getScan()->gapEventHandler(event, param);
	}
} // gapEventHandler


/**
 * @brief Initialize the %BLE environment.
 * @param deviceName The device name of the device.
 */
void BLEDevice::init(std::string deviceName) {
	esp_err_t errRc = ::nvs_flash_init();
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "nvs_flash_init: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  errRc = esp_bt_controller_init(&bt_cfg);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_bt_controller_init: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

	errRc = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_bt_controller_enable: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

	errRc = esp_bluedroid_init();
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_bluedroid_init: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

	errRc = esp_bluedroid_enable();
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_bluedroid_enable: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

	errRc = esp_ble_gap_register_callback(BLEDevice::gapEventHandler);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_ble_gap_register_callback: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

	errRc = esp_ble_gattc_register_callback(BLEDevice::gattClientEventHandler);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_ble_gattc_register_callback: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

	errRc = esp_ble_gatts_register_callback(BLEDevice::gattServerEventHandler);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_ble_gatts_register_callback: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

	errRc = ::esp_ble_gap_set_device_name(deviceName.c_str());
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_ble_gap_set_device_name: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	};

	esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;
	errRc = ::esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_ble_gap_set_security_param: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	};

	vTaskDelay(200/portTICK_PERIOD_MS); // Delay for 200 msecs as a workaround to an apparent Arduino environment issue.
} // init



/**
 * @brief Retrieve the Scan object that we use for scanning.
 * @return The scanning object reference.
 */
BLEScan* BLEDevice::getScan() {
	if (m_pScan == nullptr) {
		m_pScan = new BLEScan();
	}
	return m_pScan;
} // getScan



#endif // CONFIG_BT_ENABLED
