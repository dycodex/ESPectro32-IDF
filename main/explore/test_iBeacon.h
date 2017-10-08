/*
 * TestIBeacon.h
 *
 *  Created on: Sep 15, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTIBEACON_H_
#define MAIN_EXPLORE_TESTIBEACON_H_


#if defined(__cplusplus)
extern "C" {
#endif

// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "bt.h"
#include "bta_api.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_main.h"

#define LOG_INFO(...) 	ESP_LOGI("BEACON", __VA_ARGS__);
#define LOG_ERROR(...) 	ESP_LOGE("BEACON", __VA_ARGS__);

#define GATTS_SERVICE_UUID_TEST     0x00FF
#define TEST_DEVICE_NAME            "ESP_GATTS_DEMO"

#define TEST_MANUFACTURER_DATA_LEN  31

static uint8_t test_manufacturer[TEST_MANUFACTURER_DATA_LEN] = {
	2,
	0x01, 0x04,
	26,
	0xFF,
    0x4C, 0x00,     // # Company identifier code (0x004C == Apple)
    0x02,           // # Byte 0 of iBeacon advertisement indicator
    21,           // # Byte 1 of iBeacon advertisement indicator
	0x11,
    0xe2, 0xc5, 0x6d, 0xb5, 0xdf, 0xfb, 0x48, 0xd2, 0xb0, 0x60, 0xd0, 0xf5, 0xa7, 0x10, 0x96, 0xe0, //# iBeacon proximity uuid
    0x00, 0x20,     // # major
    0x21, 0x22,     // # minor
	0xA0
};

static esp_ble_adv_data_t test_adv_data = {
    .set_scan_rsp = false,
    .include_name = false,
    .include_txpower = false,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .manufacturer_len = TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = &test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = 0x2,
};

static esp_ble_adv_params_t test_adv_params = {
    256,
    256,
	ADV_TYPE_IND,
	BLE_ADDR_TYPE_PUBLIC,
    0,
	0,
    ADV_CHNL_ALL,
    ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    LOG_ERROR("GAP_EVT, event %d\n", event);

    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&test_adv_params);
        break;
    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_ble_gatts_cb_param_t *p = (esp_ble_gatts_cb_param_t *)param;

    switch (event) {
    case ESP_GATTS_REG_EVT:
        LOG_INFO("REGISTER_APP_EVT, status %d, app_id %d\n", p->reg.status, p->reg.app_id);

        esp_ble_gap_set_device_name(TEST_DEVICE_NAME);
        esp_ble_gap_config_adv_data(&test_adv_data);
        break;
    case ESP_GATTS_READ_EVT: {
        break;
    }
    case ESP_GATTS_WRITE_EVT: {
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
    case ESP_GATTS_MTU_EVT:
    case ESP_GATTS_CONF_EVT:
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        LOG_INFO("CREATE_SERVICE_EVT, status %d, service_handle %d\n", p->create.status, p->create.service_handle);
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        LOG_INFO("ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                 p->add_char.status, p->add_char.attr_handle, p->add_char.service_handle);
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        LOG_INFO("SERVICE_START_EVT, status %d, service_handle %d\n",
                 p->start.status, p->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        break;
    case ESP_GATTS_DISCONNECT_EVT:
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}

void test_ibeacon()
{
    esp_err_t ret;

    ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    LOG_INFO("%s init bluetooth\n", __func__);
    if (ret) {
        LOG_ERROR("%s init bluetooth failed\n", __func__);
        return;
    }
    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret) {
        LOG_ERROR("%s enable bluetooth failed\n", __func__);
        return;
    }

    ret = esp_bluedroid_init();
	if (ret) {
		LOG_ERROR("%s init bluetooth failed\n", __func__);
		return;
	}
	ret = esp_bluedroid_enable();
	if (ret) {
		LOG_ERROR("%s enable bluetooth failed\n", __func__);
		return;
	}

    esp_ble_gatts_register_callback(gatts_event_handler);
    esp_ble_gap_register_callback(gap_event_handler);
    esp_ble_gatts_app_register(GATTS_SERVICE_UUID_TEST);

    return;
}

#if defined(__cplusplus)
}
#endif

#endif /* MAIN_EXPLORE_TESTIBEACON_H_ */
