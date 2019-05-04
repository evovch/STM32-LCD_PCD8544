/*
 * lcd_pcd8544_ll.c
 *
 *  Created on: May 4, 2019
 *      Author: evovch
 */

#include "lcd_pcd8544_ll.h"

// =============================================================================

HAL_StatusTypeDef LCD_PCD8544_LL_send_data(LCD_PCD8544_screen_t* scr, unsigned char* bytes, unsigned short int nBytes)
{
	HAL_StatusTypeDef stat = HAL_SPI_Transmit(scr->mPinout.mSpiHandle, bytes, nBytes, SPI_TIMEOUT);
	if (stat != HAL_OK) {
		HAL_GPIO_WritePin(scr->mPinout.mLcdErrorLedPort, scr->mPinout.mLcdErrorLedPin, GPIO_PIN_RESET);
	}
	return stat;
}

HAL_StatusTypeDef LCD_PCD8544_LL_send_byte(LCD_PCD8544_screen_t* scr, unsigned char byte)
{
	unsigned short int nBytes = 1;
	return LCD_PCD8544_LL_send_data(scr, &byte, nBytes);
}

// =============================================================================
// H=0 or H=1
// =============================================================================

HAL_StatusTypeDef LCD_PCD8544_LL_set_function_set(LCD_PCD8544_screen_t* scr, unsigned char pd, unsigned char v, unsigned char h)
{
	//TODO check that pd argument is within the limits
	//TODO check that v argument is within the limits
	//TODO check that h argument is within the limits
	// Also, probably, implement proper error handling
	unsigned char command = (1 << 5) | ((pd & 0x01) << 2) | ((v & 0x01) << 1) | (h & 0x1);
	return LCD_PCD8544_LL_send_byte(scr, command);
}

HAL_StatusTypeDef LCD_PCD8544_LL_write_data(LCD_PCD8544_screen_t* scr, unsigned char d)
{
	//TODO check that d argument is within the limits
	// Also, probably, implement proper error handling
	return LCD_PCD8544_LL_send_byte(scr, d);
}

// =============================================================================
// H=0
// =============================================================================

HAL_StatusTypeDef LCD_PCD8544_LL_set_display_control(LCD_PCD8544_screen_t* scr, unsigned char d, unsigned char e)
{
	//TODO check that d argument is within the limits
	//TODO check that e argument is within the limits
	// Also, probably, implement proper error handling
	unsigned char command = (1 << 3) | ((d & 0x01) << 2) | (e & 0x01);
	return LCD_PCD8544_LL_send_byte(scr, command);
}

HAL_StatusTypeDef LCD_PCD8544_LL_set_Y_address_of_RAM(LCD_PCD8544_screen_t* scr, unsigned char y)
{
	//TODO check that y argument is within the limits
	// Also, probably, implement proper error handling
	unsigned char command = (1 << 6) | (y & 0x07);
	return LCD_PCD8544_LL_send_byte(scr, command);
}

HAL_StatusTypeDef LCD_PCD8544_LL_set_X_address_of_RAM(LCD_PCD8544_screen_t* scr, unsigned char x)
{
	//TODO check that x argument is within the limits
	// Also, probably, implement proper error handling
	unsigned char command = (1 << 7) | (x & 0x7f);
	return LCD_PCD8544_LL_send_byte(scr, command);
}

HAL_StatusTypeDef LCD_PCD8544_LL_set_YX_address_of_RAM(LCD_PCD8544_screen_t* scr, unsigned char y, unsigned char x)
{
	//TODO check that y argument is within the limits
	//TODO check that x argument is within the limits
	// Also, probably, implement proper error handling
	unsigned char commands[2];
	commands[0] = (1 << 6) | (y & 0x07);
	commands[1] = (1 << 7) | (x & 0x7f);
	return LCD_PCD8544_LL_send_data(scr, commands, 2);
}

// =============================================================================
// H=1
// =============================================================================

HAL_StatusTypeDef LCD_PCD8544_LL_set_temperature_control(LCD_PCD8544_screen_t* scr, unsigned char tc)
{
	//TODO check that tc argument is within the limits
	// Also, probably, implement proper error handling
	unsigned char command = (1 << 2) | (tc & 0x03);
	return LCD_PCD8544_LL_send_byte(scr, command);
}

HAL_StatusTypeDef LCD_PCD8544_LL_set_bias_system(LCD_PCD8544_screen_t* scr, unsigned char bs)
{
	//TODO check that bs argument is within the limits
	// Also, probably, implement proper error handling
	unsigned char command = (1 << 4) | (bs & 0x07);
	return LCD_PCD8544_LL_send_byte(scr, command);
}

HAL_StatusTypeDef LCD_PCD8544_LL_set_VOP(LCD_PCD8544_screen_t* scr, unsigned char vop)
{
	//TODO check that vop argument is within the limits
	// Also, probably, implement proper error handling
	unsigned char command = (1 << 7) | (vop & 0x7f);
	return LCD_PCD8544_LL_send_byte(scr, command);
}

// =============================================================================
