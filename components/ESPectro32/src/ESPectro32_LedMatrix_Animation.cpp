/*
 * ESPectro32_LedMatrix_Animation.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: andri
 */

#include "ESPectro32_LedMatrix_Animation.h"

ESPectro32_LedMatrix_Animation::ESPectro32_LedMatrix_Animation(){
}

ESPectro32_LedMatrix_Animation::~ESPectro32_LedMatrix_Animation() {
	stop();
}

void ESPectro32_LedMatrix_Animation::addFrameWithData(uint8_t* data, uint16_t color) {

	if (this->ledMatrix_ == NULL || frameCount_ >= ESPECTRO32_LEDMATRIX_MAX_FRAME_COUNT) {
		return;
	}

	setFrameWithData(frameCount_, data, color);
	frameCount_++;
}

void ESPectro32_LedMatrix_Animation::addFrameWithDataCallback(ESPectro32_LedMatrix_Animation::FrameDataCallback cb) {

	if (this->ledMatrix_ == NULL || frameCount_ >= ESPECTRO32_LEDMATRIX_MAX_FRAME_COUNT) {
		return;
	}

	setFrameWithDataCallback(frameCount_, cb);
	frameCount_++;
}

void ESPectro32_LedMatrix_Animation::setFrameWithData(uint8_t frameNo, uint8_t* data, uint16_t color) {

	this->ledMatrix_->setFrame(frameNo);
	this->ledMatrix_->clear();
	this->ledMatrix_->drawBitmap(0, 0, data, 7, 7, color);
}

void ESPectro32_LedMatrix_Animation::setFrameWithDataCallback(uint8_t frameNo,
		ESPectro32_LedMatrix_Animation::FrameDataCallback cb) {

	this->ledMatrix_->setFrame(frameNo);
	this->ledMatrix_->clear();

	cb(*this->ledMatrix_);
}

void ESPectro32_LedMatrix_Animation::start(
		Animator::AnimationUpdateCallback animUpdateCallback,
		Animator::AnimationFinishedCallback animFinishedCallback,
		uint16_t duration, uint16_t updateInterval) {

	if (this->ledMatrix_ == NULL || frameCount_ == 0) {
		return;
	}

	getAnimatorPtr()->start(animUpdateCallback, animFinishedCallback, duration, updateInterval);
}

void ESPectro32_LedMatrix_Animation::stop() {
	forceStop_ = true;
	if (animator_ != NULL) {
		animator_->stop();
	}
}

void ESPectro32_LedMatrix_Animation::run() {
	if (animator_ != NULL) {
		if (animator_->isRunning()) {
			animator_->run();
		}
	}
}

void ESPectro32_LedMatrix_Animation::start(uint16_t duration) {
	if (frameCount_ == 0) {
		return;
	}

	if (duration == 0) {
		duration = frameCount_ * 300;
	}

	getAnimatorPtr()->start([this](const Animator::AnimationParam param) {

		float index = param.progress * frameCount_;

		if (index < frameCount_) {
			this->ledMatrix_->displayFrame(index);
		}

	}, [this]() {
		//printf("LEDM ANIM DONE\n");
		if (this->animCompletedCb_ != NULL) {
			this->animCompletedCb_();
		}

	}, duration, 10);
}

Animator* ESPectro32_LedMatrix_Animation::getAnimatorPtr() {
	if (animator_ == NULL) {
		animator_ = new Animator();
	}

	return animator_;
}

