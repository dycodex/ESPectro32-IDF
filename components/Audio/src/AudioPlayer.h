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


#define AUDIOPLAYER_DEBUG_PRINT(...)  ESP_LOGI("AUDIO", __VA_ARGS__)

class AudioPlayer: public Task {
public:
	AudioPlayer();
	virtual ~AudioPlayer();

	void begin(fs::FS &fs = SD, i2s_pin_config_t *pin_config = nullptr);
	void playAsync(const char* path);

	//void start(void *taskData=nullptr);
	void run();
	void runAsync(void *data);
	void stop();

private:
	fs::FS &audioFS_ = SD;
	Renderer *audioRenderer_ = NULL;
	//i2s_pin_config_t *i2s_pin_config_ = NULL;
};

#endif /* MAIN_AUDIOPLAYER_H_ */
