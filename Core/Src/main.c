/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "init.h"
#include "ssd1306.h"
#include "fonts.h"
#include "bitmaps.h"
#include "GUI.h"
#include "math.h"
#include "servo.h"

/* variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_lpuart1_rx;
DMA_HandleTypeDef hdma_uart4_rx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim15;

uint8_t S_STATE = 1;  //Safety software status
uint8_t S_ERR = 0;  //Safety error

uint8_t bstate[] =
{ 1, 1, 1 };  // Button states (SEL[0], FWD[1], BCK[2])

//Serial Port buffer
uint8_t rx1buffer = 0;   //RX1 data buffer
uint8_t livebuff[20];    //RX1 live buffer table
uint8_t buffloc = 0;     //RX1 live buffer filled location

extern uint8_t buffloc;
extern uint8_t displine;
extern uint8_t enableterm;

extern uint8_t vol_mem[];

int main(void)
{
	/* MCU Configuration------------------------------------------------------*/
	HAL_Init();
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_LPUART1_UART_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_I2C1_Init();
	MX_UART4_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_TIM5_Init();
	MX_TIM15_Init();

	ssd1306_Init(&hi2c1);


	HAL_GPIO_WritePin(SAFE_OV_GPIO_Port, SAFE_OV_Pin, GPIO_PIN_SET);

	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2);

	GUI(&hi2c1, 0);

	Servo_Init();
	Servo_Start(ALL);

	Soft_Error(0x01);
	while (1)
	{

	}
}

void ACT_LED(uint8_t duty)
{
	//HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2);
	uint16_t CCR = ((htim15.Init.Period / 255) * duty);
	TIM15->CCR2 = CCR;
}

/* Hardware interrupts ------------------------------------------------------------*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == SAFE_FB_Pin)
	{
		if ((S_STATE == 1 || S_ERR == 1)
				&& HAL_GPIO_ReadPin(SAFE_FB_GPIO_Port, SAFE_FB_Pin))
		{
			Soft_Error(0x01);  //Emergency stop error
		}
		else
		{
			/* if suposed state is not equal to currnt state */
			vol_mem[3] ^= 1;
			GUI(&hi2c1, 0);
		}
	}

	else if (GPIO_Pin == SEL_SW_Pin && bstate[SEL - 1] == 1) //Verify which button has been pressed and that it hasn't been pressed too quickly
	{
		HAL_TIM_Base_Start_IT(&htim5);  //Start timer
		bstate[0] = 0;  //Disables new button presses
		if (S_ERR != 1)  //Disable GUI when error reported
		{
			GUI(&hi2c1, SEL);  //Register action in GUI
		}
	}

	else if (GPIO_Pin == FWD_SW_Pin && bstate[FWD - 1] == 1)
	{
		bstate[1] = 0;
		HAL_TIM_Base_Start_IT(&htim5);
		if (S_ERR != 1)  //Disable GUI when error reported
		{
			GUI(&hi2c1, FWD);  //Register action in GUI
		}
	}

	else if (GPIO_Pin == BCK_SW_Pin && bstate[BCK - 1] == 1)
	{
		HAL_TIM_Base_Start_IT(&htim5);
		bstate[2] = 0;
		if (S_ERR != 1)  //Disable GUI when error reported
		{
			GUI(&hi2c1, BCK);  //Register action in GUI
		}
	}
}

/* Timer Interrupts ---------------------------------------------------------------*/

/* Debouncing
 * When a button is pressed, it checks which button has been pressed and starts timer 5.
 * As long as the timer period hasn't been reached, the pressed button cannot register
 * any new button presses, thus preventing any accidental interpretation of a bouncing contact.
 * Once timer 5 finishes its count (here ...ms), the timer is stopped and reset, and
 * the before-pressed button can again register new button presses.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (bstate[SEL - 1] == 0)   //Verify if select button has been pressed
	{
		HAL_TIM_Base_Stop_IT(&htim5);  //Stop timer
		bstate[SEL - 1] = 1; //Enable select button to register new button presses
	}
	if (bstate[FWD - 1] == 0)   //Verify if forward button has been pressed
	{
		HAL_TIM_Base_Stop_IT(&htim5);
		bstate[FWD - 1] = 1;
	}
	if (bstate[BCK - 1] == 0)   //Verify if back button has been pressed
	{
		HAL_TIM_Base_Stop_IT(&htim5);
		bstate[BCK - 1] = 1;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *hlpuart)
{
	HAL_UART_Receive_IT(&hlpuart1, &rx1buffer, 1);
	livebuff[buffloc] = rx1buffer;
	buffloc++;
	if(enableterm == 1)
	{
		if (rx1buffer == 10)   //Line Feed
		{
			buffloc = buffloc - 1;
			displine++;
		}
		if (rx1buffer == 13)   //Carriage return
		{
			/*for(uint8_t j = buffloc; j > 0; j--)
			{
				storebuff[storeloc] = livebuff[]
			}
			storeloc++;*/
			buffloc = 0;
		}
		if (rx1buffer == 8)   //Backspace
		{
			ssd1306_EraseLine(7, displine * 10, Font_7x10, buffloc, Black);
			buffloc = 0;
		}
		if (rx1buffer == 127)   //Delete
		{
			ssd1306_Fill(Black);
			displine = 0;
			buffloc = 0;
		}
		else
		{
			//to do
			//find way to scroll down
			//empty writebuffer when Delete
			//pass writebuffer data to other buffer to be interpreted then empty writebuffer
			//add header Rn: receive when receiving live data, En:exectuted when CR (n for lne number) (with > arrows?)
			//add error/return command echo
			//keep command history. when opening live terminal, start on new line. when going back, go back into history
			//scroll bar look idea (up H for history, then scroll bar tha changes size with amount of lines. arrow down to say
			//where new line is)
			//new line starts on line 0 then keeps in middle all next lines.
			//
			//add manual overide
			//add live data/raw
			//add bt terminal
			//enhance menu

			ssd1306_EraseLine(0, displine * 10, Font_7x10, buffloc, Black);
			for(uint8_t i = buffloc; i > 0; i--)
			{
				ssd1306_SetCursor(((i - 1)*7)+7, displine * 10);
				ssd1306_WriteChar(livebuff[i-1], Font_7x10, White);
			}
		}
		ssd1306_SetCursor(0, displine * 10);
		ssd1306_WriteChar(62, Font_7x10, White);   //>
		ssd1306_UpdateScreen(&hi2c1);
	}
}

/* Error Handling ------------------------------------------------------------*/

void Soft_Error(uint8_t code)  //Soft software Errors
{
	__disable_irq();  //Disable interrupt requests

	/* Trigger emergency stop */
	HAL_GPIO_WritePin(SAFE_OV_GPIO_Port, SAFE_OV_Pin, GPIO_PIN_RESET);

	S_ERR = 1;

	//HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
	ACT_LED(0);
	HAL_GPIO_WritePin(ERR_D_GPIO_Port, ERR_D_Pin, GPIO_PIN_SET);
	ssd1306_Fill(Black);
	ssd1306_DrawLineH(0, 127, 10, White);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("ERROR: 0x0", Font_7x10, White);
	ssd1306_WriteVar(code, Font_7x10, White);  //Write Error code
	ustat(Halted);  //update status to "Halted" mode

	if (code == 0x01)  //Emergency stop code
	{
		ssd1306_SetCursor(0, 12);
		ssd1306_WriteString("E-STOP", Font_11x18, White);
	}

	else
	{
		Error_Handler();  //If none of the errors above, Hard software error
	}

	ssd1306_UpdateScreen(&hi2c1);

	/* Error GUI */
	__enable_irq();  //Enable interrupt requests
	while (bstate[SEL - 1] == 1)  //Wait for select key to be pressed once
	{
		HAL_GPIO_TogglePin(ERR_D_GPIO_Port, ERR_D_Pin);
		for (uint32_t i = 4560000; i > 0; i--)
			;
	}

	HAL_GPIO_WritePin(ERR_D_GPIO_Port, ERR_D_Pin, GPIO_PIN_SET);
	ACT_LED(30);
	ssd1306_EraseLine(0, 12, Font_11x18, 11, Black);
	ssd1306_SetCursor(0, 12);
	ssd1306_WriteString("SAFE?", Font_11x18, White);
	ssd1306_SetCursor(94, 12);
	ssd1306_WriteString("ACK", Font_11x18, Black);
	ssd1306_UpdateScreen(&hi2c1);

	while (bstate[SEL - 1] == 0)
		;  //Wait for select key to be released
	while (bstate[SEL - 1] == 1)
		;  //Wait for select key to be pressed a second time for confirmation
	HAL_GPIO_WritePin(ERR_D_GPIO_Port, ERR_D_Pin, GPIO_PIN_RESET);
	ACT_LED(185);
	HAL_GPIO_WritePin(SAFE_OV_GPIO_Port, SAFE_OV_Pin, GPIO_PIN_SET);
	S_ERR = 0;
	S_STATE = 0;
	GUI(&hi2c1, 0);

}

void Error_Handler(void)  //Hard firmware Error
{
	__disable_irq();  //Disable interrupt requests

	/* Trigger emergency stop */
	HAL_GPIO_WritePin(SAFE_OV_GPIO_Port, SAFE_OV_Pin, GPIO_PIN_RESET);

	/* Disable all timer outputs */
	__HAL_TIM_MOE_DISABLE(&htim1);
	__HAL_TIM_MOE_DISABLE(&htim2);
	__HAL_TIM_MOE_DISABLE(&htim5);
	__HAL_TIM_MOE_DISABLE(&htim15);

	/* Disable all power supplies */
	HAL_GPIO_WritePin(S123_EN_GPIO_Port, S123_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(S456_EN_GPIO_Port, S456_EN_Pin, GPIO_PIN_RESET);

	/* Disable all timers */
	__HAL_TIM_DISABLE(&htim1);
	__HAL_TIM_DISABLE(&htim2);
	__HAL_TIM_DISABLE(&htim5);
	__HAL_TIM_DISABLE(&htim15);

	S_ERR = 1;

	HAL_GPIO_WritePin(ERR_D_GPIO_Port, ERR_D_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ACT_D_GPIO_Port, ACT_D_Pin, GPIO_PIN_RESET);
	ssd1306_Fill(Black);
	ssd1306_DrawLineH(0, 127, 10, White);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("ERROR: FIRMW", Font_7x10, White);
	ssd1306_SetCursor(0, 12);
	ssd1306_WriteString("Hard Error", Font_11x18, White);
	ustat(Halted);  //update status to "Halted" mode
	ssd1306_UpdateScreen(&hi2c1);

	while (1)
	{
		HAL_GPIO_TogglePin(ERR_D_GPIO_Port, ERR_D_Pin);
		for (uint32_t i = 1230000; i > 0; i--);
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
