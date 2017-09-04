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
#include <WavFile.h>
#include <audio_renderer.hpp>

extern "C" {
#include "driver/i2s.h"
#include "esp_log.h"
}

const static char *TAG_WAV = "WAV";

//struct wav_header_t
//{
//    char chunkID[4]; //"RIFF" = 0x46464952
//    unsigned long chunkSize; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
//    char format[4]; //"WAVE" = 0x45564157
//    char subchunk1ID[4]; //"fmt " = 0x20746D66
//    unsigned long subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
//    unsigned short audioFormat;
//    unsigned short numChannels;
//    unsigned long sampleRate;
//    unsigned long byteRate;
//    unsigned short blockAlign;
//    unsigned short bitsPerSample;
//    //[WORD wExtraFormatBytes;]
//    //[Extra format bytes]
//};

static void parseWavFile() {
	ESP_LOGI(TAG_WAV, "Opening file for playing");

	//fs::FS fs = SPIFFS;
	fs::FS fs = SD;

//	File file = fs.open("/WAV/MONKEY.WAV", FILE_READ);
//	wav_header_t header;
//
//	file.read((uint8_t *)&header, sizeof(header));

	WavFile f = WavFile::openWavFile(fs, "/WAV/SHARK.WAV", FILE_READ);
//	File file = fs.open("/WAV/MONKEY.WAV", FILE_READ);
//	WavFile *f = (WavFile*)&file;

//	wav_header_t header;
//	f.read((uint8_t *)&header, sizeof(header));

	wav_header_t header;
	std::vector<wav_chunk_t> chunks;
	f.parseHeader(&header, chunks);

	//Print WAV header
	printf("WAV File Header read:\n");
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


//	wav_chunk_t chunk;
//	printf("id\t" "size\n");
//	//go to data chunk
//	while (true)
//	{
//		f.read((uint8_t*)&chunk, sizeof(chunk));
//		printf("%c%c%c%c\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
//		if (*(unsigned int *)&chunk.ID == 0x61746164)
//			break;
//		//skip chunk data bytes
//		f.seek(chunk.size, SeekCur);
//	}

	for(auto chunk: chunks) {
		printf("%c%c%c%c ->\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
	}

	printf("Start playing\n");

	Renderer r;
	r.renderer_init();
	r.renderer_start();

	pcm_format_t pcm_fmt = {
		.sample_rate = header.sampleRate,
		.bit_depth = I2S_BITS_PER_SAMPLE_16BIT,
		.num_channels = header.numChannels,
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

	f.rewind();

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

	r.renderer_stop();
	r.renderer_destroy();
}


#endif /* MAIN_EXPLORE_TESTWAVFILE_HPP_ */
