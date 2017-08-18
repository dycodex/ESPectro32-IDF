/*
 * ESPectro32_RGBLED.h
 *
 *  Created on: Aug 19, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_H_
#define COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_H_

#include <WS2812.h>
#include <WS2812Animator.h>
#include "ESPectro32_Constants.h"

class ESPectro32_RGBLED: public WS2812 {
public:
	ESPectro32_RGBLED(gpio_num_t gpioNum = ESPECTRO32_RGBLED_GPIO);
	virtual ~ESPectro32_RGBLED();
};

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_H_ */
