/*
 * TestMatrix.h
 *
 *  Created on: Aug 18, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TESTMATRIX_H_
#define MAIN_EXPLORE_TESTMATRIX_H_


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <ESPectro32_LedMatrix.h>


ESPectro32_LedMatrix ledmatrix = ESPectro32_LedMatrix();
// The lookup table to make the brightness changes be more visible
uint8_t sweep[] = {1, 2, 3, 4, 6, 8, 10, 15, 20, 30, 40, 60, 60, 40, 30, 20, 15, 10, 8, 6, 4, 3, 2, 1};

static const uint8_t PROGMEM smile_bmp[] =
{ B00000000,
  B01101100,
  B01101100,
  B00000000,
  B01000100,
  B00111000,
  B00000000
};

void testLedMatrix(void *p) {

	Serial.println("LED Matrix Test");
	delay(2000);

	if (!ledmatrix.begin()) {
		Serial.println("Led matrix not found");
		while (1);
	}
	Serial.println("Led matrix found!");

	ledmatrix.clear();
	ledmatrix.drawBitmap(0, 0, LED_MATRIX_ICON_HEART, 7, 7, 200);

	delay(1000);

	ledmatrix.setTextSize(1);
	ledmatrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
	ledmatrix.setTextColor(100);
	for (int x=0; x>=-(7*29); x--) {
		ledmatrix.clear();
		ledmatrix.setCursor(x,0);
		ledmatrix.print("Hiduplah Indonesia Raya...");
	  	delay(30);
	}

	Serial.println("Led matrix done text!");

	ledmatrix.clear();
	ledmatrix.drawBitmap(0, 0, LED_MATRIX_EMOTICON_SMILE, 7, 7, 200);

	// delay(5000);
	// ledmatrix.clear();

	// ledMatrix.begin();
	// delay(10);
	//ledMatrix.fillScreen(200);


	// ledmatrix.setRotation(1);
	// //for(;;) {
	// 	//for (uint8_t incr = 0; incr < 24; incr++)
	// 		for (uint8_t x = 0; x < 5; x++)
	// 			for (uint8_t y = 0; y < 5; y++)
	// 				ledmatrix.drawPixel(x, y, 200);//sweep[(x+y+incr)%24]);
	// // 	delay(20);
	// // }
	// delay(1000);

	// for (uint8_t i=0; i<6; i++) {
	// 	Wire.beginTransmission(0x74);
	// 	Wire.write((byte) 0x24+i*24);
	// 	// write 24 bytes at once
	// 	for (uint8_t j=0; j<24; j++) {
	// 		Wire.write((byte) 200);
	// 	}
	// 	Wire.endTransmission();
	// }


	// ESPectro32.scanI2C();

	vTaskDelete(NULL);
}


#endif /* MAIN_EXPLORE_TESTMATRIX_H_ */
