/*
 * test_imu.hpp
 *
 *  Created on: Nov 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_IMU_ADXL_U8G2_HPP_
#define MAIN_EXPLORE_TEST_IMU_ADXL_U8G2_HPP_

#include <Arduino.h>
#include <ESPectro32_Board.h>
#include "esp_log.h"
#include <SparkFun_ADXL345.h>
#include <arduinoFFT.h>
#include <SPI.h>

#define DISPLAY 0

#define SAMPLING_SIZE 256//512 // Sampling point

ADXL345 adxl = ADXL345(33);

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
double samplingFrequency = 500;

// real values
double ax_in[SAMPLING_SIZE];
double ay_in[SAMPLING_SIZE];
double az_in[SAMPLING_SIZE];

// imag values
double vImag_x[SAMPLING_SIZE];
double vImag_y[SAMPLING_SIZE];
double vImag_z[SAMPLING_SIZE];

#if DISPLAY

#define LCD_H 64
#define X_OFFSET 40
#define U8X8_USE_PINS
#include <U8g2lib.h>
#define OLED_SDA 21
#define OLED_SCL 22

//U8G2_SH1106_128X64_VCOMH0_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, OLED_SCL, OLED_SDA);
//U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R3);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
#endif

volatile bool imuDataIsReady = false;
void imuDataReady() {
//	Serial.print(".");
	imuDataIsReady = true;
}

#if DISPLAY
static void displaySpectrum(double *fft_buffer, double max_val, int n) {

	max_val = max_val / (LCD_H);

	//skalowanie wykresu do najwiekszej wartosci i wyswietlacza o wysokosci 64px
	byte p;
	for (word n = 1; n < (SAMPLING_SIZE / 2); n++) {
	  p = fft_buffer[n] / max_val;
	  p = (LCD_H-1) - p;
	  for (byte a = (LCD_H-1); a > p; a--)
		u8g2.drawPixel(X_OFFSET + n, a);
	}
}

#endif

static void test_imu_adxl_u8g2() {

//	pinMode(34, INPUT_PULLUP);
//	attachInterrupt(34, imuDataReady, FALLING);
	pinMode(34, INPUT_PULLDOWN);
	attachInterrupt(34, imuDataReady, RISING);
//	pinMode(14, INPUT_PULLUP);
//	attachInterrupt(14, imuDataReady, FALLING);

	adxl.powerOn();                     // Power on the ADXL345

	adxl.setRangeSetting(2);           // Give the range settings
									  // Accepted values are 2g, 4g, 8g or 16g
									  // Higher Values = Wider Measurement Range
									  // Lower Values = Greater Sensitivity

	adxl.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
									  // Default: Set to 1
									  // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library
	adxl.set_bw(ADXL345_BW_200);
	adxl.setInterruptLevelBit(0);
	adxl.setDataReadyInterruptMapping(1);
	adxl.dataReadyINT(true);

	// Enable Detection.
//	stat = imu->Enable_Pedometer();
//	stat = imu->Enable_Tilt_Detection(LSM6DSL_INT2_PIN);
//	stat = imu->Enable_Free_Fall_Detection(LSM6DSL_INT2_PIN);
//
//	if (stat == LSM6DSL_STATUS_OK) {
//		//ESP_LOGI("IMU", "All is OK");
//	}
//	else {
//		return;
//	}


	delay(500);

#if DISPLAY
	u8g2.begin(); //wyswietlacz
	u8g2.setContrast(200); //ustawienie kontrastu
	u8g2.setFont(u8g2_font_5x7_tr); //czcionka
	u8g2.setFontMode(1);
	delay(1000);
	//u8g2.clearDisplay();
	u8g2.clearBuffer();

//	u8g2.firstPage();
	u8g2.drawUTF8(X_OFFSET + 0, 8, "Waiting!");
	u8g2.sendBuffer();

	delay(500);
#endif

	int x,y,z;
	adxl.readAccel(&x, &y, &z);

//	for(;;) {
//		if (imuDataIsReady) {
//			imuDataIsReady = false;
//			adxl.readAccel(&x, &y, &z);
//			ESP_LOGI("IMU", "X: %d, Y: %d, Z: %d", x, y, z);
//			delay(100);
//		}
//	}
//
//	return;

	char buf_txt[16];
	for (;;) {

#if DISPLAY
		u8g2.clearBuffer(); //czyszczenie bufora LCD
#endif

		uint16_t i = 0;
		//for(int i=0; i < SAMPLING_SIZE; i++)

		unsigned long startTime = micros();

		while( i != SAMPLING_SIZE )
		{
			//currentMillis = millis();    //Overflows after around 70 minutes!

			if (imuDataIsReady) {
				imuDataIsReady = false;

				adxl.readAccel(&x, &y, &z);
				ax_in[i] = z;
				vImag_x[i] = 0;

				i++;
			}
	//		else {
	//			Serial.print("x");
	//		}

	//		while(millis() < (currentMillis + delayTime)){
	//		}

			delay(0);
		}

		unsigned long duration = micros() - startTime;
		samplingFrequency = SAMPLING_SIZE * 1000000.0f / (duration);
		Serial.printf("Freq: %f\r\n", samplingFrequency);

		/*FFT*/
		FFT.Windowing(ax_in, SAMPLING_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
		FFT.Compute(ax_in, vImag_x, SAMPLING_SIZE, FFT_FORWARD);
		FFT.ComplexToMagnitude(ax_in, vImag_x, SAMPLING_SIZE);
		double peak = FFT.MajorPeak(ax_in, SAMPLING_SIZE, samplingFrequency);

//		Serial.println(peak);     //Print out what frequency is the most dominant.
//		displayResult(peak, 0);
//		Serial.println();

		double maxAmp = 0;
		for(int i=2; i<(SAMPLING_SIZE/2); i++)
		{
			/*View all these three lines in serial terminal to see which frequencies has which amplitudes*/

	//		Serial.print((i * 1.0 * samplingFrequency) / SAMPLING_SIZE, 1);
	//		Serial.print(" ");
	//		Serial.println(ax_in[i], 1);    //View only this line in serial plotter to visualize the bins

			if (ax_in[i] > maxAmp)
				maxAmp = ax_in[i];
		}

		if (maxAmp <= 50) {
			snprintf (buf_txt, 10, "Noise!!");
		} else {
			snprintf (buf_txt, 10, "%.2fHz", peak);
		}
		Serial.println(buf_txt);
#if DISPLAY
		u8g2.drawStr(X_OFFSET + 5, 8, buf_txt);
#endif
		snprintf (buf_txt, 12, "%d", (int)maxAmp);
		Serial.println(buf_txt);
#if DISPLAY
		u8g2.drawStr(X_OFFSET + 5, 18, buf_txt);
#endif

		Serial.println("Restart");
//		delay(1000);

#if DISPLAY
		displaySpectrum(ax_in, maxAmp, SAMPLING_SIZE/2);
		u8g2.sendBuffer();
#endif

		delay(500);
	}

}

#endif /* MAIN_EXPLORE_TEST_IMU_HPP_ */
