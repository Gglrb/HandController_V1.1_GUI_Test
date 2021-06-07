/*
 * Math.c
 *
 *  Created on: 20 Feb 2021
 *      Author: gauth
 */

#include "math.h"

uint32_t Map(uint32_t x, uint32_t xMin, uint32_t xMax, uint32_t yMin, uint32_t yMax)
{
	uint32_t y = ((((x - xMin) * (yMax - yMin)) / (xMax - xMin)) + yMin);
	return y;
}

