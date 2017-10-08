/*
 * TestSPIFFS.h
 *
 *  Created on: Aug 18, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTSPIFFS_H_
#define MAIN_EXPLORE_TESTSPIFFS_H_


#include <SPIFFS.h>
#include <FS.h>

/*
extern "C" {
	#include "esp_vfs.h"
	#include "esp_vfs_fat.h"
	#include "wear_levelling.h"
}
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;
static bool mount_fs() {
	ESP_LOGI(TAG, "Mounting FAT filesystem");
	// To mount device we need name of device partition, define base_path
	// and allow format partition in case if it is new one and was not formated before
	const esp_vfs_fat_mount_config_t mount_config = {
		.format_if_mount_failed = true,
		.max_files = 4};
	esp_err_t err = esp_vfs_fat_spiflash_mount("/spiflash", "storage", &mount_config, &s_wl_handle);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to mount FATFS (0x%x)", err);
		return false;
	}

	return true;
}
*/

void test_spiffs() {
	if (!SPIFFS.begin()) {
	    ESP_LOGE(TAG, "SPIFFS Mount Failed");
	    return;
	}

	fs::FS fs = SPIFFS;
	const char *dirname = "/";
	uint8_t levels = 0;

	File root = fs.open(dirname);
	if (!root) {
		Serial.println("Failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println("Not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels) {
				//listDir(fs, file.name(), levels - 1);
			}
		} else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}

}


#endif /* MAIN_EXPLORE_TESTSPIFFS_H_ */
