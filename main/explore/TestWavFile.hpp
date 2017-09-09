/*
 * TestWavFile.hpp
 *
 *  Created on: Sep 5, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTWAVFILE_HPP_
#define MAIN_EXPLORE_TESTWAVFILE_HPP_

#include "Arduino.h"
#include <FS.h>
#include "SD.h"
#include "SPI.h"
#include <SPIFFS.h>
#include <WavFile.h>
#include <audio_renderer.hpp>

extern "C" {
#include "driver/i2s.h"
#include "esp_log.h"
}

const static char *TAG_WAV = "WAV";

//Change this define to 1 for using SPIFFS instead of SD-Card
#define USE_SDCARD 	0

static void playWavFile() {
	ESP_LOGI(TAG_WAV, "Opening file for playing");


#if USE_SDCARD
	if(!SD.begin(ESPECTRO32_SDCARD_CSPIN)){
		ESP_LOGE(TAG, "Card Mount Failed");
		return;
	}
	fs::FS fs = SD;
#else
	if (!SPIFFS.begin()) {
		ESP_LOGE(TAG, "SPIFFS Mount Failed");
		return;
	}
	fs::FS fs = SPIFFS;
#endif

#if USE_SDCARD
//	File file = fs.open("/WAV/MONKEY.WAV", FILE_READ);
//	wav_header_t header;
//
//	file.read((uint8_t *)&header, sizeof(header));

	WavFile f = WavFile::openWavFile(fs, "/WAV/SHARK.WAV", FILE_READ);

	wav_header_t header;
	std::vector<wav_chunk_t> chunks;
	f.parseHeader(&header, chunks);

#else
	File f = fs.open("/bikedocked.wav", FILE_READ);
	delay(100);
//	WavFile f = *(WavFile*)&file; //--> it's not working if using SPIFFS

	wav_header_t header;
	f.read((uint8_t *)&header, sizeof(header));

	wav_chunk_t chunk;
	printf("id\t" "size\n");
	//Run through data chunk
	while (true)
	{
		f.read((uint8_t*)&chunk, sizeof(chunk));
		printf("%c%c%c%c\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
		if (*(unsigned int *)&chunk.ID == 0x61746164)
			break;
		//skip chunk data bytes
		f.seek(chunk.size, SeekCur);
	}

#endif

	//Print WAV header
	printf("WAV File Header readings:\n");
	printf("File Type: %s\n", header.chunkID);
	printf("File Size: %ld\n", header.chunkSize);
	printf("WAV Marker: %s\n", header.format);
	printf("Format Name: %s\n", header.subchunk1ID);
	printf("Format Length: %ld\n", header.subchunk1Size );
	printf("Format Type: %hd\n", header.audioFormat);
	printf("Number of Channels: %hd\n", header.numChannels);
	printf("Sample Rate: %ld\n", header.sampleRate);
	printf("Sample Rate * Bits/Sample * Channels / 8: %ld\n", header.byteRate);
	printf("Bits per Sample * Channels / 8.1: %hd\n", header.blockAlign);
	printf("Bits per Sample: %hd\n", header.bitsPerSample);

#if USE_SDCARD
	for(auto chunk: chunks) {
		printf("%c%c%c%c ->\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
	}
#endif

	printf("Start playing\n");

	Renderer r;
	r.renderer_init();
	r.renderer_start();

	pcm_format_t pcm_fmt = {
		.sample_rate = 22050,//header.sampleRate,
		.bit_depth = I2S_BITS_PER_SAMPLE_16BIT,
		.num_channels = 1,//header.numChannels,
		.buffer_format = PCM_LEFT_RIGHT,
		.endianness = PCM_BIG_ENDIAN
	};

	char buffer_read[512];
	int readed = 0;

	while(1) {
		readed = f.read((uint8_t*)buffer_read, sizeof(buffer_read));

		if (readed > 0) {
			r.render_samples(buffer_read, 512, &pcm_fmt);
		}
		else {
			printf("Stop playing\n");
			break;
		}
	}

//	f.rewind();
//
//	while(1) {
//		readed = f.read((uint8_t*)buffer_read, sizeof(buffer_read));
//
//		if (readed > 0) {
//			r.render_samples(buffer_read, 512, &pcm_fmt);
//		}
//		else {
//			printf("Stop playing\n");
//			break;
//		}
//	}

	r.renderer_stop();
	r.renderer_destroy();
}


#endif /* MAIN_EXPLORE_TESTWAVFILE_HPP_ */
