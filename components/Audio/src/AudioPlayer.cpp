/*
 * AudioPlayer.cpp
 *
 *  Created on: Sep 10, 2017
 *      Author: andri
 */

#include "AudioPlayer.h"
#include <ESPectro32_Board.h>
#include <FS.h>
#include "SD.h"
#include "SPI.h"
#include <SPIFFS.h>

extern "C" {
#include "driver/i2s.h"
#include "esp_log.h"
}

AudioPlayer::AudioPlayer():
	Task("AudioPlayer_Task", 2048*4, configMAX_PRIORITIES - 2) {

}

AudioPlayer::~AudioPlayer() {

	if (audioRenderer_ != NULL) {
		audioRenderer_->renderer_stop();
		audioRenderer_->renderer_destroy();
		audioRenderer_ = NULL;
	}
}

void AudioPlayer::begin(fs::FS &fs, i2s_pin_config_t *pin_config) {
	AUDIOPLAYER_DEBUG_PRINT("Opening file for playing");

	//i2s_pin_config_ = pin_config;
	audioFS_ = fs;

	if (audioRenderer_ == NULL) {
		audioRenderer_ = new Renderer(pin_config);
		audioRenderer_->renderer_init();
	}
}

void AudioPlayer::playAsync(const char* path) {
	Task::start((void*)path);
}

//void AudioPlayer::start(void* taskData) {
//}

void AudioPlayer::run() {
}

bool starts_with (const char* base, const char* str) {
    return (strstr(base, str) - base) == 0;
}

void AudioPlayer::runAsync(void* data) {
	const char* path = (const char*)data;

	File f;
	wav_header_t header;

		f = WavFile::openWavFile(audioFS_, path, FILE_READ);


		std::vector<wav_chunk_t> chunks;

		WavFile file = *(WavFile*)&f;
		file.parseHeader(&header, chunks);

//		f = audioFS_.open(path, FILE_READ);
//		delay(100);
//	//	WavFile f = *(WavFile*)&file; //--> it's not working if using SPIFFS
//
//		f.read((uint8_t *)&header, sizeof(header));



//	wav_chunk_t chunk;
//	printf("id\t" "size\n");
//	//Run through data chunk
//	while (true)
//	{
//		f.read((uint8_t*)&chunk, sizeof(chunk));
//		printf("%c%c%c%c\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
//		if (*(unsigned int *)&chunk.ID == 0x61746164)
//			break;
//		//skip chunk data bytes
//		f.seek(chunk.size, SeekCur);
//	}

	//Print WAV header
		AUDIOPLAYER_DEBUG_PRINT("WAV File Header readings:");
		AUDIOPLAYER_DEBUG_PRINT("\tFile Type: %s", header.chunkID);
		AUDIOPLAYER_DEBUG_PRINT("\tFile Size: %ld", header.chunkSize);
		AUDIOPLAYER_DEBUG_PRINT("\tWAV Marker: %s", header.format);
		AUDIOPLAYER_DEBUG_PRINT("\tFormat Name: %s", header.subchunk1ID);
		AUDIOPLAYER_DEBUG_PRINT("\tFormat Length: %ld", header.subchunk1Size );
		AUDIOPLAYER_DEBUG_PRINT("\tFormat Type: %hd", header.audioFormat);
		AUDIOPLAYER_DEBUG_PRINT("\tNumber of Channels: %hd", header.numChannels);
		AUDIOPLAYER_DEBUG_PRINT("\tSample Rate: %ld", header.sampleRate);
		AUDIOPLAYER_DEBUG_PRINT("\tSample Rate * Bits/Sample * Channels / 8: %ld", header.byteRate);
		AUDIOPLAYER_DEBUG_PRINT("\tBits per Sample * Channels / 8.1: %hd", header.blockAlign);
		AUDIOPLAYER_DEBUG_PRINT("\tBits per Sample: %hd", header.bitsPerSample);

//	for(auto chunk: chunks) {
//		printf("%c%c%c%c ->\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
//	}

		AUDIOPLAYER_DEBUG_PRINT("Start playing");

//	//if (audioRenderer_ == NULL) {
//		audioRenderer_ = new Renderer(i2s_pin_config_);
//		audioRenderer_->renderer_init();
//	//}

	audioRenderer_->renderer_start();

	i2s_bits_per_sample_t bps = (header.bitsPerSample == 0)? I2S_BITS_PER_SAMPLE_16BIT: (i2s_bits_per_sample_t)header.bitsPerSample; //I2S_BITS_PER_SAMPLE_16BIT;

	pcm_format_t pcm_fmt = {
		.sample_rate = header.sampleRate,//22050
		.bit_depth = bps,
		.num_channels = header.numChannels,//1
		.buffer_format = PCM_LEFT_RIGHT,
		.endianness = PCM_BIG_ENDIAN
	};

	char buffer_read[512];
	int readed = 0;

	while(1) {
		readed = f.read((uint8_t*)buffer_read, sizeof(buffer_read));

		if (readed > 0) {
			audioRenderer_->render_samples(buffer_read, 512, &pcm_fmt);
		}
		else {
			AUDIOPLAYER_DEBUG_PRINT("Stop playing");
			break;
		}
	}

	audioRenderer_->renderer_stop();
//	audioRenderer_->renderer_destroy();

	vTaskDelete(NULL);
}

void AudioPlayer::stop() {
	if (audioRenderer_ != NULL) {
		audioRenderer_->renderer_stop();
		audioRenderer_->renderer_destroy();
		audioRenderer_ = NULL;
	}

	Task::stop();
}
