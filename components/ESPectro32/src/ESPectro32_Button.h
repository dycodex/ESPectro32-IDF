/*
 * ESPectro32_Button.h
 *
 *  Created on: Aug 8, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BUTTON_H_
#define COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BUTTON_H_

#include "Arduino.h"
#include "ESPectro32_Constants.h"
#include <functional>
#include <Task.h>

//#define _GLIBCXX_USE_C99
#include <string>

#define ESPECTRO32_BUTTON_DEBOUNCE_DURATION_MS        30
#define ESPECTRO32_BUTTON_PRESS_DURATION_MS           750
#define ESPECTRO32_BUTTON_CLICK_DURATION_MS           250
//#define ESPECTRO32_BUTTON_LONG_PRESS_DURATION_MS      800

class ESPectro32_Button: public Task {
public:

	enum Button_State {
	    ESPectro32ButtonUnknown = 0,
	    ESPectro32ButtonPressed,
	    ESPectro32ButtonReleased,
		ESPectro32ButtonSecondPressed,
		ESPectro32ButtonWaitingForLongPressed,
	    ESPectro32ButtonLongPressed
	};

	typedef std::function<void()> ButtonActionCallback;

	ESPectro32_Button(uint8_t gpio, boolean activeHigh = false);
	~ESPectro32_Button();

	void begin();
	void start(void *taskData=nullptr);
	Button_State getState();
	void run();
	void runAsync(void *data);
	void onButtonDown(ButtonActionCallback cb);
	void onButtonUp(ButtonActionCallback cb);
	void onPressed(ButtonActionCallback cb);
	void onLongPressed(ButtonActionCallback cb);
	void onDoublePressed(ButtonActionCallback cb);

	void setGpioNumber(uint8_t g) {
		gpioNumber_ = g;
	}

private:

    uint8_t gpioNumber_;
    bool activeHigh_ = false;
    bool trackLongPressed_ = false;
    Button_State buttonState_ = ESPectro32ButtonUnknown;

    unsigned long lastButtonChangedMillis_  = 0, lastButtonPressedMillis_ = 0;
    unsigned long checkingStartTime_  = 0, checkingEndTime_ = 0;
    volatile bool interruptTriggered_ = false;
    volatile bool runAlreadyCalled_ = false;

    ButtonActionCallback btnDownCallback_;
    ButtonActionCallback btnUpCallback_;
    ButtonActionCallback pressedCallback_;
    ButtonActionCallback longPressedCallback_;
    ButtonActionCallback doublePressedCallback_;

    bool isActive();
    uint8_t pressCount_ = 0;

    void examine();
};

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BUTTON_H_ */
