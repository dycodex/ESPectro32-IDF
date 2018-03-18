/*
 * test_imu.hpp
 *
 *  Created on: Nov 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_IMU_LSM6_U8G2_HPP_
#define MAIN_EXPLORE_TEST_IMU_LSM6_U8G2_HPP_

#include <Arduino.h>
#include <ESPectro32_Board.h>
#include "esp_log.h"
#include <LSM6DSLSensor.h>
#include <arduinoFFT.h>
#include <SPI.h>

#define DISPLAY 		1
#define IMU_SPI		0

#define SAMPLING_SIZE 256//512 // Sampling point

LSM6DSLSensor *imu;

#if !IMU_SPI
TwoWire *dev_i2c;
#endif

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

static void test_imu_lsm6() {

	pinMode(34, INPUT_PULLUP);
	attachInterrupt(34, imuDataReady, FALLING);
//	pinMode(34, INPUT_PULLDOWN);
//	attachInterrupt(34, imuDataReady, RISING);
//	pinMode(14, INPUT_PULLUP);
//	attachInterrupt(14, imuDataReady, FALLING);

#if !IMU_SPI
	dev_i2c = new TwoWire(1);
	dev_i2c->begin(23, 18, 400000);
#endif

//	int address;
//	int foundCount = 0;
//	for (address=1; address<127; address++) {
//		dev_i2c->beginTransmission(address);
//		uint8_t error = dev_i2c->endTransmission();
//		if (error == 0) {
//			foundCount++;
//			ESP_LOGI("IMU", "Found device at 0x%.2x", address);
//		}
//	}
//	ESP_LOGI("IMU", "Found %d I2C devices by scanning.", foundCount);

	delay(1000);

#if !IMU_SPI
	imu = new LSM6DSLSensor(dev_i2c, LSM6DSL_ACC_GYRO_I2C_ADDRESS_HIGH);
#else
	imu = new LSM6DSLSensor(&SPI, 33);
#endif

	LSM6DSLStatusTypeDef stat;
	stat = imu->Begin();
	if (stat != LSM6DSL_STATUS_OK) {
		ESP_LOGE("IMU", "Begin failed. Retry");
		//return;
#if !IMU_SPI
		dev_i2c->reset();
		delay(100);
#endif
		stat = imu->Begin();
		if (stat != LSM6DSL_STATUS_OK) {
			ESP_LOGE("IMU", "Begin failed");
			return;
		}
	}

	stat = imu->Enable_X();
	//stat = imu->Enable_G();
	if (stat != LSM6DSL_STATUS_OK) {
		ESP_LOGE("IMU", "cannot enable Accel");
		return;
	}

	uint8_t chipid;
	stat = imu->ReadID(&chipid);
	ESP_LOGE("IMU", "Chip ID: 0x%x", chipid);
	if (chipid != 0x6a) {
#if !IMU_SPI
		dev_i2c->reset();
#endif
	}

	float fs;
	stat = imu->Get_G_FS(&fs);
	ESP_LOGE("IMU", "FS: %f", fs);

	float odr;
	stat = imu->Get_X_ODR(&odr);
	ESP_LOGE("IMU", "ODR: %f", odr);

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

//	float xSen = 0;
//	imu->Get_X_Sensitivity(&xSen);
//	float xFs = 0;
//	imu->Get_X_FS(&xFs);
//	ESP_LOGI("IMU", "Sen: %f, Full scale: %f", xSen, xFs);

	imu->Set_X_ODR(833.0f);

	LSM6DSL_ACC_GYRO_W_HP_SLOPE_XL(imu, LSM6DSL_ACC_GYRO_HP_SLOPE_XL_DIS);
//	imu->Set_Active_Axis(X_AXIS);

//	imu->WriteReg(LSM6DSL_ACC_GYRO_CTRL3_C, 0x44);
//	imu->WriteReg(LSM6DSL_ACC_GYRO_CTRL9_XL, 0xE0);
	LSM6DSL_ACC_GYRO_W_DRDY_XL_on_INT1(imu, LSM6DSL_ACC_GYRO_INT1_DRDY_XL_ENABLED);
//	LSM6DSL_ACC_GYRO_W_DRDY_XL_on_INT2(imu, LSM6DSL_ACC_GYRO_INT2_DRDY_XL_ENABLED);
//	LSM6DSL_ACC_GYRO_W_DRDY_ON_INT1(imu, LSM6DSL_ACC_GYRO_DRDY_ON_INT1_ENABLED);
	LSM6DSL_ACC_GYRO_W_INT_ACT_LEVEL(imu, LSM6DSL_ACC_GYRO_INT_ACT_LEVEL_ACTIVE_LO);
//	LSM6DSL_ACC_GYRO_W_INT_ACT_LEVEL(imu, LSM6DSL_ACC_GYRO_INT_ACT_LEVEL_ACTIVE_HI);
//	LSM6DSL_ACC_GYRO_W_INT2_ON_INT1(imu, LSM6DSL_ACC_GYRO_INT2_ON_INT1_ENABLED);

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

	int32_t accelerometer[3];
	imu->Get_X_Axes(accelerometer);

	int noIntCounter = 0;
//	for(;;) {
//		if (imuDataIsReady) {
//			imuDataIsReady = false;
//			imu->Get_X_Axes(accelerometer);
//
//			ESP_LOGI("IMU", "X: %d, Y: %d, Z: %d", accelerometer[0], accelerometer[1], accelerometer[2]);
//
//		}
//		else {
//			noIntCounter++;
//			if (noIntCounter > 3) {
//				noIntCounter = 0;
//				imu->Get_X_Axes(accelerometer);
//			}
//		}
//
//		delay(10);
//	}
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

				imu->Get_X_Axes(accelerometer);
				ax_in[i] = accelerometer[2];
				vImag_x[i] = 0;

				i++;
			}
			else {
				noIntCounter++;
				if (noIntCounter > 2) {
					noIntCounter = 0;
					imu->Get_X_Axes(accelerometer);
				}
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
