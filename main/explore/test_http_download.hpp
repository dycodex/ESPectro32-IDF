/*
 * test_http_download.hpp
 *
 *  Created on: Dec 25, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTHTPPDOWNLOAD_HPP_
#define MAIN_EXPLORE_TESTHTPPDOWNLOAD_HPP_

#include <esp_log.h>
#include "sdkconfig.h"
#include <Arduino.h>
#include <FS.h>
#include "SD.h"
#include "SPI.h"

#include <ESPectro32_Board.h>
#include <AudioPlayer.h>

#include "test_wait_wifi.hpp"

extern "C" {
	#include <esp_request.h>
}

#define HTTP_DOWNLOAD_DEBUG_PRINT(...)  ESP_LOGI("HTTP", __VA_ARGS__);

static const uint8_t PROGMEM LED_PLAYING_SONG[] =
{ B00011110,
  B00110010,
  B00100010,
  B00100110,
  B01101110,
  B11101100,
  B11000000
};

File downloadedFile;
int download_callback(request_t *req, char *data, int len) {

	req_list_t *found = req->response->header;
	static int binary_len = -1, remain_len = -1;

	static fs::FS fs = SD;

	HTTP_DOWNLOAD_DEBUG_PRINT("Response code: %d", req->response->status_code);

//	while(found->next != NULL) {
//		found = found->next;
//		HTTP_DOWNLOAD_DEBUG_PRINT("Response header %s:%s", (char*)found->key, (char*)found->value);
//	}

//	HardwareSerial *gpsSerialPtr = (HardwareSerial*)req->context;

	HTTP_DOWNLOAD_DEBUG_PRINT("Downloading...%d/%d bytes, remain=%d bytes", len, binary_len, remain_len);
	if(req->response->status_code == 200) {
		//first time
		if(binary_len == -1) {
			found = req_list_get_key(req->response->header, "Content-Length");
			if(found) {
				HTTP_DOWNLOAD_DEBUG_PRINT("Binary len=%s", (char*)found->value);
				binary_len = atoi(found->value);
				remain_len = binary_len;
			} else {
				HTTP_DOWNLOAD_DEBUG_PRINT("Error get content length");
				return -1;
			}

			//begin
			downloadedFile = fs.open("/WAV/DOWNLOAD.WAV", FILE_WRITE);
			if (!downloadedFile) {
				HTTP_DOWNLOAD_DEBUG_PRINT("Failed to open file for writing");
				return -1;
			}
		}

//		size_t written = gpsSerialPtr->write((const uint8_t *)data, len);
//		if (written == 0) {
//			HTTP_DOWNLOAD_DEBUG_PRINT("No data written to GPS Serial");
//			return -1;
//		}

		size_t written = downloadedFile.write((uint8_t *)data, len);
		if (written == 0) {
			HTTP_DOWNLOAD_DEBUG_PRINT("No data written to File");
			downloadedFile.close();
			return -1;
		}

		remain_len -= len;

		//finish
		if(remain_len == 0) {
			HTTP_DOWNLOAD_DEBUG_PRINT("Download callback is finished");
			downloadedFile.close();
		}

		return 0;
	}

	return -1;
}

static void test_http_download() {

	//Will be blocking
	test_wait_wifi();

	ESPectro32_LedMatrix_Animation downloadAnim = ESPectro32_LedMatrix_Animation::startDownloadAnimation();

	request_t *req;
	int status;
	// vTaskDelay(1000/portTICK_RATE_MS);
	//req = req_new("http://music.albertarose.org/christmas/lyrics/music/AlvinAndTheChipmunks_WeWishYouAMerryChristmas.wav");
	req = req_new("http://andriyadi.me/wp-content/uploads/2017/12/We_Wish_You_A_Merry_Christmas_The_Chipmunks_.wav");
	//req->context = pvParameter;

	req_setopt(req, REQ_SET_HEADER, "User-Agent:Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36");
	req_setopt(req, REQ_FUNC_DOWNLOAD_CB, download_callback);
	status = req_perform(req);
	req_clean(req);

	if (status) {


		downloadAnim.stop();

		//Display last frame
		//downloadAnim.addFrameWithData(LED_PLAYING_SONG);
		//ESPectro32.LedMatrix().displayFrame(downloadAnim.getFrameCount() - 1);
		downloadAnim.clearFrames();
		ESPectro32.LedMatrix().drawBitmapFull(LED_PLAYING_SONG);

		//ESPectro32.LedMatrix().drawBitmapFull(LED_PLAYING_SONG);

		HTTP_DOWNLOAD_DEBUG_PRINT("File is downloaded");

		i2s_pin_config_t pin_config;
		pin_config.bck_io_num = GPIO_NUM_26;
		pin_config.ws_io_num = GPIO_NUM_25;
		pin_config.data_out_num = GPIO_NUM_32;
		pin_config.data_in_num = I2S_PIN_NO_CHANGE;

		AudioPlayer *player = new AudioPlayer();
		player->begin(SD, &pin_config);

		player->playAsync("/WAV/DOWNLOAD.WAV");
	}
}



#endif /* MAIN_EXPLORE_TESTHTPPDOWNLOAD_HPP_ */
