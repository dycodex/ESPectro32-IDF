/*
 * DockingService.h
 *
 *  Created on: Sep 10, 2017
 *      Author: andri
 */

#ifndef MAIN_DOCKINGSERVICE_H_
#define MAIN_DOCKINGSERVICE_H_

#include "Arduino.h"
#include <functional>
#include <Task.h>
#include "PN532_SPI.h"
#include "PN532.h"
#include "NfcAdapter.h"
#include "esp_log.h"


#define PN532_SPI_NSS_GPIO		13
#define HARDCODED_NFC_UDID 		"53 B8 E1 2B"

class DockingService: public Task {
public:

	typedef std::function<void(const char*)> DockingNFCCallback;
	typedef std::function<void(const char*, bool allowed)> DockingAllowedCallback;

	DockingService();
	virtual ~DockingService();

	bool begin();
	//void start(void *taskData=nullptr);
	void run();
	void runAsync(void *data);

	void onDockingAllowed(DockingAllowedCallback cb) {
		dockingAllowedCallback_ = cb;
	}

private:
	PN532_SPI *pn532_ = NULL;
	NfcAdapter *nfcDef_ = NULL;
	PN532 *nfc_ = NULL;

	DockingNFCCallback nfcDetectedCallback_ = NULL;
	DockingAllowedCallback dockingAllowedCallback_ = NULL;
};

#endif /* MAIN_DOCKINGSERVICE_H_ */
