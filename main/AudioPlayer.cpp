/*
 * AudioPlayer.cpp
 *
 *  Created on: Sep 10, 2017
 *      Author: andri
 */

#include "AudioPlayer.h"
#include <FS.h>
#include "SD.h"
#include "SPI.h"
#include <SPIFFS.h>

extern "C" {
#include "driver/i2s.h"
#include "esp_log.h"
}

const static char *TAG_AUDIO = "AUDIO";

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

void AudioPlayer::begin() {
	ESP_LOGI(TAG_AUDIO, "Opening file for playing");

	if(!SD.begin(ESPECTRO32_SDCARD_CSPIN)){
		ESP_LOGE(TAG_AUDIO, "Card Mount Failed");
		return;
	}
}

void AudioPlayer::playAsync(const char* path) {
	Task::start((void*)path);
}

//void AudioPlayer::start(void* taskData) {
//}

void AudioPlayer::run() {
}

void AudioPlayer::runAsync(void* data) {
	const char* path = (const char*)data;

	fs::FS fs = SD;

//	WavFile f = WavFile::openWavFile(fs, path, FILE_READ);
//
//	wav_header_t header;
//	std::vector<wav_chunk_t> chunks;
//	f.parseHeader(&header, chunks);

	File f = fs.open(path, FILE_READ);
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

//	for(auto chunk: chunks) {
//		printf("%c%c%c%c ->\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
//	}

	printf("Start playing\n");

	if (audioRenderer_ == NULL) {
		audioRenderer_ = new Renderer();
		audioRenderer_->renderer_init();
	}

	audioRenderer_->renderer_start();

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
			audioRenderer_->render_samples(buffer_read, 512, &pcm_fmt);
		}
		else {
			printf("Stop playing\n");
			break;
		}
	}

	audioRenderer_->renderer_stop();
	//audioRenderer_->renderer_destroy();

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
