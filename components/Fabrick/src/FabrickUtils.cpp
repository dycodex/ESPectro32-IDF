/*
 * FabricUtils.cpp
 *
 *  Created on: Aug 24, 2017
 *      Author: andri
 */

#include <sstream>
#include <iomanip>
#include "FabrickUtils.h"

void FabrickUtils::addZerosToChar(char *dest, const char *src, unsigned width) {
	size_t len = strlen(src);
	size_t zeros = (len > width) ? 0 : width - len;
	memset(dest, '0', zeros);
	strcpy(dest + zeros, src);
}

std::string FabrickUtils::addZerosToString(std::string str, unsigned width) {
	std::ostringstream ss;
	ss << std::setw(5) << std::setfill('0') << str;
	return ss.str();
}

String FabrickUtils::addZerosToString(String data, unsigned len) {
    int dataLength = data.length();
    String dataii = data;
    if (dataLength < len) {
        for (int ii = 0; ii < len - dataLength; ii++) {
            dataii = "0" + dataii;
        }
    }

    return dataii;
}
