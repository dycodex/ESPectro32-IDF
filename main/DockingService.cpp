/*
 * DockingService.cpp
 *
 *  Created on: Sep 10, 2017
 *      Author: andri
 */

#include "DockingService.h"

const static char *TAG_DOCK = "DOCK";

DockingService::DockingService():
	Task("DockingService_Task", 2048) {
	// TODO Auto-generated constructor stub

}

DockingService::~DockingService() {
	// TODO Auto-generated destructor stub
}

bool DockingService::begin() {
	pn532_ = new PN532_SPI(SPI, PN532_SPI_NSS_GPIO);
	nfcDef_ = new NfcAdapter(*pn532_);
	nfc_ = new PN532(*pn532_);

	ESP_LOGI(TAG_DOCK, "NFC begins");

	nfc_->begin();
	nfcDef_->begin(true);

	ESP_LOGI(TAG_DOCK, "Getting NFC firmware");

	uint32_t versiondata = nfc_->getFirmwareVersion();
	if (! versiondata) {
		ESP_LOGI(TAG_DOCK, "Didn't find PN53x board");
		return false;
	}
	else {
		ESP_LOGI(TAG_DOCK, "Find PN53x board %d\n", versiondata);
	}

	// Got ok data, print it out!
	ESP_LOGI(TAG_DOCK, "Found chip PN5%x", ((versiondata>>24) & 0xFF));
	ESP_LOGI(TAG_DOCK, "Firmware ver. %d.%d", ((versiondata>>16) & 0xFF), ((versiondata>>8) & 0xFF));

	// Set the max number of retry attempts to read from a card
	// This prevents us from waiting forever for a card, which is
	// the default behaviour of the PN532.
	nfc_->setPassiveActivationRetries(0xFF);

	// configure board to read RFID tags
	nfc_->SAMConfig();

	ESP_LOGI(TAG_DOCK, "Waiting for an ISO14443A card");

	return true;
}

void DockingService::run() {
	if (nfcDef_->tagPresent()) // Do an NFC scan to see if an NFC tag is present
	{
		NfcTag tag = nfcDef_->read(); // read the NFC tag into an object, nfc.read() returns an NfcTag object.
		//tag.print(); // prints the NFC tags type, UID, and NDEF message (if available)
		ESP_LOGI(TAG_DOCK, "UUID: %s", tag.getUidString().c_str());
		if (dockingAllowedCallback_) {
			dockingAllowedCallback_(tag.getUidString().c_str(), tag.getUidString().equals(HARDCODED_NFC_UDID));
		}
	}
}

void DockingService::runAsync(void* data) {
	for(;;) {
		run();
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}
