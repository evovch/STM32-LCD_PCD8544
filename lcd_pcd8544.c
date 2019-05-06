/*
 * lcd_pcd8544.c
 *
 *  Created on: May 4, 2019
 *      Author: evovch
 */

#include "lcd_pcd8544.h"

#include <string.h> // for memset

#include "font6x8.h"

void LCD_PCD8544_init(LCD_PCD8544_screen_t* scr)
{
	//TODO clear g_LCD_PCD8544_lines

	//TODO check that this is necessary at all
	// Fill gZeroArray with zeros
	memset(gZeroArray, 0, sizeof(gZeroArray));

	// 0 - horizontal addressing;
	// 1 - vertical addressing.
	unsigned char addressing_type = 0;

	// 0 - chip is active;
	// 1 - chip is in power-down mode.
	unsigned char power_down = 0;

	LCD_PCD8544_LL_reset(scr);

	LCD_PCD8544_LL_switch_to_commands_mode(scr);

	LCD_PCD8544_LL_begin_data_transfer(scr);

	// Part 1 ==================================================================
	// 1st byte: instruction = 'Function set' with H=1
	// With this command sent to the LCD controller we
	// indicate that so called extended instruction set will be used
	LCD_PCD8544_LL_set_function_set(scr, power_down, addressing_type, 1);
	// 2nd byte: instruction = 'Bias system'
	LCD_PCD8544_LL_set_bias_system(scr, 3); //TODO study!
	// 3rd byte: instruction = 'Set VOP'
	LCD_PCD8544_LL_set_VOP(scr, 0x40); //TODO study!
	// 4th byte: instruction = 'Temperature control'
	LCD_PCD8544_LL_set_temperature_control(scr, 2); //TODO study!

	// Part 2 ==================================================================
	// 1st byte: instruction = 'Function set' with H=0
	// With this command sent to the LCD controller we
	// indicate that so called basic instruction set will be used
	LCD_PCD8544_LL_set_function_set(scr, power_down, addressing_type, 0);
	// 2nd byte: instruction = 'Display control'
	LCD_PCD8544_LL_set_display_control(scr, 1, 0);
	// 3rd byte: instruction = 'Set Y address of RAM'
	// 4th byte: instruction = 'Set X address of RAM'
	LCD_PCD8544_LL_set_YX_address_of_RAM(scr, 0, 0);

	LCD_PCD8544_LL_end_data_transfer(scr);

	LCD_PCD8544_clear_ram(scr);
}

HAL_StatusTypeDef LCD_PCD8544_write_bytes(LCD_PCD8544_screen_t* scr,
                                          unsigned char vIndex, unsigned char hIndex,
                                          unsigned char* bytes, unsigned short int size)
{
	//TODO check that vIndex, hIndex, and size are in the allowed range

	// 0 - horizontal addressing;
	// 1 - vertical addressing.
	unsigned char addressing_type = 0;

	// 0 - chip is active;
	// 1 - chip is in power-down mode.
	unsigned char power_down = 0;

	// Set the cursor to (vIndex,hIndex)
	LCD_PCD8544_LL_switch_to_commands_mode(scr);
	LCD_PCD8544_LL_begin_data_transfer(scr);
	// 1st byte: instruction = 'Function set' with H=0
	// With this command sent to the LCD controller we
	// indicate that so called basic instruction set will be used
	LCD_PCD8544_LL_set_function_set(scr, power_down, addressing_type, 0);
	// 2nd byte: instruction = 'Set Y address of RAM'
	// 3rd byte: instruction = 'Set X address of RAM'
	LCD_PCD8544_LL_set_YX_address_of_RAM(scr, vIndex, LCD_PCD8544_CHAR_WIDTH*hIndex);
	LCD_PCD8544_LL_end_data_transfer(scr);

	//TODO can we do both parts within the same batch?

	// Per-se fill the RAM with the provided string
	LCD_PCD8544_LL_switch_to_data_mode(scr);
	LCD_PCD8544_LL_begin_data_transfer(scr);
	HAL_StatusTypeDef stat = LCD_PCD8544_LL_send_data(scr, bytes, size);
	LCD_PCD8544_LL_end_data_transfer(scr);

	return stat;
}

HAL_StatusTypeDef LCD_PCD8544_clear_ram(LCD_PCD8544_screen_t* scr)
{
	return LCD_PCD8544_write_bytes(scr, 0, 0, gZeroArray, sizeof(gZeroArray));
}

HAL_StatusTypeDef LCD_PCD8544_write_char(LCD_PCD8544_screen_t* scr, unsigned char vIndex, unsigned char hIndex, const char val)
{
	// Need not initialize as everything is guaranteed to be overwritten by memcpy
	unsigned char data[LCD_PCD8544_CHAR_WIDTH];

	// Translate the input string into the bit array using the font
	unsigned short int idx = (unsigned short int)val;
	memcpy(data, &gFont6x8[LCD_PCD8544_CHAR_WIDTH*idx], LCD_PCD8544_CHAR_WIDTH);

	return LCD_PCD8544_write_bytes(scr, vIndex, hIndex, data, sizeof(data));
}

HAL_StatusTypeDef LCD_PCD8544_fill_block(LCD_PCD8544_screen_t* scr, unsigned char vIndex, unsigned char hIndex, unsigned char color)
{
	// Need not initialize as everything is guaranteed to be overwritten by memcpy
	unsigned char data[LCD_PCD8544_CHAR_WIDTH];

	// Translate the input string into the bit array using the font
	// Item n 256 in the gFont6x8 array is a special block of 6 bytes filled with 0x00
	// Item n 257 in the gFont6x8 array is a special block of 6 bytes filled with 0xff
	unsigned short int idx = 256 + (unsigned short int)color;
	memcpy(data, &gFont6x8[LCD_PCD8544_CHAR_WIDTH*idx], LCD_PCD8544_CHAR_WIDTH);

	return LCD_PCD8544_write_bytes(scr, vIndex, hIndex, data, sizeof(data));
}

HAL_StatusTypeDef LCD_PCD8544_write_str(LCD_PCD8544_screen_t* scr, unsigned char vIndex, unsigned char hIndex, const char* string)
{
	unsigned short int strLen = strlen(string);
	if ((strLen+hIndex) > LCD_PCD8544_LINEWIDTH) { strLen = LCD_PCD8544_LINEWIDTH-hIndex; } // cut to the right edge of the screen

	//TODO check that initialization works; maybe better use memset?
	unsigned char data[LCD_PCD8544_LINEWIDTH*LCD_PCD8544_CHAR_WIDTH]; // = { 0 };
	memset(data, 0, sizeof(data));

	// Translate the input string into the bit array using the font
	unsigned short int nChars=0;
	for ( ; nChars<strLen; nChars++) {
		if (string[nChars] == '\0') {
			//TODO check
			break;
		} else {
			unsigned short int idx = (unsigned short int)(string[nChars]);
			memcpy(&data[LCD_PCD8544_CHAR_WIDTH*nChars], &gFont6x8[LCD_PCD8544_CHAR_WIDTH*idx], LCD_PCD8544_CHAR_WIDTH);
		}
	}

	// not sizeof(data) but strLen; this minimizes the size of the operation
	return LCD_PCD8544_write_bytes(scr, vIndex, hIndex, data, nChars*LCD_PCD8544_CHAR_WIDTH);
}

HAL_StatusTypeDef LCD_PCD8544_write_line(LCD_PCD8544_screen_t* scr, unsigned char vIndex, const char* string)
{
	unsigned short int strLen = strlen(string);
	if (strLen > LCD_PCD8544_LINEWIDTH) { strLen = LCD_PCD8544_LINEWIDTH; } // cut to the right edge of the screen

	//TODO check that initialization works; maybe better use memset?
	unsigned char data[LCD_PCD8544_LINEWIDTH*LCD_PCD8544_CHAR_WIDTH]; // = { 0 };
	memset(data, 0, sizeof(data));

	// Translate the input string into the bit array using the font
	unsigned short int nChars=0;
	for ( ; nChars<strLen; nChars++) {
		if (string[nChars] == '\0') {
			//TODO check
			break;
		} else {
			unsigned short int idx = (unsigned short int)(string[nChars]);
			memcpy(&data[LCD_PCD8544_CHAR_WIDTH*nChars], &gFont6x8[LCD_PCD8544_CHAR_WIDTH*idx], LCD_PCD8544_CHAR_WIDTH);
		}
	}

	// not sizeof(data) but strLen; this minimizes the size of the operation
	return LCD_PCD8544_write_bytes(scr, vIndex, 0, data, sizeof(data));
}

//TODO implement correct return value of type HAL_StatusTypeDef
void LCD_PCD8544_push_line(LCD_PCD8544_screen_t* scr, const char* string)
{
	// Shift
	for (unsigned short int i=LCD_PCD8544_NLINES-1; i>0; i--) {
		strncpy(g_LCD_PCD8544_lines[i], g_LCD_PCD8544_lines[i-1], LCD_PCD8544_LINEWIDTH);
	}
	strncpy(g_LCD_PCD8544_lines[0], string, LCD_PCD8544_LINEWIDTH);

	// Print
	for (unsigned short int i=0; i<NUSEDLINES; i++) {
		LCD_PCD8544_write_line(scr, i, g_LCD_PCD8544_lines[i]);
	}
}

HAL_StatusTypeDef LCD_PCD8544_push_line2(LCD_PCD8544_screen_t* scr, const char* string)
{
	// Shift
	for (unsigned short int i=LCD_PCD8544_NLINES-1; i>0; i--) {
		strncpy(g_LCD_PCD8544_lines[i], g_LCD_PCD8544_lines[i-1], LCD_PCD8544_LINEWIDTH);
	}
	strncpy(g_LCD_PCD8544_lines[0], string, LCD_PCD8544_LINEWIDTH);

	//TODO
	//TODO check that initialization works; maybe better use memset?
	unsigned char data[LCD_PCD8544_NLINES*LCD_PCD8544_LINEWIDTH*LCD_PCD8544_CHAR_WIDTH] = { 0 };

	for (unsigned short int i=0; i<NUSEDLINES; i++) {
		for (unsigned short int j=0; j<LCD_PCD8544_LINEWIDTH; j++) {
			unsigned short int idx = (unsigned short int)(g_LCD_PCD8544_lines[i][j]);
			memcpy(&data[i*LCD_PCD8544_LINEWIDTH*LCD_PCD8544_CHAR_WIDTH+j*LCD_PCD8544_CHAR_WIDTH],
			       &gFont6x8[LCD_PCD8544_CHAR_WIDTH*idx], LCD_PCD8544_CHAR_WIDTH);
		}
	}

	return LCD_PCD8544_write_bytes(scr, 0, 0, data, NUSEDLINES*LCD_PCD8544_LINEWIDTH*LCD_PCD8544_CHAR_WIDTH);
}

//TODO implement correct return value of type HAL_StatusTypeDef
HAL_StatusTypeDef LCD_PCD8544_stack_line(LCD_PCD8544_screen_t* scr, const char* string)
{
	static unsigned int vIndex = 0;
	HAL_StatusTypeDef stat;
	stat = LCD_PCD8544_write_line(scr, vIndex, string);
	if (++vIndex > LCD_PCD8544_NLINES) { vIndex=0; }
	return stat;
}
