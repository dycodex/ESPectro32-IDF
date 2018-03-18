/*
 * AloraUtils.cpp
 *
 *  Created on: Oct 18, 2017
 *      Author: andri
 */


#include "AloraUtils.h"

void printLoc( Print & outs, int32_t degE7 ) {
	// Extract and print negative sign
	if (degE7 < 0) {
		degE7 = -degE7;
		outs.print( '-' );
	}

	// Whole degrees
	int32_t deg = degE7 / 10000000L;
	outs.print( deg );
	outs.print( '.' );

	// Get fractional degrees
	degE7 -= deg*10000000L;

	// Print leading zeroes, if needed
	int32_t factor = 1000000L;
	while ((degE7 < factor) && (factor > 1L)){
		outs.print( '0' );
		factor /= 10L;
	}

	// Print fractional degrees
	outs.print( degE7 );
}

void printLocToString(String& str, int32_t degE7) {
	// Extract and print negative sign
	if (degE7 < 0) {
		degE7 = -degE7;
		str += '-' ;
	}

	// Whole degrees
	int32_t deg = degE7 / 10000000L;
	str += deg ;
	str += '.' ;

	// Get fractional degrees
	degE7 -= deg*10000000L;

	// Print leading zeroes, if needed
	//int32_t factor = 1000000L;
	int32_t factor = 10000L;
	while ((degE7 < factor) && (factor > 1L)){
		str += '0';
		factor /= 10L;
	}

	// Print fractional degrees
	str += degE7;
}

int32_t encodeLat(int32_t lat) {
	double b = (90.0 / pow(2, 23));
	double newLat = lat / 10000000.0;

	if (newLat < 0) {
		return (180 + newLat) / b;
	}

	return newLat / b;
}

int32_t encodeLon(int32_t lon) {
	double b = (180.0 / pow(2, 23));
	double newLon = lon / 10000000.0;

	if (newLon < 0) {
		return (360 + newLon) / b;
	}

	return newLon / b;
}
