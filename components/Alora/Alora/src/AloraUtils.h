/*
 * AloraUtils.h
 *
 *  Created on: Oct 18, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ALORA_ALORA_SRC_ALORAUTILS_H_
#define COMPONENTS_ALORA_ALORA_SRC_ALORAUTILS_H_

#include <Arduino.h>

void printLoc( Print & outs, int32_t degE7 );
void printLocToString(String & str, int32_t degE7);
int32_t encodeLat(int32_t lat);
int32_t encodeLon(int32_t lon);

#endif /* COMPONENTS_ALORA_ALORA_SRC_ALORAUTILS_H_ */
