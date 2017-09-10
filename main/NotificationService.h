/*
 * NotificationService.h
 *
 *  Created on: Sep 10, 2017
 *      Author: andri
 */

#ifndef MAIN_NOTIFICATIONSERVICE_H_
#define MAIN_NOTIFICATIONSERVICE_H_

#include "Arduino.h"
#include <functional>
#include "AudioPlayer.h"
#include <ESPectro32_Board.h>
#include <ESPectro32_LedMatrix_Animation.h>

static const uint8_t PROGMEM LED_MATRIX_UNLOCK_1[] =
{ B00111000,
  B01000100,
  B01000100,
  B01000100,
  B11111110,
  B11101110,
  B11111110,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_UNLOCK_2[] =
{ B01000100,
  B01000100,
  B01000100,
  B00000000,
  B11111110,
  B11101110,
  B11111110,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_UNLOCK_3[] =
{ B01000100,
  B01000100,
  B00000000,
  B00000000,
  B11111110,
  B11101110,
  B11111110,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_DOCKING_1[] =
{ B00001110,
  B11001110,
  B00101110,
  B00101110,
  B00101110,
  B11001110,
  B00001110,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_DOCKING_2[] =
{ B00001110,
  B11101110,
  B00011110,
  B00011110,
  B00011110,
  B11101110,
  B00001110,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_DOCKING_3[] =
{ B00001110,
  B11111110,
  B10001110,
  B10001110,
  B10001110,
  B11111110,
  B00001110,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_HOURGLASS[] =
{ B11111110,
  B10000010,
  B01000100,
  B00101000,
  B01000100,
  B10000010,
  B11111110,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_SLOWDOWN[] =
{ B00111000,
  B01000100,
  B10000010,
  B10010010,
  B10100010,
  B01000100,
  B00111000,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_NOT_OK[] =
{ B10000010,
  B01000100,
  B00101000,
  B00010000,
  B00101000,
  B01000100,
  B10000010,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_OK[] =
{ B00000000,
  B00000000,
  B00000010,
  B00000100,
  B10001000,
  B01010000,
  B00100000,
  B00000000
};

#define NOTIF_VOICE_UNLOCK 		"/WAV/B_UNLOCKED.WAV"
#define NOTIF_VOICE_DOCKED 		"/WAV/B_DOCKED.WAV"
#define NOTIF_VOICE_LOCK 		"/WAV/B_LOCKED.WAV"
#define NOTIF_VOICE_WAIT 		"/WAV/WAIT.WAV"
#define NOTIF_VOICE_SLOWDOWN		"/WAV/SLOWDOWN.WAV"
#define NOTIF_VOICE_ALERT		"/WAV/ALERT.WAV"

class NotificationService {
public:
	NotificationService();
	virtual ~NotificationService();

	void begin();
	void notifyUnlocked();
	void notifyLocked();
	void notifyDocked();
	void notifyWait();
	void notifySlowdown();
	void notifyWarning();
	void notifyOK();
	void clear();

private:
	AudioPlayer *audioPlayer_ = NULL;
	ESPectro32_LedMatrix_Animation *ledMatrixAnim_ = NULL;

	ESPectro32_LedMatrix_Animation *getLedMatrixAnimPtr();
	AudioPlayer *getAudioPlayerPtr();
};

#endif /* MAIN_NOTIFICATIONSERVICE_H_ */
