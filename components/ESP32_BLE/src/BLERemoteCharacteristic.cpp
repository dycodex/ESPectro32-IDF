/*
 * BLERemoteCharacteristic.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: kolban
 */

#include "BLERemoteCharacteristic.h"

#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include <esp_gattc_api.h>
#include <esp_log.h>
#include <esp_err.h>

#include <sstream>
#include "BLEUtils.h"
#include "GeneralUtils.h"


static const char* LOG_TAG = "BLERemoteCharacteristic";

BLERemoteCharacteristic::BLERemoteCharacteristic(
		esp_gatt_id_t        charId,
		esp_gatt_char_prop_t charProp,
		BLERemoteService*    pRemoteService) {
	m_charId         = charId;
	m_charProp       = charProp;
	m_pRemoteService = pRemoteService;
	m_notifyCallback = nullptr;
} // BLERemoteCharacteristic


static bool compareSrvcId(esp_gatt_srvc_id_t id1, esp_gatt_srvc_id_t id2) {
	if (id1.id.inst_id != id2.id.inst_id) {
		return false;
	}
	if (!BLEUUID(id1.id.uuid).equals(BLEUUID(id2.id.uuid))) {
		return false;
	}
	return true;
} // compareSrvcId


static bool compareGattId(esp_gatt_id_t id1, esp_gatt_id_t id2) {
	if (id1.inst_id != id2.inst_id) {
		return false;
	}
	if (!BLEUUID(id1.uuid).equals(BLEUUID(id2.uuid))) {
		return false;
	}
	return true;
} // compareCharId


/**
 * @brief Handle GATT Client events.
 * When an event arrives for a GATT client we give this characteristic the opportunity to
 * take a look at it to see if there is interest in it.
 * @param [in] event The type of event.
 * @param [in] gattc_if The interface on which the event was received.
 * @param [in] evtParam Payload data for the event.
 * @returns N/A
 */
void BLERemoteCharacteristic::gattClientEventHandler(
	esp_gattc_cb_event_t      event,
	esp_gatt_if_t             gattc_if,
	esp_ble_gattc_cb_param_t* evtParam) {
	switch(event) {
		//
		// ESP_GATTC_READ_CHAR_EVT
		// This event indicates that the server has responded to the read request.
		//
		// read:
		// esp_gatt_status_t  status
		// uint16_t           conn_id
		// esp_gatt_srvc_id_t srvc_id
		// esp_gatt_id_t      char_id
		// esp_gatt_id_t      descr_id
		// uint8_t*           value
		// uint16_t           value_type
		// uint16_t           value_len
		case ESP_GATTC_READ_CHAR_EVT: {
			if (compareSrvcId(evtParam->read.srvc_id, *m_pRemoteService->getSrvcId()) == false) {
				break;
			}

			if (evtParam->read.conn_id != m_pRemoteService->getClient()->getConnId()) {
				break;
			}

			if (compareGattId(evtParam->read.char_id, m_charId) == false) {
				break;
			}

			if (evtParam->read.status == ESP_GATT_OK) {
				m_value = std::string((char*)evtParam->read.value, evtParam->read.value_len);
			} else {
				m_value = "";
			}

			m_semaphoreReadCharEvt.give();
			break;
		} // ESP_GATTC_READ_CHAR_EVT


		//
		// ESP_GATTC_REG_FOR_NOTIFY_EVT
		//
		// reg_for_notify:
		// - esp_gatt_status_t status
		// - esp_gatt_srvc_id_t srvc_id
		// - esp_gatt_id_t char_id
		case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
			if (compareSrvcId(evtParam->reg_for_notify.srvc_id, *m_pRemoteService->getSrvcId()) == false) {
				break;
			}
			if (compareGattId(evtParam->reg_for_notify.char_id, m_charId) == false) {
				break;
			}
			m_semaphoreRegForNotifyEvt.give();
			break;
		} // ESP_GATTC_REG_FOR_NOTIFY_EVT


		//
		// ESP_GATTC_WRITE_CHAR_EVT
		//
		// write:
		// esp_gatt_status_t  status
		// uint16_t           conn_id
		// esp_gatt_srvc_id_t srvc_id
		// esp_gatt_id_t      char_id
		// esp_gatt_id_t      descr_id
		case ESP_GATTC_WRITE_CHAR_EVT: {
			if (compareSrvcId(evtParam->write.srvc_id, *m_pRemoteService->getSrvcId()) == false) {
				break;
			}
			if (evtParam->write.conn_id != m_pRemoteService->getClient()->getConnId()) {
				break;
			}
			if (compareGattId(evtParam->write.char_id, m_charId) == false) {
				break;
			}
			m_semaphoreWriteCharEvt.give();
			break;
		} // ESP_GATTC_WRITE_CHAR_EVT


		default: {
			break;
		}
	}
}; // gattClientEventHandler


BLEUUID BLERemoteCharacteristic::getUUID() {
	return BLEUUID(m_charId.uuid);
}

/**
 * @brief Read an unsigned 16 bit value
 * @return The unsigned 16 bit value.
 */
uint16_t BLERemoteCharacteristic::readUInt16(void) {
	std::string value = readValue();
	if (value.length() >= 2) {
		return *(uint16_t*)(value.data());
	}
	return 0;
} // readUInt16


/**
 * @brief Read an unsigned 32 bit value.
 * @return the unsigned 32 bit value.
 */
uint32_t BLERemoteCharacteristic::readUInt32(void) {
	std::string value = readValue();
	if (value.length() >= 4) {
		return *(uint32_t*)(value.data());
	}
	return 0;
} // readUInt32


/**
 * @brief Read a byte value
 * @return The value as a byte
 */
uint8_t BLERemoteCharacteristic::readUInt8(void) {
	std::string value = readValue();
	if (value.length() >= 1) {
		return (uint8_t)value[0];
	}
	return 0;
} // readUInt8


/**
 * @brief Read the value of the remote characteristic.
 * @return The value of the remote characteristic.
 */
std::string BLERemoteCharacteristic::readValue() {
	ESP_LOGD(LOG_TAG, ">> readValue()");

	m_semaphoreReadCharEvt.take("readValue");

	// Ask the BLE subsystem to retrieve the value for the remote hosted characteristic.
	esp_err_t errRc = ::esp_ble_gattc_read_char(
		m_pRemoteService->getClient()->getGattcIf(),
		m_pRemoteService->getClient()->getConnId(),
		m_pRemoteService->getSrvcId(),
		&m_charId,
		ESP_GATT_AUTH_REQ_NONE);

	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_ble_gattc_read_char: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return "";
	}

	// Block waiting for the event that indicates that the read has completed.  When it has, the std::string found
	// in m_value will contain our data.
	m_semaphoreReadCharEvt.wait("readValue");

	ESP_LOGD(LOG_TAG, "<< readValue()");
	return m_value;
} // readValue


/**
 * @brief Register for notifications.
 * @param [in] notifyCallback A callback to be invoked for a notification.
 * @return N/A.
 */
void BLERemoteCharacteristic::registerForNotify(
		void (*notifyCallback)(
			BLERemoteCharacteristic* pBLERemoteCharacteristic,
			uint8_t* pData,
			size_t length,
			bool isNotify)) {
	ESP_LOGD(LOG_TAG, ">> registerForNotify()");

	m_notifyCallback = notifyCallback; // Save the notification callback.

	m_semaphoreRegForNotifyEvt.take("registerForNotify");

	esp_err_t errRc = ::esp_ble_gattc_register_for_notify(
		m_pRemoteService->getClient()->getGattcIf(),
		*m_pRemoteService->getClient()->getPeerAddress().getNative(),
		m_pRemoteService->getSrvcId(),
		&m_charId);

	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_ble_gattc_register_for_notify: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

	m_semaphoreRegForNotifyEvt.wait("registerForNotify");

	ESP_LOGD(LOG_TAG, "<< registerForNotify()");
} // registerForNotify


/**
 * @brief Convert a BLERemoteCharacteristic to a string representation;
 * @return a String representation.
 */
std::string BLERemoteCharacteristic::toString() {
	std::ostringstream ss;
	ss << "Characteristic: uuid: " << BLEUUID(m_charId.uuid).toString() <<
		", props: " << BLEUtils::characteristicPropertiesToString(m_charProp) <<
		", inst_id: " << (int)m_charId.inst_id;
	return ss.str();
} // toString


/**
 * @brief Write the new value for the characteristic.
 * @param [in] newValue The new value to write.
 * @param [in] response Do we expect a response?
 * @return N/A.
 */
void BLERemoteCharacteristic::writeValue(std::string newValue, bool response) {
	ESP_LOGD(LOG_TAG, ">> writeValue(), length: %d", newValue.length());

	m_semaphoreWriteCharEvt.take("writeValue");

	esp_err_t errRc = ::esp_ble_gattc_write_char(
		m_pRemoteService->getClient()->getGattcIf(),
		m_pRemoteService->getClient()->getConnId(),
		m_pRemoteService->getSrvcId(),
		&m_charId,
		newValue.length(),
		(uint8_t*)newValue.data(),
		response?ESP_GATT_WRITE_TYPE_RSP:ESP_GATT_WRITE_TYPE_NO_RSP,
		ESP_GATT_AUTH_REQ_NONE
	);

	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "esp_ble_gattc_write_char: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}

	m_semaphoreWriteCharEvt.wait("writeValue");

	ESP_LOGD(LOG_TAG, "<< writeValue");
} // writeValue


/**
 * @brief Write the new value for the characteristic.
 *
 * This is a convenience function.  Many BLE characteristics are a single byte of data.
 * @param [in] newValue The new byte value to write.
 * @param [in] response Whether we require a response from the write.
 * @return N/A.
 */
void BLERemoteCharacteristic::writeValue(uint8_t newValue, bool response) {
	writeValue(std::string(reinterpret_cast<char*>(&newValue), 1), response);
} // writeValue


/**
 * @brief Write the new value for the characteristic from a data buffer.
 * @param [in] data A pointer to a data buffer.
 * @param [in] length The length of the data in the data buffer.
 * @param [in] response Whether we require a response from the write.
 */
void BLERemoteCharacteristic::writeValue(uint8_t* data, size_t length, bool response) {
	writeValue(std::string((char *)data, length), response);
} // writeValue


#endif /* CONFIG_BT_ENABLED */
