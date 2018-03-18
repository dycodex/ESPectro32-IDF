/*
 * WavFile.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: andri
 */

#include "WavFile.h"

WavFile WavFile::openWavFile(fs::FS& fs, const char* path, const char* mode) {
	File file = fs.open(path, mode);
	return *(WavFile*)&file;
}

void WavFile::parseHeader() {
	parseHeader(&header, chunks);
}

void WavFile::parseHeader(wav_header_t* theHeader, std::vector<wav_chunk_t> &theChunk) {
	read((uint8_t *)theHeader, sizeof(wav_header_t));

//	wav_chunk_t chunk;
//	printf("id\t" "size\n");
//	//go to data chunk
//	while (true)
//	{
//		read((uint8_t*)&chunk, sizeof(chunk));
//		theChunk.push_back(chunk);
//		//printf("%c%c%c%c\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
//		if (*(unsigned int *)&chunk.ID == 0x61746164)
//			break;
//		//skip chunk data bytes
//		seek(chunk.size, SeekCur);
//	}

	headerParsed_ = true;
}

bool WavFile::rewind() {
	return seek(0, SeekSet);
}
