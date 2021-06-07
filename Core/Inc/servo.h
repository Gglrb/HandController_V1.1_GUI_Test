/*
 * Servo.h
 *
 *  Created on: Feb 20, 2021
 *      Author: gauth
 */

#ifndef SERVO_SERVO_H_
#define SERVO_SERVO_H_

#include "stm32g4xx_hal.h"

typedef enum {
    ALL = 7,
	S123 = 8,
	S456 = 9,
} SERVO_START;

void Servo_Init(void);
void Servo_Start(uint8_t servo);
void Servo_Stop(uint8_t servo);
uint8_t Servo_Move(uint8_t Pos, uint8_t servo);

#endif /* SERVO_SERVO_H_ */
