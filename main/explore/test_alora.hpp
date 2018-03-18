/*
 * TestAlora.hpp
 *
 *  Created on: Oct 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTALORA_HPP_
#define MAIN_EXPLORE_TESTALORA_HPP_

#include <Arduino.h>
#include <AloraSensorKit.h>
#include <MakestroCloudClient32.h>

AloraSensorKit sensorKit;
static void test_alora() {

    Wire.begin();

    sensorKit.begin();

    for (;;) {
		sensorKit.run();

		// get sensor values
		SensorValues sensorData = sensorKit.getLastSensorData();

		// sensorData.T1 and sensorData.T2 are temperature sensor data
		Serial.printf("Temp: %.2f, CO2: %d, Light: %.2f\n", sensorData.T1, sensorData.co2, sensorData.lux);

		delay(1000);
    }
}
#endif /* MAIN_EXPLORE_TESTALORA_HPP_ */
