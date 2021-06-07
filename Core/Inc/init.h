/*
 * init.h
 *
 *  Created on: May 22, 2021
 *      Author: gauth
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_LPUART1_UART_Init(void);
void MX_ADC1_Init(void);
void MX_ADC2_Init(void);
void MX_I2C1_Init(void);
void MX_UART4_Init(void);
void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM5_Init(void);
void MX_TIM15_Init(void);

#endif /* INC_INIT_H_ */
