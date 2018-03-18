/*
 * test_imu.hpp
 *
 *  Created on: Nov 3, 2017
 *      Author: andri
 */

#ifndef MAIN_EXPLORE_TEST_IMU_HPP_
#define MAIN_EXPLORE_TEST_IMU_HPP_

#include <Arduino.h>
#include <ESPectro32_Board.h>
#include "esp_log.h"
#include <LSM6DSLSensor.h>
#include <arduinoFFT.h>
#include <SPI.h>

typedef struct {
  int32_t avr_x[4];
  int32_t avr_y[4];
  int32_t avr_z[4];
  int32_t max_x[4];
  int32_t max_y[4];
  int32_t max_z[4];
} tlv_vibration_value_t;

#define max(a,b) ((a)>(b)?(a):(b))

#define SAMPLING_SIZE 512 // Sampling point

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02

#define RANGE_0  0
#define RANGE_1  1
#define RANGE_2  2
#define RANGE_3  3

// define renge values
#define RANGE_0_1   0.05 //Hz
#define RANGE_0_2   2    //Hz

#define RANGE_1_1   2    //Hz
#define RANGE_1_2   10   //Hz

#define RANGE_2_1   10   //Hz
#define RANGE_2_2   50   //Hz

#define RANGE_3_1   50   //Hz
#define RANGE_3_2 100 //Hz

#define OUTPUT_READABLE_ACCELGYRO

LSM6DSLSensor *imu;
arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

// real values
double ax_in[SAMPLING_SIZE];
double ay_in[SAMPLING_SIZE];
double az_in[SAMPLING_SIZE];

// imag values
double vImag_x[SAMPLING_SIZE];
double vImag_y[SAMPLING_SIZE];
double vImag_z[SAMPLING_SIZE];

double samplingFrequency = 500;
unsigned int delayTime = 0;

unsigned long previousMillis = 0;        // will store last time accelerometer values are collected
unsigned long currentMillis = 0;

int sample_counter = 0;
bool data_ready = false;

tlv_vibration_value_t vibration_values;

double average_0_x = 0;
double average_1_x = 0;
double average_2_x = 0;
double average_3_x = 0;
double max_0_x = 0;
double max_1_x = 0;
double max_2_x = 0;
double max_3_x = 0;

double average_0_y= 0;
double average_1_y = 0;
double average_2_y = 0;
double average_3_y = 0;
double max_0_y = 0;
double max_1_y = 0;
double max_2_y = 0;
double max_3_y = 0;

double average_0_z = 0;
double average_1_z = 0;
double average_2_z = 0;
double average_3_z = 0;
double max_0_z = 0;
double max_1_z = 0;
double max_2_z = 0;
double max_3_z = 0;


/* Function for calculating average value */
double average(double *in, int range) {
	int start_num = 0;
	int stop_num = 0;
	double average = 0;
	switch (range) {
	case 0:
		start_num = 1;
		stop_num = 6;
		break;
	case 1:
		start_num = 6;
		stop_num = 14;
		break;
	case 2:
		start_num = 14;
		stop_num = 27;
		break;
	case 3:
		start_num = 27;
		stop_num = 44;
		break;
	}
	for (int i = start_num; i < stop_num; i++) {
		average += in[i];
	}
	average = average / (stop_num - start_num);
	return average;
}

/* Function for determing maximum value */
double max_value(double *in, int range) {
	int start_num = 0;
	int stop_num = 0;
	double max_value = 0;
	switch (range) {
	case 0:
		start_num = 1;
		stop_num = 6;
		break;
	case 1:
		start_num = 6;
		stop_num = 14;
		break;
	case 2:
		start_num = 14;
		stop_num = 27;
		break;
	case 3:
		start_num = 27;
		stop_num = 44;
		break;
	}
	for (int i = start_num; i < stop_num; i++) {
		max_value = max(max_value, in[i]);
	}
	return max_value;
}

/* Function for printing readable data */
void PrintVector(double *vData, double *vData1, double *vData2, uint16_t bufferSize, uint8_t scaleType){

	double abscissa = 0;
	for (uint16_t i = 0; i < bufferSize; i++) {
		/* Print abscissa value */
		switch (scaleType) {
		case SCL_INDEX:
			abscissa = (i * 1.0);
			break;
		case SCL_TIME:
			abscissa = ((i * 1.0) / samplingFrequency);
			break;
		case SCL_FREQUENCY:
			abscissa = ((i * 1.0 * samplingFrequency) / SAMPLING_SIZE);
			break;
		}
		Serial.print(abscissa, 4);
		Serial.print(" ");
		Serial.print(vData[i]);
		Serial.print(" ");
		Serial.print(vData1[i]);
		Serial.print(" ");
		Serial.print(vData2[i]);
		Serial.println();
	}

	Serial.println();
}

void communicate(void) {

//	if (command_received) {
//		frame_parser((uint8_t *) &rx_buffer, message_len, &msg_parsed);
//		if (message_tlv_get_command(&msg_parsed, &parsed_command)
//				!= MESSAGE_SUCCESS) {
//			message_free (&msg_parsed);
//		} else {
//			if (parsed_command == COMMAND_GET_STATUS) {
				/* print readable data */
#ifdef OUTPUT_READABLE_ACCELGYRO
				PrintVector(ax_in, ay_in, az_in, (SAMPLING_SIZE >> 1), SCL_FREQUENCY);
				Serial.println();
				Serial.println();

				// display tab-separated accel/gyro x/y/z values
				Serial.print("average x:\t");
				Serial.print(average_0_x); Serial.print("\t");
				Serial.print(average_1_x); Serial.print("\t");
				Serial.print(average_2_x); Serial.print("\t");
				Serial.println(average_3_x);

				Serial.print("max x:\t");
				Serial.print(max_0_x); Serial.print("\t");
				Serial.print(max_1_x); Serial.print("\t");
				Serial.print(max_2_x); Serial.print("\t");
				Serial.println(max_3_x);

				Serial.print("average y:\t");
				Serial.print(average_0_y); Serial.print("\t");
				Serial.print(average_1_y); Serial.print("\t");
				Serial.print(average_2_y); Serial.print("\t");
				Serial.println(average_3_y);

				Serial.print("max y:\t");
				Serial.print(max_0_y); Serial.print("\t");
				Serial.print(max_1_y); Serial.print("\t");
				Serial.print(max_2_y); Serial.print("\t");
				Serial.println(max_3_y);

				Serial.print("average z:\t");
				Serial.print(average_0_z); Serial.print("\t");
				Serial.print(average_1_z); Serial.print("\t");
				Serial.print(average_2_z); Serial.print("\t");
				Serial.println(average_3_z);

				Serial.print("max z:\t");
				Serial.print(max_0_z); Serial.print("\t");
				Serial.print(max_1_z); Serial.print("\t");
				Serial.print(max_2_z); Serial.print("\t");
				Serial.println(max_3_z);
#endif

				// blink LED to indicate activity
				ESPectro32.toggleLED();

				/* send calculated data */
//				message_init (&msg_send);
//				message_tlv_add_reply(&msg_send, REPLY_STATUS_REPORT);

				if (data_ready == true) {
					/* put data in struct*/
					/*
					 vibration_values.avr_x[0] = 11111;//(int32_t)average_0_x;
					 vibration_values.avr_x[1] = 12222;//(int32_t)average_1_x;
					 vibration_values.avr_x[2] = 13333;//(int32_t)average_2_x;
					 vibration_values.avr_x[3] = 14444;//(int32_t)average_3_x;
					 vibration_values.avr_y[0] = 21111;//(int32_t)average_0_y;
					 vibration_values.avr_y[1] = 22222;//(int32_t)average_1_y;
					 vibration_values.avr_y[2] = 23333;///(int32_t)average_2_y;
					 vibration_values.avr_y[3] = 24444;//(int32_t)average_3_y;
					 vibration_values.avr_z[0] = 31111;//(int32_t)average_0_z;
					 vibration_values.avr_z[1] = 32222;//(int32_t)average_1_z;
					 vibration_values.avr_z[2] = 33333;//(int32_t)average_2_z;
					 vibration_values.avr_z[3] = 34444;//(int32_t)average_3_z;
					 vibration_values.max_x[0] = 15555;//(int32_t)max_0_x;
					 vibration_values.max_x[1] = 16666;//(int32_t)max_1_x;
					 vibration_values.max_x[2] = 17777;//(int32_t)max_2_x;
					 vibration_values.max_x[3] = 18888;//(int32_t)max_3_x;
					 vibration_values.max_y[0] = 25555;//(int32_t)max_0_y;
					 vibration_values.max_y[1] = 26666;//(int32_t)max_1_y;
					 vibration_values.max_y[2] = 27777;//(int32_t)max_2_y;
					 vibration_values.max_y[3] = 28888;//(int32_t)max_3_y;
					 vibration_values.max_z[0] = 35555;//(int32_t)max_0_z;
					 vibration_values.max_z[1] = 36666;//(int32_t)max_1_z;
					 vibration_values.max_z[2] = 37777;//(int32_t)max_2_z;
					 vibration_values.max_z[3] = 38888;//(int32_t)max_3_z;
					 */
					/***************************************************/
					vibration_values.avr_x[0] = (int32_t) average_0_x;
					vibration_values.avr_x[1] = (int32_t) average_1_x;
					vibration_values.avr_x[2] = (int32_t) average_2_x;
					vibration_values.avr_x[3] = (int32_t) average_3_x;

					vibration_values.avr_y[0] = (int32_t) average_0_y;
					vibration_values.avr_y[1] = (int32_t) average_1_y;
					vibration_values.avr_y[2] = (int32_t) average_2_y;
					vibration_values.avr_y[3] = (int32_t) average_3_y;

					vibration_values.avr_z[0] = (int32_t) average_0_z;
					vibration_values.avr_z[1] = (int32_t) average_1_z;
					vibration_values.avr_z[2] = (int32_t) average_2_z;
					vibration_values.avr_z[3] = (int32_t) average_3_z;

					vibration_values.max_x[0] = (int32_t) max_0_x;
					vibration_values.max_x[1] = (int32_t) max_1_x;
					vibration_values.max_x[2] = (int32_t) max_2_x;
					vibration_values.max_x[3] = (int32_t) max_3_x;

					vibration_values.max_y[0] = (int32_t) max_0_y;
					vibration_values.max_y[1] = (int32_t) max_1_y;
					vibration_values.max_y[2] = (int32_t) max_2_y;
					vibration_values.max_y[3] = (int32_t) max_3_y;

					vibration_values.max_z[0] = (int32_t) max_0_z;
					vibration_values.max_z[1] = (int32_t) max_1_z;
					vibration_values.max_z[2] = (int32_t) max_2_z;
					vibration_values.max_z[3] = (int32_t) max_3_z;

					//message_tlv_add_vibration_value(&msg_send, &vibration_values);

					data_ready = false;
				}
//
////				message_tlv_add_checksum(&msg_send);
////				send_bytes(&msg_send);
//
//				/* print readable sent message*/
//#ifdef OUTPUT_READABLE_ACCELGYRO
////				message_print(&msg_send);
//				Serial.println();
//#endif
//
////				message_free(&msg_send);
////				message_free (&msg_parsed);
//
//			} else {
////				message_free (&msg_parsed);
//			}
////			command_received = false;
//		}
//	}
}


volatile bool imuDataIsReady = false;
void imuDataReady() {
	//Serial.print(".");
	imuDataIsReady = true;
}

static void test_imu() {

	if(samplingFrequency<=1000)
	    delayTime = 1000/samplingFrequency;
	else
		delayTime = 1000000/samplingFrequency;

	pinMode(34, INPUT_PULLUP);
	attachInterrupt(34, imuDataReady, FALLING);
//	pinMode(14, INPUT_PULLUP);
//	attachInterrupt(14, imuDataReady, FALLING);

	imu = new LSM6DSLSensor(&Wire, LSM6DSL_ACC_GYRO_I2C_ADDRESS_HIGH);
	//imu = new LSM6DSLSensor(&SPI, 33);

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

//	Serial.println(peak);     //Print out what frequency is the most dominant.
	Serial.println();

	for(int i=2; i<(SAMPLING_SIZE/2); i++)
	{
		/*View all these three lines in serial terminal to see which frequencies has which amplitudes*/

//		Serial.print((i * 1.0 * samplingFrequency) / SAMPLING_SIZE, 1);
//		Serial.print(" ");
		Serial.println(ax_in[i], 1);    //View only this line in serial plotter to visualize the bins
	}

	Serial.println("Restart");
	delay(1000);
}

//	for (;;) {
//		int32_t accelerometer[3];
////		int32_t gyroscope[3];
//		imu->Get_X_Axes(accelerometer);
////		imu->Get_G_Axes(gyroscope);
//
//		// Output data.
////		ESP_LOGI("IMU", "Acc[mg]: %d\t%d\t%d", accelerometer[0], accelerometer[1], accelerometer[2]);
//		Serial.println(accelerometer[0]);
//
////		ESP_LOGI("IMU", "Gyr[mdps]: %d\t%d\t%d", gyroscope[0], gyroscope[1], gyroscope[2]);
//
//		/*
//		currentMillis = millis();
//		if (currentMillis - previousMillis >= delayTime) {
//			// save the last time accel data is collected
//			previousMillis = currentMillis;
//
//			//int32_t gyroscope[3];
//			imu->Get_X_Axes(accelerometer);
//			//imu->Get_G_Axes(gyroscope);
//
//			ax_in[sample_counter] = (double)accelerometer[0];
//			ay_in[sample_counter] = (double)accelerometer[1];
//			az_in[sample_counter] = (double)accelerometer[2];
//			vImag_x[sample_counter] = 0;
//			vImag_y[sample_counter] = 0;
//			vImag_z[sample_counter] = 0;
//
//			sample_counter++;
//		}
//
//		if(sample_counter == SAMPLING_SIZE){
//
//		    sample_counter = 0;
//
////		    Serial.println("Data:");
////		    PrintVector(ax_in, ay_in, az_in, (uint16_t)SAMPLING_SIZE, (uint8_t)SCL_TIME);
////		    Serial.println();
////		    Serial.println();
//
//		    // calculate data
//		    FFT.Compute(ax_in, vImag_x, (uint16_t)SAMPLING_SIZE, FFT_FORWARD);
//		    FFT.Compute(ay_in, vImag_y, (uint16_t)SAMPLING_SIZE, FFT_FORWARD);
//		    FFT.Compute(az_in, vImag_z, (uint16_t)SAMPLING_SIZE, FFT_FORWARD);
//
//		    Serial.println("Computed Real values:");
//		    PrintVector(ax_in, ay_in, az_in, (uint16_t)SAMPLING_SIZE, (uint8_t)SCL_INDEX);
//		    Serial.println();
//		    Serial.println("v2");
//
//		    FFT.ComplexToMagnitude(ax_in, vImag_x, (uint16_t)SAMPLING_SIZE);
//		    FFT.ComplexToMagnitude(ay_in, vImag_y, (uint16_t)SAMPLING_SIZE);
//		    FFT.ComplexToMagnitude(az_in, vImag_z, (uint16_t)SAMPLING_SIZE);
//
//		    // calculate data - average and max
//			average_0_x = average(ax_in, (int)RANGE_0);
//			average_1_x = average(ax_in, (int)RANGE_1);
//			average_2_x = average(ax_in, (int)RANGE_2);
//			average_3_x = average(ax_in, (int)RANGE_3);
//
//			max_0_x = max_value(ax_in, (int)RANGE_0);
//			max_1_x = max_value(ax_in, (int)RANGE_1);
//			max_2_x = max_value(ax_in, (int)RANGE_2);
//			max_3_x = max_value(ax_in, (int)RANGE_3);
//
//			average_0_y = average(ay_in, (int)RANGE_0);
//			average_1_y = average(ay_in, (int)RANGE_1);
//			average_2_y = average(ay_in, (int)RANGE_2);
//			average_3_y = average(ay_in, (int)RANGE_3);
//
//			max_0_y = max_value(ay_in, (int)RANGE_0);
//			max_1_y = max_value(ay_in, (int)RANGE_1);
//			max_2_y = max_value(ay_in, (int)RANGE_2);
//			max_3_y = max_value(ay_in, (int)RANGE_3);
//
//			average_0_z = average(az_in, (int)RANGE_0);
//			average_1_z = average(az_in, (int)RANGE_1);
//			average_2_z = average(az_in, (int)RANGE_2);
//			average_3_z = average(az_in, (int)RANGE_3);
//
//			max_0_z = max_value(az_in, (int)RANGE_0);
//			max_1_z = max_value(az_in, (int)RANGE_1);
//			max_2_z = max_value(az_in, (int)RANGE_2);
//			max_3_z = max_value(az_in, (int)RANGE_3);
//
//			data_ready = true;
//		}
//
//
//
//		communicate();*/
//
//		//delay(10);
//	}

}

#endif /* MAIN_EXPLORE_TEST_IMU_HPP_ */
