/*
 * SSM2518.cpp
 *
 *  Created on: Sep 23, 2017
 *      Author: andri
 */

#include "SSM2518.h"

#define WRITE_BIT  				I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT   				I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN   			0x1     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS  			0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL    				0x0         /*!< I2C ack value */
#define NACK_VAL   				0x1         /*!< I2C nack value */

#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */

SSM2518::SSM2518() {
	// TODO Auto-generated constructor stub

}

SSM2518::~SSM2518() {
	// TODO Auto-generated destructor stub
}


bool SSM2518::begin() {

	int i2c_master_port = SSM2518_I2C_NUM;
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = (gpio_num_t)SSM2518_I2C_SDA_IO;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = (gpio_num_t)SSM2518_I2C_SCL_IO;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = SSM2518_I2C_FREQ_HZ;
	i2c_param_config((i2c_port_t)i2c_master_port, &conf);
	i2c_driver_install((i2c_port_t)i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);


	esp_err_t ret;
	uint8_t readval;

//	setPower(false);
//
//	ret = readReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Reset_Power_Control, &readval);
//	SSM2518_INFO_PRINT("Reg: %d, Val: %d\n", SSM2518_Reset_Power_Control, readval);
//
//	SSM2518_INFO_PRINT("Writing...");
//	setPower(true);
//	//ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Reset_Power_Control, 0x04);
//	//ret = updateRegBits(SSM2518_Reset_Power_Control, SSM2518_POWER1_SPWDN | SSM2518_POWER1_RESET, 0x00);
//
//	ret = readReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Reset_Power_Control, &readval);
//	SSM2518_INFO_PRINT("Reg: %d, Val: %d\n", SSM2518_Reset_Power_Control, readval);

	ret = initRegs();

	return true;
}

bool SSM2518::initRegs() {

	SSM2518_INFO_PRINT("\n\rInitializing SSM2518 via I2C...");
	esp_err_t ret;

	//ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Reset_Power_Control, 							0x04);

	ret = setPower(true);

	ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Edge_Clock_Control, 						0x00);
	ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Serial_Interface_Sample_Rate_Control, 	0x00);
	ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Serial_Interface_Control, 				0x00);
	ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Channel_Mapping_Control, 					0x01);
	//ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Left_Volume_Control, 					0x70);
	//ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Right_Volume_Control, 					0x70);

	ret = setVolume(-10, -10); //-10db

	ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Volume_Mute_Control, 						0x80);
	ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Fault_Control_1, 							0x0C);
	ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Power_Fault_Control, 						0x99);

	SSM2518_INFO_PRINT(" Done!\n\r");

	return (ret == ESP_OK);
}

esp_err_t SSM2518::writeReg(i2c_port_t i2c_num, uint8_t reg_addr,
		uint8_t reg_val) {

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	    uint8_t dwr[4];

	// Start condition
	i2c_master_start(cmd);
	// Address + Write bit
	i2c_master_write_byte(cmd, (SSM2518_I2C_ADDR<<1)|WRITE_BIT, ACK_CHECK_EN);
	// LSB for reg address
	dwr[0] = reg_addr;
	// LSB for reg data
	dwr[1] = reg_val;
	i2c_master_write(cmd, dwr, 2, ACK_CHECK_EN);
	i2c_master_stop(cmd);

	// Execute and return status
	esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t SSM2518::readReg(i2c_port_t i2c_num, uint8_t reg_addr,
		uint8_t* reg_val) {

	uint8_t *byte_val = reg_val;		// This will cause warning, please ignore
	    esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	// Start condition
	i2c_master_start(cmd);
	// Address + Write bit
	i2c_master_write_byte(cmd, (SSM2518_I2C_ADDR<<1)|WRITE_BIT, ACK_CHECK_EN);
	// MSB for reg address
	i2c_master_write_byte(cmd, reg_addr, ACK_CHECK_EN);

	// Restart (stop + start)
	i2c_master_start(cmd);

	// Address + read
	i2c_master_write_byte(cmd, (SSM2518_I2C_ADDR<<1)|READ_BIT, ACK_CHECK_EN);

	// MSB for reg data
	i2c_master_read_byte(cmd, byte_val, I2C_MASTER_ACK);//I2C_MASTER_NACK);//NACK_VAL);

	i2c_master_stop(cmd);
	// Execute and return status, should return 0
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

bool SSM2518::setPower(bool enable) {
	esp_err_t ret;

	if (!enable) {
		ret = updateRegBits(SSM2518_Reset_Power_Control,
			SSM2518_POWER1_SPWDN, SSM2518_POWER1_SPWDN);
	} else {
		ret = updateRegBits(SSM2518_Reset_Power_Control,
			SSM2518_POWER1_SPWDN | SSM2518_POWER1_RESET, 0x00);
	}

	return (ret == ESP_OK);
}

bool SSM2518::setVolume(int8_t left_vol, int8_t right_vol) {
	uint8_t left, right;

	left = -1 * (left_vol - 24) / 0.375;
	right = -1 * (right_vol - 24) / 0.375;

	SSM2518_INFO_PRINT("Set vols: %d %d", left, right);

	esp_err_t ret;
	ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Right_Volume_Control, right);
	ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, SSM2518_Left_Volume_Control, left);

	return (ret == ESP_OK);
}

bool SSM2518::setMute(int mute) {
	unsigned int val;
	if (mute)
		val = SSM2518_MUTE_CTRL_MUTE_MASTER;
	else
		val = 0;

	SSM2518_INFO_PRINT("Set mute: %d", val);

	esp_err_t ret = updateRegBits(SSM2518_Volume_Mute_Control, SSM2518_MUTE_CTRL_MUTE_MASTER, val);
	return (ret == ESP_OK);
}

esp_err_t SSM2518::updateRegBits(uint8_t reg_addr, uint8_t mask, uint8_t val, bool *changed, bool force_write) {

	esp_err_t ret;
	uint8_t orig, tmp;

	if (changed)
		*changed = false;

	ret = readReg((i2c_port_t)SSM2518_I2C_NUM, reg_addr, &orig);
	if (ret != ESP_OK) {
		return ret;
	}

	tmp = orig & ~mask;
	tmp |= val & mask;

	if (force_write || (tmp != orig)) {
		ret = writeReg((i2c_port_t)SSM2518_I2C_NUM, reg_addr, tmp);
		if (ret == ESP_OK && changed)
			*changed = true;
	}

	return ret;
}
