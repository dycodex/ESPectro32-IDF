/*
 * AudioPlayer.h
 *
 *  Created on: Sep 10, 2017
 *      Author: andri
 */

#ifndef MAIN_AUDIOPLAYER_H_
#define MAIN_AUDIOPLAYER_H_

#include "Arduino.h"
#include <functional>
#include <Task.h>
#include <WavFile.h>
#include <ESPectro32_Constants.h>
#include <audio_renderer.hpp>

class AudioPlayer: public Task {
public:
	AudioPlayer();
	virtual ~AudioPlayer();

	void begin();
	void playAsync(const char* path);

	//void start(void *taskData=nullptr);
	void run();
	void runAsync(void *data);
	void stop();

private:
	Renderer *audioRenderer_ = NULL;
};

#endif /* MAIN_AUDIOPLAYER_H_ */
