/*
 * SSM2518.h
 *
 *  Created on: Sep 23, 2017
 *      Author: andri
 */

#ifndef MAIN_SSM2518_H_
#define MAIN_SSM2518_H_

#include <esp_log.h>
#include <stdio.h>
#include <esp_types.h>
#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/i2s.h"
#include "soc/soc.h"

// GPIO pads for the control bus (I2C)
#define SSM2518_I2C_SCL_IO		22
#define SSM2518_I2C_SDA_IO		21
#define SSM2518_I2C_ADDR  		0x34	// AudioBit slave address

#define SSM2518_I2C_NUM 			1			// I2C module number
#define SSM2518_I2C_FREQ_HZ    	100000		// Master clock frequency (Hz)

#define SSM2518_DEBUG_PRINT(...)  ESP_LOGD("SSM2518", __VA_ARGS__);
#define SSM2518_INFO_PRINT(...)   ESP_LOGI("SSM2518", __VA_ARGS__);
#define SSM2518_ERROR_PRINT(...)  ESP_LOGE("SSM2518", __VA_ARGS__);

enum
{
	SSM2518_Reset_Power_Control							= 0x00,
	SSM2518_Edge_Clock_Control							= 0x01,
	SSM2518_Serial_Interface_Sample_Rate_Control			= 0x02,
	SSM2518_Serial_Interface_Control						= 0x03,
	SSM2518_Channel_Mapping_Control						= 0x04,
	SSM2518_Left_Volume_Control							= 0x05,
	SSM2518_Right_Volume_Control							= 0x06,
	SSM2518_Volume_Mute_Control							= 0x07,
	SSM2518_Fault_Control_1								= 0x08,
	SSM2518_Power_Fault_Control  						= 0x09,
	SSM2518_DRC_Control_1								= 0x0A,
	SSM2518_DRC_Control_2								= 0x0B,
	SSM2518_DRC_Control_3								= 0x0C,
	SSM2518_DRC_Control_4								= 0x0D,
	SSM2518_DRC_Control_5								= 0x0E,
	SSM2518_DRC_Control_6								= 0x0F,
	SSM2518_DRC_Control_7								= 0x10,
	SSM2518_DRC_Control_8								= 0x11,
	SSM2518_DRC_Control_9								= 0x12
};

#define SSM2518_POWER1_SPWDN			BIT(0)
#define SSM2518_POWER1_NO_BCLK		BIT(5)
#define SSM2518_POWER1_MCS_MASK		(0xf << 1)
#define SSM2518_POWER1_MCS_64FS		(0x0 << 1)
#define SSM2518_POWER1_MCS_128FS		(0x1 << 1)
#define SSM2518_POWER1_MCS_256FS		(0x2 << 1)
#define SSM2518_POWER1_MCS_384FS		(0x3 << 1)
#define SSM2518_POWER1_MCS_512FS		(0x4 << 1)
#define SSM2518_POWER1_MCS_768FS		(0x5 << 1)
#define SSM2518_POWER1_MCS_100FS		(0x6 << 1)
#define SSM2518_POWER1_MCS_200FS		(0x7 << 1)
#define SSM2518_POWER1_MCS_400FS		(0x8 << 1)
#define SSM2518_POWER1_RESET			BIT(7)

#define SSM2518_MUTE_CTRL_ANA_GAIN			BIT(5)
#define SSM2518_MUTE_CTRL_MUTE_MASTER		BIT(0)

class SSM2518 {
public:
	SSM2518();
	virtual ~SSM2518();

	bool begin();
	bool setPower(bool enable);

	bool setVolume(int8_t left_vol, int8_t right_vol);
	bool setMute(int mute);

private:
	bool initRegs();
	esp_err_t writeReg(i2c_port_t i2c_num, uint8_t reg_addr, uint8_t reg_val);
	esp_err_t readReg (i2c_port_t i2c_num, uint8_t reg_addr, uint8_t *reg_val);

	esp_err_t updateRegBits (uint8_t reg_addr, uint8_t mask, uint8_t val, bool *changed = NULL, bool force_write = false);
};

#endif /* MAIN_SSM2518_H_ */
