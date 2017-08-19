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
#include "esp_log.h"

#define DEBUG_RGBLED_ANIM 	1

#if DEBUG_RGBLED_ANIM
#define RGBLED_ANIM_DEBUG_PRINT(...)   	ESP_LOGI("ESP32_ANIM", __VA_ARGS__); //ESP_LOGD("ESP32_ANIM", __VA_ARGS__);
#else
#define RGBLED_ANIM_DEBUG_PRINT(...)
#endif

/**
 * @brief Base class of Neopixel RGB LED animation. You should provide callback for animUpdateCallback.
 *
 */
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

	uint16_t animCompletedCount_ = 0, animMaxCount_ = 0;
	bool forceStop_ = false;

	WS2812Animator *getAnimatorPtr();
};

/**
 * @brief A class of Neopixel RGB LED fading in/out animation.
 *
 */
class ESPectro32_RGBLED_FadeInOutAnimation: public ESPectro32_RGBLED_Animation {
public:
	ESPectro32_RGBLED_FadeInOutAnimation(ESPectro32_RGBLED &rgbLed, RgbLedColor_t &defaultColor);
	virtual ~ESPectro32_RGBLED_FadeInOutAnimation();

	/**
	 * @brief Start the animation
	 *
	 * @param[in] duration How long in milisecond that one cycle of fading in and out takes place
	 * @param[in] count How many cycle
	 */
	void start(uint16_t duration = 0, uint16_t count = 0);
	void stop();
};

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_ANIMATION_H_ */
