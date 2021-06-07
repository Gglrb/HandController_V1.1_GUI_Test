/*
 * Servo.c
 *
 *  Created on: Feb 20, 2021
 *      Author: gauth
 */

#include "servo.h"
#include "main.h"
#include "math.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

//#define htim1_ARR htim1.Init.Period
//#define htim2_ARR htim2.Init.Period

uint8_t SelectedServo = 1;
uint8_t defPos[] = {90, 90, 90, 90, 90, 90};  //Default servo positions
uint8_t spos[6];  //Actual servo positions

#define N_servos 6

// Servo 1 config
float CCR1_Max;
float CCR1_Min;
float t1_Min = 0.6;
float t1_Max = 2.4;

// Servo 2 config
float CCR2_Max;
float CCR2_Min;
float t2_Min = 0.6;
float t2_Max = 2.4;

// Servo 3 config
float CCR3_Max;
float CCR3_Min;
float t3_Min = 0.6;
float t3_Max = 2.4;

// Servo 4 config
float CCR4_Max;
float CCR4_Min;
float t4_Min = 0.6;
float t4_Max = 2.4;

// Servo 5 config
float CCR5_Max;
float CCR5_Min;
float t5_Min = 0.6;
float t5_Max = 2.4;

// Servo 6 config
float CCR6_Max;
float CCR6_Min;
float t6_Min = 0.6;
float t6_Max = 2.4;


void Servo_Init(void)
{
	float T = 0.02;  // PWM period (50Hz => T = 1/50 = 0.02)

	float S1_DutyMin = ((t1_Min /  T) / 1000);
	float S1_DutyMax = ((t1_Max /  T) / 1000);
	CCR1_Min = (S1_DutyMin * htim1.Init.Period);
	CCR1_Max = (S1_DutyMax * htim1.Init.Period);

	float S2_DutyMin = ((t2_Min /  T) / 1000);
	float S2_DutyMax = ((t2_Max /  T) / 1000);
	CCR2_Min = (S2_DutyMin * htim1.Init.Period);
	CCR2_Max = (S2_DutyMax * htim1.Init.Period);

	float S3_DutyMin = ((t3_Min /  T) / 1000);
	float S3_DutyMax = ((t3_Max /  T) / 1000);
	CCR3_Min = (S3_DutyMin * htim2.Init.Period);
	CCR3_Max = (S3_DutyMax * htim2.Init.Period);

	float S4_DutyMin = ((t4_Min /  T) / 1000);
	float S4_DutyMax = ((t4_Max /  T) / 1000);
	CCR4_Min = (S4_DutyMin * htim1.Init.Period);
	CCR4_Max = (S4_DutyMax * htim1.Init.Period);

	float S5_DutyMin = ((t5_Min /  T) / 1000);
	float S5_DutyMax = ((t5_Max /  T) / 1000);
	CCR5_Min = (S5_DutyMin * htim2.Init.Period);
	CCR5_Max = (S5_DutyMax * htim2.Init.Period);

	float S6_DutyMin = ((t6_Min /  T) / 1000);
	float S6_DutyMax = ((t6_Max /  T) / 1000);
	CCR6_Min = (S6_DutyMin * htim2.Init.Period);
	CCR6_Max = (S6_DutyMax * htim2.Init.Period);

	for(uint8_t i = N_servos; i > 0; i--)  //Copy default servo positions to actual position
	{
		spos[i-1] = defPos[i-1];
	}

}

void Servo_Start(uint8_t servo)
{
	uint8_t s = 0;

	switch(servo)
	{
	case ALL:
		s = s + 1;
	case S123:
		s = s + 2;
	case S456:
		if(servo == S456 || servo == ALL)
		{
			s = s + 4;
		}
	case 1:
		if((s == 2) || (s == 7) || (servo == 1))
		{
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
		}
		else
		{
			break;
		}
	case 2:
		if((s == 2) || (s == 7) || (servo == 2))
		{
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		}
		else
		{
			break;
		}
	case 3:
		if((s == 2) || (s == 7) || (servo == 3))
		{
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		}
		else
		{
			break;
		}
	case 4:
		if((s == 4) || (s == 7) || (servo == 4))
		{
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		}
		else
		{
			break;
		}
	case 5:
		if((s == 4) || (s == 7) || (servo == 5))
		{
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		}
		else
		{
			break;
		}
	case 6:
		if((s == 4) || (s == 7) || (servo == 6))
		{
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		}
		else
		{
			break;
		}
	}
}

void Servo_Stop(uint8_t servo)
{
	uint8_t s = 0;

	switch(servo)
	{
	case ALL:
	{
		s = s + 1;
	}
	case S123:
	{
		s = s + 2;
	}
	case S456:
	{
		if(servo == S456 || servo == ALL)
		{
			s = s + 4;
		}
	}
	case 1:
	{
		if((s == 2) || (s == 7) || (servo == 1))
		{
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
		}
		else
		{
			break;
		}
	}
	case 2:
	{
		if((s == 2) || (s == 7) || (servo == 2))
		{
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
		}
		else
		{
			break;
		}
	}
	case 3:
	{
		if((s == 2) || (s == 7) || (servo == 3))
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
		}
		else
		{
			break;
		}
	}
	case 4:
	{
		if((s == 4) || (s == 7) || (servo == 4))
		{
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		}
		else
		{
			break;
		}
	}
	case 5:
	{
		if((s == 4) || (s == 7) || (servo == 5))
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
		}
		else
		{
			break;
		}
	}
	case 6:
	{
		if((s == 4) || (s == 7) || (servo == 6))
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
		}
		else
		{
			break;
		}
	}
	}
}

uint8_t Servo_Move(uint8_t pos, uint8_t servo)  //, uint32_t time
{
	if(pos <= 0)
	{
		pos = 0;
	}
	if(pos >= 180)
	{
		pos = 180;
	}
	float CCR_Pos = Map(pos, 0, 180, CCR1_Min, CCR1_Max);
	uint32_t CCR_Pos_INT = (uint32_t)CCR_Pos;
	if(servo == 1)
	{
		TIM1->CCR2 = CCR_Pos_INT;
	}
	if(servo == 2)
	{
		TIM1->CCR3 = CCR_Pos_INT;
	}
	if(servo == 3)
	{
		TIM2->CCR3 = CCR_Pos_INT;
	}
	if(servo == 4)
	{
		TIM1->CCR1 = CCR_Pos_INT;
	}
	if(servo == 5)
	{
		TIM2->CCR2 = CCR_Pos_INT;
	}
	if(servo == 6)
	{
		TIM2->CCR1 = CCR_Pos_INT;
	}
	return servo;
}
