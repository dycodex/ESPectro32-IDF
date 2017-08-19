/*
 * ESPectro32_RGBLED_Animation.cpp
 *
 *  Created on: Aug 19, 2017
 *      Author: andri
 */

#include "ESPectro32_RGBLED_Animation.h"

ESPectro32_RGBLED_Animation::ESPectro32_RGBLED_Animation(ESPectro32_RGBLED &rgbLed, RgbLedColor_t &defaultColor):
rgbLed_(rgbLed), defaultColor_(defaultColor)
{
}

ESPectro32_RGBLED_Animation::~ESPectro32_RGBLED_Animation() {
}

void ESPectro32_RGBLED_Animation::start() {
	if (rgbLed_.getPixelCount() == 0) {
		return;
	}

	animationPrevStarted_ = true;
	this->rgbLed_.clear();
}

void ESPectro32_RGBLED_Animation::stop() {
	forceStop_ = true;
	animationPrevStarted_ = false;
	if (animator_ != NULL) {
		animator_->stop();
	}
}

void ESPectro32_RGBLED_Animation::start(
		WS2812Animator::AnimationUpdateCallback animUpdateCallback,
		WS2812Animator::AnimationFinishedCallback animFinishedCallback,
		uint16_t duration, uint16_t updateInterval) {

	if (rgbLed_.getPixelCount() == 0) {
		return;
	}

	animationPrevStarted_ = true;
	this->rgbLed_.clear();

	for (uint8_t pixNum = 0; pixNum < this->rgbLed_.getPixelCount(); pixNum++) {
		this->rgbLed_.setPixel(pixNum, defaultColor_.red, defaultColor_.green, defaultColor_.blue);
	}

	getAnimatorPtr()->start(animUpdateCallback, animFinishedCallback, duration, updateInterval);
}

void ESPectro32_RGBLED_Animation::run() {
	if (animator_ != NULL) {
		if (animator_->isRunning()) {
			animator_->run();
			rgbLed_.show();
		}
	}
}

WS2812Animator* ESPectro32_RGBLED_Animation::getAnimatorPtr() {
	if (animator_ == NULL) {
		animator_ = new WS2812Animator();
	}

	return animator_;
}



ESPectro32_RGBLED_FadeInOutAnimation::ESPectro32_RGBLED_FadeInOutAnimation(
		ESPectro32_RGBLED& rgbLed, RgbLedColor_t& defaultColor): ESPectro32_RGBLED_Animation(rgbLed, defaultColor) {
}

ESPectro32_RGBLED_FadeInOutAnimation::~ESPectro32_RGBLED_FadeInOutAnimation() {
}

void ESPectro32_RGBLED_FadeInOutAnimation::start(uint16_t duration, uint16_t count) {

	animMaxCount_ = count;

	ESPectro32_RGBLED_Animation::start([this](const WS2812Animator::AnimationParam param) {

		//Triangle function
		//y = (A/P) * (P - abs(x % (2*P) - P))
		float x = param.progress * 100;
		float P = 100/2;
		float b = (100/P) * (P - abs(((int)x % (int)(2*P)) - P));

		//float b = sin(param.progress * PI) * 100;
//		ESP_LOGI(TAG, "Progress %f", b);

		for (uint8_t pixNum = 0; pixNum < this->RgbLed().getPixelCount(); pixNum++) {
			this->RgbLed().setBrightnessPercent(pixNum, b);
		}

		this->RgbLed().show();

	}, [this, duration, count]() {

		animCompletedCount_++;

		if (animCompletedCount_ >= animMaxCount_ || forceStop_) {
			RGBLED_ANIM_DEBUG_PRINT("Animation DONE");
			if (this->animCompletedCb_ != NULL) {
				this->animCompletedCb_();
			}

			return;
		}

		forceStop_ = false;
		this->start(duration, count);

	}, duration, 10);
}

void ESPectro32_RGBLED_FadeInOutAnimation::stop() {
}
