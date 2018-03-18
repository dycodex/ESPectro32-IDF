/*
 * test_imu.hpp
 *
 *  Created on: Nov 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_IMU_LSM6_HPP_
#define MAIN_EXPLORE_TEST_IMU_LSM6_HPP_

#include <Arduino.h>
#include <ESPectro32_Board.h>
#include "esp_log.h"
#include <LSM6DSLSensor.h>
#include <arduinoFFT.h>
#include <SPI.h>

#define max(a,b) ((a)>(b)?(a):(b))

#define SAMPLING_SIZE 256//512 // Sampling point

LSM6DSLSensor *imu;
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

#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#define OLED_SDA 21
#define OLED_SCL 22
Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

uint8_t amp_previa[SAMPLING_SIZE];
uint8_t peak[SAMPLING_SIZE];
uint8_t diffPeak;
#define fallPeakSpeed 0

volatile bool imuDataIsReady = false;
void imuDataReady() {
	Serial.print(".");
	imuDataIsReady = true;
}

static void displayResult(double freq1, double freq2) {

	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0,0);
	display.print("Freq (Hz):");
	display.setCursor(128/2, 0);
	display.println("Dunno:");

	display.setCursor(0, display.getCursorY() + 2);
	display.setTextSize(2);
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.print(freq1, 1);
	display.setCursor(128/2, display.getCursorY());
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.println(freq2, 1);

//	display.setCursor(0, display.getCursorY() + 2);
//	display.setTextSize(1);
//	display.setTextColor(WHITE);
//	display.print("Watt (W):");
//	display.setCursor(128/2, display.getCursorY());
//	display.println("Freq (Hz):");
//
//	display.setCursor(0, display.getCursorY() + 2);
//	display.setTextSize(2);
//	display.setTextColor(BLACK, WHITE); // 'inverted' text
//	display.print((curr*volt), 1);
//	display.setCursor(128/2, display.getCursorY());
//	display.setTextColor(BLACK, WHITE); // 'inverted' text
//	display.println(freq, 1);

	display.display();
}

static void displaySpectrum(double *fft_buffer, int n) {

	display.clearDisplay();

	int i;
	uint8_t amp_actual;
	int x_max = display.width();
	int y_max = display.height();

	for (i = 2; i < n; i++) {
		// amplitud del espectro
		amp_actual = (uint8_t) fft_buffer[i] / 2;

		if (amp_previa[i] > amp_actual)
			display.drawFastVLine(i, y_max - amp_previa[i], amp_previa[i] - amp_actual, WHITE);

		if (amp_actual > amp_previa[i])
			display.drawFastVLine(i, y_max - amp_actual, amp_actual - amp_previa[i], WHITE);
		amp_previa[i] = amp_actual;

		/*
		// Effect drop slow peaks
		if (peak[i] > amp_actual) {
			diffPeak = peak[i] - amp_actual;
			display.drawPixel(i, y_max - peak[i], WHITE);
			if (diffPeak > 40)
				peak[i] -= fallPeakSpeed * 16;
			else if (diffPeak > 30)
				peak[i] -= fallPeakSpeed * 8;
			else if (diffPeak > 20)
				peak[i] -= fallPeakSpeed * 4;
			else if (diffPeak > 10)
				peak[i] -= fallPeakSpeed * 2;
			else
				peak[i] -= fallPeakSpeed;
		} else {
			display.drawPixel(i, y_max - peak[i], WHITE);
			peak[i] = amp_actual;
		}*/
		display.drawPixel(i, y_max - peak[i], WHITE);
	}

	display.display();
}

static void test_imu_lsm6() {

	pinMode(34, INPUT_PULLUP);
	attachInterrupt(34, imuDataReady, FALLING);
//	attachInterrupt(34, imuDataReady, RISING);
//	pinMode(14, INPUT_PULLUP);
//	attachInterrupt(14, imuDataReady, FALLING);

	display.begin(SH1106_SWITCHCAPVCC, 0x3C);
	display.display();
	delay(1000);
	display.clearDisplay();

	display.setTextSize(1);
	display.setCursor(0, 0);
	display.setTextColor(WHITE);
	display.println("		Waiting...");
	display.display();

	imu = new LSM6DSLSensor(&Wire, LSM6DSL_ACC_GYRO_I2C_ADDRESS_HIGH);
//	imu = new LSM6DSLSensor(&SPI, 33);

	LSM6DSLStatusTypeDef stat = imu->Enable_X();
	//stat = imu->Enable_G();
	if (stat != LSM6DSL_STATUS_OK) {
		ESP_LOGE("IMU", "cannot enable Accel");
		return;
	}

	uint8_t chipid;
	stat = imu->ReadID(&chipid);
	ESP_LOGE("IMU", "Chip ID: 0x%x", chipid);

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

	//LSM6DSL_ACC_GYRO_W_HP_SLOPE_XL(imu, LSM6DSL_ACC_GYRO_HP_SLOPE_XL_DIS);
//	imu->Set_Active_Axis(X_AXIS);

//	imu->WriteReg(LSM6DSL_ACC_GYRO_CTRL3_C, 0x44);
//	imu->WriteReg(LSM6DSL_ACC_GYRO_CTRL9_XL, 0xE0);
	LSM6DSL_ACC_GYRO_W_DRDY_XL_on_INT1(imu, LSM6DSL_ACC_GYRO_INT1_DRDY_XL_ENABLED);
//	LSM6DSL_ACC_GYRO_W_DRDY_XL_on_INT2(imu, LSM6DSL_ACC_GYRO_INT2_DRDY_XL_ENABLED);
//	LSM6DSL_ACC_GYRO_W_DRDY_ON_INT1(imu, LSM6DSL_ACC_GYRO_DRDY_ON_INT1_ENABLED);
	LSM6DSL_ACC_GYRO_W_INT_ACT_LEVEL(imu, LSM6DSL_ACC_GYRO_INT_ACT_LEVEL_ACTIVE_LO);
//	LSM6DSL_ACC_GYRO_W_INT2_ON_INT1(imu, LSM6DSL_ACC_GYRO_INT2_ON_INT1_ENABLED);

	delay(1000);

	for (;;) {

		int32_t accelerometer[3];
		uint16_t i = 0;
		//for(int i=0; i < SAMPLING_SIZE; i++)
		imu->Get_X_Axes(accelerometer);
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
	//		else {
	//			Serial.print("x");
	//		}


	//		while(millis() < (currentMillis + delayTime)){
	//		}

			delay(1);
		}

		/*FFT*/
		FFT.Windowing(ax_in, SAMPLING_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
		FFT.Compute(ax_in, vImag_x, SAMPLING_SIZE, FFT_FORWARD);
		FFT.ComplexToMagnitude(ax_in, vImag_x, SAMPLING_SIZE);
		double peak = FFT.MajorPeak(ax_in, SAMPLING_SIZE, samplingFrequency);

		Serial.println(peak);     //Print out what frequency is the most dominant.
//		displayResult(peak, 0);
//		Serial.println();
//
		for(int i=2; i<(SAMPLING_SIZE/2); i++)
		{
			/*View all these three lines in serial terminal to see which frequencies has which amplitudes*/

	//		Serial.print((i * 1.0 * samplingFrequency) / SAMPLING_SIZE, 1);
	//		Serial.print(" ");
			Serial.println(ax_in[i], 1);    //View only this line in serial plotter to visualize the bins
		}

		Serial.println("Restart");
//		delay(1000);

//		displaySpectrum(ax_in, SAMPLING_SIZE/2);
		delay(500);
	}

}

#endif /* MAIN_EXPLORE_TEST_IMU_HPP_ */
