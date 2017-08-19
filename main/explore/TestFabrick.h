/*
 * TestFabrick.h
 *
 *  Created on: Aug 20, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTFABRICK_H_
#define MAIN_EXPLORE_TESTFABRICK_H_

 #include <FabrickClient.h>

 HardwareSerial loraSerial(1);
 static void testLoRa() {
 	loraSerial.begin(9600, SERIAL_8N1, 33, 23);
 	FabrickLoraClient.begin(&loraSerial);

 	//String temp_data_s = String(2350, HEX);
 	float temp_f = 23.45;

 	int temp_int       = int(temp_f * 10);                                 // Scale and convert to int.
 	long temp_2c       = (temp_int >= 0) ? temp_int : (temp_int + 65536);  // Convert to 2's complement
 	String temp_data_s = String(temp_2c, HEX);                             // Convert to hex.
 	temp_data_s        = FabrickLoraClientClass::addZeros(temp_data_s, 4);                     // Ensure consistent length

 	//FabrickLoraClient.send("81ef46e9", 1, 0, 3303, temp_data_s, 4);
 	FabrickLoraClient.send("c1a12d25", 1, 0, 3303, temp_data_s, 4);
 }

#endif /* MAIN_EXPLORE_TESTFABRICK_H_ */
