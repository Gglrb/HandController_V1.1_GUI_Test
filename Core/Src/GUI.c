/*
 * GUI.c
 *
 *  Created on: 23 May 2021
 *      Author: gauth
 */

#include "GUI.h"
#include "ssd1306.h"
#include "servo.h"

extern uint8_t spos[];  //Actual servo positions

extern uint8_t bstate[];  // Button states (SEL[0], FWD[1], BCK[2])

uint8_t item = 1;     //Current Item
uint8_t itemact = 0;  //Current item action
uint8_t page = 1;     //Current page
uint8_t ppage = 1;    //Previous page
uint8_t layer = 0;    //Current layer

uint8_t b_action[] = {0, 2, 1, -1};  // Button actions (REFRESH[0], SEL[1], FWD[2], BCK[3])

uint8_t vol_mem[8] = {0, 0, 0, 0, 0, 0, 0, 0};  //Volatile memory for general use

//Serial port mem
uint8_t storebuff[20];   //Store buffer
uint8_t storeloc = 0;    //Store location

//Serial Port buffer
extern uint8_t rx1buffer;       //RX1 data buffer
extern uint8_t livebuff[20];    //RX1 live buffer table
extern uint8_t buffloc;         //RX1 live buffer filled location

uint8_t displine = 0;    //Current display line

uint8_t enableterm = 0;   //enable live terminal page


uint8_t ustat(uint8_t stat)
{
	ssd1306_SetCursor(95, 0);
	ssd1306_DrawBitmap(stat, Icon_32x9, White);
	return stat;
}


uint32_t constrain(uint32_t val, uint32_t min, uint32_t max, uint8_t type)
{
	if(val < min || val > max)   //Direction aware item limits
	{
		if(val < min)
		{
			if(type == Constrained)
			{
				val = min;
			}
			else
			{
				val = max;
			}
		}
		else if(val > max)
		{
			if(type == Constrained)
			{
				val = max;
			}
			else
			{
				val = min;
			}
		}
	}
	return val;
}


void GUI(I2C_HandleTypeDef *hi2c, uint8_t key)
{
	/* Key actions ----------------------------------------------------------------------*/
	if(key == SEL)   //Select key actions
	{
		if(layer == 0)   //Home layer
		{
			layer = item;
			item = 1;
		}

		else if(layer == 1)   //Info layer SEL key
		{
			layer = 0;
			item = 1;
		}

		else if(layer == 4)   //Power supplies layer
		{
			if(item == 4)  //Go back to main home
			{
				item = 4;
				layer = 0;
			}
			if(item == 1)
			{
				HAL_GPIO_TogglePin(S456_EN_GPIO_Port, S456_EN_Pin);
				vol_mem[0] ^= 1;
			}
			if(item == 2)
			{
				HAL_GPIO_TogglePin(S123_EN_GPIO_Port, S123_EN_Pin);
				vol_mem[1] ^= 1;
			}
			if(item == 3)
			{
				HAL_GPIO_TogglePin(SAFE_OV_GPIO_Port, SAFE_OV_Pin);
				vol_mem[2] ^= 1;
			}
		}

		else if(layer == 6)   //Movement layer
		{
			if(page == 1)  //Movement layer home
			{
				if(item == 4)  //Go back to main home
				{
					page = 1;
					item = 6;
					layer = 0;
				}
				else  //Enter selected page
				{
					page = item + 1;
					item = 1;
				}
			}
			if(page == 4)
			{
				if(item == 7)  //Go back to movement layer home
				{
					page = 1;
					item = 1;
				}
				else if(ppage != 1)
				{
					itemact = itemact ^= 1;
				}
			}
		}
		ssd1306_Fill(Black);
	}

	else if(key == RTRN)
	{
		layer = 0;
		item = 1;
		ssd1306_Fill(Black);
	}

	else if (key == 0)
	{
		ssd1306_Fill(Black);
	}

	else   //Back and Forward key actions
	{
		if(itemact == 1)
		{
			spos[item-1] = spos[item-1] + b_action[key];  //Increase/decrease spos value
			spos[item-1] = constrain(spos[item-1], 0, 180, Constrained);
			Servo_Move(spos[item-1], item);
		}
		else
		{
			item = item + b_action[key];
		}
	}
	//key = 0;   //Reset key request
	ppage = page;
	GUI_ULayer(hi2c);
}


void GUI_ULayer(I2C_HandleTypeDef *hi2c)
{
	/* Layers ---------------------------------------------------------------------------*/
	/* Home layer */
	if(layer == 0)
	{
		uint8_t itemcolour[] = {White, White, White, White, White, White};   //Default items colours
		uint8_t itembitmap[] = {0, 1, 2, 3, 5, 6};  //Item bitmaps
		uint8_t itemdescription[][13] = {"Info", "Live terminal", "BT terminal", "Power", "Live data", "Run"};
		uint8_t descriptionsize[] = {4, 13, 11, 5, 9, 3};
		uint8_t nitems = 6;   //Number of items

		item = constrain(item, 1, nitems, Circular);  //Direction aware item constraints

		itemcolour[item-1] = Black;   //Invert the selected item colour

		for(uint8_t i = 0; i < nitems; i++)
		{
			ssd1306_SetCursor(4 + (21*(i)), 15);
			ssd1306_DrawBitmap(itembitmap[i], Icon_16x16, itemcolour[i]); //Info
		}

		ssd1306_EraseLine(0, 0, Font_7x10, 13, Black);

		for(uint8_t j = descriptionsize[item-1]; j > 0; j--)
		{
			ssd1306_SetCursor(((j-1)*7), 0);
			ssd1306_WriteChar(itemdescription[item-1][j-1], Font_7x10, White);
		}
		ssd1306_DrawLineH(0, 127, 10, White);
		ustat(Ready);
	}


	/* Information layer */
	else if(layer == 1)
	{
		item = constrain(item, 1, 3, Circular);  //Direction aware page limits

		if(item == 1)   //Designer page
		{
			ssd1306_Fill(Black);
			ssd1306_SetCursor(0, 0);
			ssd1306_WriteString("Designed by:", Font_7x10, White);
			ssd1306_SetCursor(0, 11);
			ssd1306_WriteString("Gauthier Herbert", Font_7x10, White);
			ssd1306_DrawLineH(4, 42, 31, White);   //Scroll line
		}
		if(item == 2)   //Hardware revision page
		{
			ssd1306_Fill(Black);
			ssd1306_SetCursor(0, 0);
			ssd1306_WriteString("Hardware: ", Font_7x10, White);
			ssd1306_SetCursor(0, 11);
			ssd1306_WriteString("PotatoController", Font_7x10, White);
			ssd1306_SetCursor(0, 21);
			ssd1306_WriteString("V1.2  15/02/2021", Font_7x10, White);
			ssd1306_DrawLineH(44, 82, 31, White);   //Scroll line

		}
		if(item == 3)   //Software version page
		{

			ssd1306_Fill(Black);
			ssd1306_SetCursor(0, 0);
			ssd1306_WriteString("Software: ", Font_7x10, White);
			ssd1306_SetCursor(0, 11);
			ssd1306_WriteString("V0.6.8", Font_7x10, White);
			ssd1306_SetCursor(0, 21);
			ssd1306_WriteString("23/05/2021", Font_7x10, White);
			ssd1306_DrawLineH(84, 122, 31, White);   //Scroll line
		}
		ssd1306_DrawLineH(0, 2, 31, White);     //Scroll line end brackets
		ssd1306_DrawLineH(125, 127, 31, White);
	}



	else if(layer == 2)
	{
		if(enableterm == 0)
		{
			enableterm = 1;   //enable terminal

			//Update screen from buffer
			ssd1306_EraseLine(0, displine * 10, Font_7x10, buffloc, Black);
			ssd1306_SetCursor(0, displine * 10);
			ssd1306_WriteChar(62, Font_7x10, White);   //>
			for(uint8_t i = buffloc; i > 0; i--)
			{
				ssd1306_SetCursor(((i - 1)*7)+7, displine * 10);
				ssd1306_WriteChar(livebuff[i-1], Font_7x10, White);
			}
		}
	}



	else if(layer == 3)
	{

	}


	/* Power supply layer */
	else if(layer == 4)
	{
		uint8_t itemcolour[] = {White, White, White, White};   //Default items colours
		uint8_t nitems = 4;   //Number of items

		item = constrain(item, 1, nitems, Circular);  //Direction aware item limits

		itemcolour[item-1] = Black;   //Invert the selected item colour

		ssd1306_SetCursor(0, 0);
		ssd1306_WriteString("Power supplies", Font_7x10, White);
		ssd1306_DrawLineH(0, 127, 10, White);
		ssd1306_SetCursor(0, 12);
		ssd1306_WriteString("S654", Font_7x10, itemcolour[0]);
		ssd1306_SetCursor(32, 12);
		ssd1306_WriteString("S321", Font_7x10, itemcolour[1]);
		ssd1306_SetCursor(64, 12);
		ssd1306_WriteString("PWR_OV", Font_7x10, itemcolour[2]);
		ssd1306_SetCursor(0, 23);
		ssd1306_DrawBitmap(vol_mem[0], Icon_8x8, White);
		ssd1306_SetCursor(32, 23);
		ssd1306_DrawBitmap(vol_mem[1], Icon_8x8, White);
		ssd1306_SetCursor(63, 23);
		ssd1306_DrawBitmap(vol_mem[2], Icon_8x8, White);
		ssd1306_SetCursor(82, 21);
		ssd1306_WriteString("&", Font_7x10, White);
		ssd1306_SetCursor(99, 23);
		ssd1306_DrawBitmap(vol_mem[3], Icon_8x8, White);
		ssd1306_SetCursor(111, 15);
		ssd1306_DrawBitmap(11, Icon_16x16, itemcolour[3]);  //Return item
	}


	/* Live data layer */
	else if(layer == 5)
	{

	}


	/* Movement layer */
	else if(layer == 6)
	{
		if(page == 1)
		{
			uint8_t itemcolour[] = {White, White, White, White};   //Default items colours
			uint8_t nitems = 4;   //Number of items

			item = constrain(item, 1, nitems, Circular);  //Direction aware item limits

			itemcolour[item-1] = Black;   //Invert the selected item colour

			ustat(Ready);

			ssd1306_SetCursor(0, 0);
			ssd1306_WriteString("Run", Font_7x10, White);
			ssd1306_DrawLineH(0, 127, 10, White);
			ssd1306_SetCursor(0, 12);
			ssd1306_WriteString("Program", Font_7x10, itemcolour[0]);
			ssd1306_SetCursor(65, 12);
			ssd1306_WriteString("Live", Font_7x10, itemcolour[1]);
			ssd1306_SetCursor(0, 21);
			ssd1306_WriteString("Manual", Font_7x10, itemcolour[2]);
			ssd1306_SetCursor(111, 15);
			ssd1306_DrawBitmap(11, Icon_16x16, itemcolour[3]);  //Return item
		}

		if(page == 2)  //Run program page
		{

		}

		if(page == 3)  //Live movement page
		{

		}

		if(page == 4)  //Manual movement page
		{
			uint8_t itemcolour[] = {White, White, White, White, White, White, White};   //Default items colours
			uint8_t nitems = 7;   //Number of items

			item = constrain(item, 1, nitems, Circular);  //Direction aware item limits

			itemcolour[item-1] = Black;   //Invert the selected item colour

			ustat(Running);

			ssd1306_SetCursor(0, 0);
			ssd1306_WriteString("S1: ", Font_7x10, White);
			ssd1306_WriteVar(spos[0], Font_7x10, itemcolour[0]);
			ssd1306_SetCursor(0, 10);
			ssd1306_WriteString("S2: ", Font_7x10, White);
			ssd1306_WriteVar(spos[1], Font_7x10, itemcolour[1]);
			ssd1306_SetCursor(0, 20);
			ssd1306_WriteString("S3: ", Font_7x10, White);
			ssd1306_WriteVar(spos[2], Font_7x10, itemcolour[2]);
			ssd1306_SetCursor(48, 0);
			ssd1306_WriteString("S4: ", Font_7x10, White);
			ssd1306_WriteVar(spos[3], Font_7x10, itemcolour[3]);
			ssd1306_SetCursor(48, 10);
			ssd1306_WriteString("S5: ", Font_7x10, White);
			ssd1306_WriteVar(spos[4], Font_7x10, itemcolour[4]);
			ssd1306_SetCursor(48, 20);
			ssd1306_WriteString("S6: ", Font_7x10, White);
			ssd1306_WriteVar(spos[5], Font_7x10, itemcolour[5]);
			ssd1306_SetCursor(111, 15);
			ssd1306_DrawBitmap(11, Icon_16x16, itemcolour[6]);  //Return item
			if(itemact == 1)  //Draw line under editable value
			{
				if(item <= 3)
				{
					ssd1306_EraseLine(0, ((item-1)*10)+28, Font_7x10, 3, Black);
					ssd1306_SetCursor(0, ((item-1)*10)+28);
					ssd1306_WriteVar(spos[item-1], Font_7x10, White);
					ssd1306_DrawLineH(28, 35, (item*10)-1, White);
				}
				else
				{
					ssd1306_EraseLine(83, ((item-1)*10)+28, Font_7x10, 3, Black);
					ssd1306_SetCursor(83, ((item-1)*10)+28);
					ssd1306_WriteVar(spos[item-1], Font_7x10, White);
					ssd1306_DrawLineH(83, 62, (item*10), White);
				}

			}
		}
	}

	ssd1306_UpdateScreen(hi2c);  //Update screen
}
