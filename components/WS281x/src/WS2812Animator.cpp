/*
 * WS2812Animation.cpp
 *
 *  Created on: May 22, 2017
 *      Author: andri
 */

#include "WS2812Animator.h"

WS2812Animator::WS2812Animator():
Task("ws2812AnimatorTask", 4096, configMAX_PRIORITIES - 3){

}

WS2812Animator::~WS2812Animator() {
	stop();
}

void WS2812Animator::start(AnimationUpdateCallback animUpdateCallback,
		AnimationFinishedCallback animFinishedCallback, uint16_t duration,
		uint16_t updateInterval) {

	if (updateInterval > 0) {
		updateInterval_ = updateInterval;
	}

	duration_ = duration;
	animationUpdateCallback_ = animUpdateCallback;
	animationFinishedCallback_ = animFinishedCallback;
	running_ = true;

	elapsed_ = 0;

	Task::start();
}

void WS2812Animator::stop() {
	running_ = false;
	if (animationFinishedCallback_) {
		animationFinishedCallback_();
	}

	Task::stop();
}

void WS2812Animator::run() {
	if (!running_) {
		return;
	}

	uint32_t currentTick = millis();
	uint32_t delta = currentTick - lastUpdated_;

	if (delta > updateInterval_) {

		if (duration_ != 0 && elapsed_ > duration_) {
			stop();
			return;
		}

		AnimationParam param;

		param.duration = duration_;
		param.elapsed = elapsed_;
		param.progress = (duration_ == 0)? 0: (float)(param.elapsed) / (float)param.duration;
		param.state = (param.elapsed == 0) ? AnimationState_Started : (duration_ != 0 && elapsed_ >= duration_)? AnimationState_Completed: AnimationState_Progress;

		if (animationUpdateCallback_) {
			animationUpdateCallback_(param);
		}

		elapsed_ += updateInterval_;

		lastUpdated_ = currentTick;
	}
}

void WS2812Animator::runAsync(void* data) {
	for(;;) {
		run();
		delay(1);
	}
}

int WS2812Animator::getStep(long elapsed, long t, int v) {
	return ((elapsed)%t)*v/t;
}

float WS2812Animator::getStepFloat(long elapsed, long t, float v) {
	return ((elapsed)%t)*v/t;
}
