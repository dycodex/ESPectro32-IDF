/*
 * ESPectro32_RGBLED_Animation.h
 *
 *  Created on: Aug 19, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_ANIMATION_H_
#define COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_ANIMATION_H_

#include "ESPectro32_RGBLED.h"
#include <WS2812Animator.h>

class ESPectro32_RGBLED_Animation {
public:
	ESPectro32_RGBLED_Animation(ESPectro32_RGBLED &rgbLed, RgbLedColor_t &defaultColor);
	virtual ~ESPectro32_RGBLED_Animation();

	void start();
	void start(WS2812Animator::AnimationUpdateCallback animUpdateCallback, WS2812Animator::AnimationFinishedCallback animFinishedCallback,
				   uint16_t duration = 0, uint16_t updateInterval = 0);
	void stop();
	void run();
	void onAnimationCompleted(WS2812Animator::AnimationFinishedCallback cb) {
		animCompletedCb_ = cb;
	}

	ESPectro32_RGBLED &RgbLed() {
		return rgbLed_;
	}

protected:
	ESPectro32_RGBLED &rgbLed_;
	RgbLedColor_t &defaultColor_;
	WS2812Animator *animator_ = NULL;

	WS2812Animator::AnimationFinishedCallback animCompletedCb_ = NULL;
	boolean animationPrevStarted_ = false;

	WS2812Animator *getAnimatorPtr();
};

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_ANIMATION_H_ */
