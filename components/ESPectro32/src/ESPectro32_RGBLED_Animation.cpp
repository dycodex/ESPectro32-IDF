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
