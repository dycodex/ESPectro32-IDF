/*
 * WavFile.h
 *
 *  Created on: Sep 5, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_AUDIO_SRC_WAVFILE_H_
#define COMPONENTS_AUDIO_SRC_WAVFILE_H_

#include "Arduino.h"
#include <FS.h>
#include "SD.h"
#include "SPI.h"
#include <vector>

struct wav_header_t
{
    char chunkID[4]; //"RIFF" = 0x46464952
    unsigned long chunkSize; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
    char format[4]; //"WAVE" = 0x45564157
    char subchunk1ID[4]; //"fmt " = 0x20746D66
    unsigned long subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
    unsigned short audioFormat;
    unsigned char numChannels;
    unsigned long sampleRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    //[WORD wExtraFormatBytes;]
    //[Extra format bytes]
};

//Chunks
struct wav_chunk_t
{
    char ID[4]; //"data" = 0x61746164
    unsigned long size;  //Chunk data bytes
};

class WavFile: public fs::File {
public:
	static WavFile openWavFile(fs::FS &fs, const char* path, const char* mode);
	void parseHeader();
	void parseHeader(wav_header_t *theHeader, std::vector<wav_chunk_t> &theChunk);
	bool rewind();


	wav_header_t header;
	std::vector<wav_chunk_t> chunks;


protected:
	bool headerParsed_ = false;
};



#endif /* COMPONENTS_AUDIO_SRC_WAVFILE_H_ */
