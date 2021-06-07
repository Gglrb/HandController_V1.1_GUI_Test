/*
 * GUI.h
 *
 *  Created on: 23 May 2021
 *      Author: gauth
 */

#ifndef INC_GUI_H_
#define INC_GUI_H_

#include "main.h"

uint8_t ustat(uint8_t stat);
uint32_t constrain(uint32_t val, uint32_t min, uint32_t max, uint8_t type);
void GUI(I2C_HandleTypeDef *hi2c, uint8_t key);
void GUI_ULayer(I2C_HandleTypeDef *hi2c);

typedef enum {
    Halted = 0,
    Running = 1,
	Ready = 2,
	Paused = 3,
} STATUS_NAMES;

typedef enum {
	Circular = 0,
	Constrained = 1,
} CONSTRAIN_TYPES;

#endif /* INC_GUI_H_ */
