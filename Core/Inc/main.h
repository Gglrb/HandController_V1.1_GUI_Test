/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

typedef enum {
    SEL = 1,
    FWD = 2,
	BCK = 3,
	RTRN = 4,
} BUTTON_NAMES;

/* Stuff ---------------------------------------------------------------------*/
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void ACT_LED(uint8_t duty);
void Soft_Error(uint8_t code);
void Error_Handler(void);


/* Private defines -----------------------------------------------------------*/
#define I_S4_Pin GPIO_PIN_0
#define I_S4_GPIO_Port GPIOC
#define I_S6_Pin GPIO_PIN_1
#define I_S6_GPIO_Port GPIOC
#define I_S5_Pin GPIO_PIN_2
#define I_S5_GPIO_Port GPIOC
#define I_S2_Pin GPIO_PIN_3
#define I_S2_GPIO_Port GPIOC
#define PWM_S6_Pin GPIO_PIN_0
#define PWM_S6_GPIO_Port GPIOA
#define PWM_S5_Pin GPIO_PIN_1
#define PWM_S5_GPIO_Port GPIOA
#define LPUART1_TX_Pin GPIO_PIN_2
#define LPUART1_TX_GPIO_Port GPIOA
#define LPUART1_RX_Pin GPIO_PIN_3
#define LPUART1_RX_GPIO_Port GPIOA
#define I_S1_Pin GPIO_PIN_4
#define I_S1_GPIO_Port GPIOA
#define LD2_ONBOARD_Pin GPIO_PIN_5
#define LD2_ONBOARD_GPIO_Port GPIOA
#define I_S3_Pin GPIO_PIN_0
#define I_S3_GPIO_Port GPIOB
#define FWD_SW_Pin GPIO_PIN_1
#define FWD_SW_GPIO_Port GPIOB
#define FWD_SW_EXTI_IRQn EXTI1_IRQn
#define BCK_SW_Pin GPIO_PIN_2
#define BCK_SW_GPIO_Port GPIOB
#define BCK_SW_EXTI_IRQn EXTI2_IRQn
#define PWM_S3_Pin GPIO_PIN_10
#define PWM_S3_GPIO_Port GPIOB
#define S456_EN_Pin GPIO_PIN_11
#define S456_EN_GPIO_Port GPIOB
#define S123_EN_Pin GPIO_PIN_12
#define S123_EN_GPIO_Port GPIOB
#define SEL_SW_Pin GPIO_PIN_13
#define SEL_SW_GPIO_Port GPIOB
#define SEL_SW_EXTI_IRQn EXTI15_10_IRQn
#define ERR_D_Pin GPIO_PIN_14
#define ERR_D_GPIO_Port GPIOB
#define ACT_D_Pin GPIO_PIN_15
#define ACT_D_GPIO_Port GPIOB
#define OLED_RST_Pin GPIO_PIN_6
#define OLED_RST_GPIO_Port GPIOC
#define SAFE_OV_Pin GPIO_PIN_7
#define SAFE_OV_GPIO_Port GPIOC
#define SAFE_FB_Pin GPIO_PIN_8
#define SAFE_FB_GPIO_Port GPIOC
#define SAFE_FB_EXTI_IRQn EXTI9_5_IRQn
#define PWM_S4_Pin GPIO_PIN_8
#define PWM_S4_GPIO_Port GPIOA
#define PWM_S1_Pin GPIO_PIN_9
#define PWM_S1_GPIO_Port GPIOA
#define PWM_S2_Pin GPIO_PIN_10
#define PWM_S2_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define COM_BT_TX_Pin GPIO_PIN_10
#define COM_BT_TX_GPIO_Port GPIOC
#define COM_BT_RX_Pin GPIO_PIN_11
#define COM_BT_RX_GPIO_Port GPIOC
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_8
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_9
#define OLED_SDA_GPIO_Port GPIOB


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
