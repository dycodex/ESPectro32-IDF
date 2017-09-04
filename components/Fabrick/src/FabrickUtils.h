/*
 * FabricUtils.h
 *
 *  Created on: Aug 24, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_FABRICK_SRC_FABRICKUTILS_H_
#define COMPONENTS_FABRICK_SRC_FABRICKUTILS_H_

#include <string.h>
#include <string>
#include <WString.h>

class FabrickUtils {
public:
	static void addZerosToChar(char *dest, const char *src, unsigned width);
	static std::string addZerosToString(std::string str, unsigned width);
	static String addZerosToString(String data, unsigned len);
};

#endif /* COMPONENTS_FABRICK_SRC_FABRICKUTILS_H_ */
