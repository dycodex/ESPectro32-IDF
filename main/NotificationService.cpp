/*
 * NotificationService.cpp
 *
 *  Created on: Sep 10, 2017
 *      Author: andri
 */

#include "NotificationService.h"


NotificationService::NotificationService() {
}

NotificationService::~NotificationService() {
}

void NotificationService::begin() {
}

void NotificationService::notifyUnlocked() {
	//getAudioPlayerPtr()->playAsync("/WAV/SHARK.WAV");
	ESPectro32.LedMatrix().clear();
	getLedMatrixAnimPtr()->addFrameWithData((uint8_t*)LED_MATRIX_UNLOCK_1);
	getLedMatrixAnimPtr()->addFrameWithData((uint8_t*)LED_MATRIX_UNLOCK_2);
	getLedMatrixAnimPtr()->addFrameWithData((uint8_t*)LED_MATRIX_UNLOCK_3);
//	getLedMatrixAnimPtr()->addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
//		ledM.clear();
//	});

	getLedMatrixAnimPtr()->start(2000, false);

	getLedMatrixAnimPtr()->onAnimationCompleted([this]() {
//		ESPectro32.LedMatrix().displayFrame(2);
		this->getAudioPlayerPtr()->playAsync(NOTIF_VOICE_UNLOCK);
	});
}

ESPectro32_LedMatrix_Animation* NotificationService::getLedMatrixAnimPtr() {
	if (ledMatrixAnim_ == NULL) {
		ledMatrixAnim_ = new ESPectro32_LedMatrix_Animation();
		ledMatrixAnim_->setLedMatrix(ESPectro32.LedMatrix());
	}

	return ledMatrixAnim_;
}

void NotificationService::notifyLocked() {
	ESPectro32.LedMatrix().clear();
	getLedMatrixAnimPtr()->addFrameWithData((uint8_t*)LED_MATRIX_UNLOCK_3);
	getLedMatrixAnimPtr()->addFrameWithData((uint8_t*)LED_MATRIX_UNLOCK_2);
	getLedMatrixAnimPtr()->addFrameWithData((uint8_t*)LED_MATRIX_UNLOCK_1);
//	getLedMatrixAnimPtr()->addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
//		ledM.clear();
//	});

	getLedMatrixAnimPtr()->start(2000, false);

	getLedMatrixAnimPtr()->onAnimationCompleted([this]() {
//		ESPectro32.LedMatrix().displayFrame(2);
		this->getAudioPlayerPtr()->playAsync(NOTIF_VOICE_LOCK);
	});
}

void NotificationService::notifyDocked() {
	ESPectro32.LedMatrix().clear();
	getLedMatrixAnimPtr()->addFrameWithData((uint8_t*)LED_MATRIX_DOCKING_1);
	getLedMatrixAnimPtr()->addFrameWithData((uint8_t*)LED_MATRIX_DOCKING_2);
	getLedMatrixAnimPtr()->addFrameWithData((uint8_t*)LED_MATRIX_DOCKING_3);
//	getLedMatrixAnimPtr()->addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
//		ledM.clear();
//	});

	getLedMatrixAnimPtr()->start(2000, false);

	getLedMatrixAnimPtr()->onAnimationCompleted([this]() {
//		ESPectro32.LedMatrix().displayFrame(2);
		this->getAudioPlayerPtr()->playAsync(NOTIF_VOICE_DOCKED);
	});
}

void NotificationService::notifyWait() {
	ESPectro32.LedMatrix().setFrame(0);
	ESPectro32.LedMatrix().clear();
	ESPectro32.LedMatrix().drawBitmap(0, 0, (uint8_t*)LED_MATRIX_HOURGLASS, 7, 7, 255);
	getAudioPlayerPtr()->playAsync(NOTIF_VOICE_WAIT);
}

void NotificationService::notifySlowdown() {
	ESPectro32.LedMatrix().setFrame(0);
	ESPectro32.LedMatrix().clear();
	ESPectro32.LedMatrix().drawBitmap(0, 0, (uint8_t*)LED_MATRIX_SLOWDOWN, 7, 7, 255);
	getAudioPlayerPtr()->playAsync(NOTIF_VOICE_SLOWDOWN);
}

void NotificationService::clear() {
	if (audioPlayer_ != NULL) {
		audioPlayer_->stop();
	}
	if (ledMatrixAnim_ != NULL) {
		ledMatrixAnim_->stop();
	}
	ESPectro32.LedMatrix().clear();
}

AudioPlayer* NotificationService::getAudioPlayerPtr() {
	if (audioPlayer_ == NULL) {
		audioPlayer_ = new AudioPlayer();
		audioPlayer_->begin();
	}

	return audioPlayer_;
}
