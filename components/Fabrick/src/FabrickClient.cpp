/*
 * FabrickClient.cpp
 *
 *  Created on: Aug 11, 2017
 *      Author: andri
 */

#include "FabrickClient.h"

FabrickLoraClientClass::FabrickLoraClientClass() {
}

FabrickLoraClientClass::~FabrickLoraClientClass() {
}

FabrickLoraClientClass FabrickLoraClient;

void FabrickLoraClientClass::begin(Stream* stream) {
	loraStream_ = stream;
}

void FabrickLoraClientClass::send(String deviceId, int frameCtr,
		int dataChannel, int dataType, String data, int dataLenInHex) {

	// Please refer to IPSO Smart Objects Document for packet format.

	// Device ID
	String device_id_s = deviceId;

	// Frame counter
	// Convert to string and ensure length of 2 (in hex) or 1 byte
	String frame_cntr_s = addZeros(String(frameCtr, HEX), 2);

	// Data channel
	// Convert to string and ensure length of 2 (in hex) or 1 byte
	String data_ch_s = addZeros(String(dataChannel, HEX), 2);

	// Data type
	// Convert to string and ensure length of 2 (in hex) or 1 byte
	String data_type_s = addZeros(String(dataType - 3200, HEX), 2);

	// Payload data
	// Ensure length of data_len_in_hex
	String data_s = addZeros(data, dataLenInHex);

	// Tx packet
	String tx_packet = device_id_s + frame_cntr_s + data_ch_s + data_type_s + data_s;

	// Length of the Tx packet
	int tx_packet_len = int(tx_packet.length());

	// Initialisation
	char at_command[tx_packet_len + 12];

	// Send data via LoRa
	String len_in_string = String(tx_packet_len);           						// Convert to string
	String at_command_s = "AT+DTX=" + len_in_string + "," + tx_packet + "\r\n"; 	// AT command in string
	at_command_s.toCharArray(at_command, tx_packet_len + 12); 					// Convert to char
	loraStream_->write(at_command);                        						// AT command to LoRa

	// Debug mode print
	//if (debug_mode){
	FABRICK_DEBUG_PRINT("LoRa send -> ");
	FABRICK_DEBUG_PRINT("%s", at_command_s.c_str());
	FABRICK_DEBUG_PRINT(" ");
	//}
}

String FabrickLoraClientClass::addZeros(String data, int len) {
	// Length of the data
	int data_len = data.length();

	// Adding zeros
	String data_ii = data;
	if (data_len < len) {
		for (int ii = 0; ii < len - data_len; ii++) {
			data_ii = "0" + data_ii;
		}
	}

	return data_ii;
}
