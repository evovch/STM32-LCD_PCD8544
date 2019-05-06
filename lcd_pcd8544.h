/*
 * lcd_pcd8544.h
 *
 *  Created on: May 4, 2019
 *      Author: evovch
 */

#ifndef __LCD_PCD8544_H
#define __LCD_PCD8544_H

#include "lcd_pcd8544_ll.h"

// Number of lines used for push_line methods
// Maximum 6 lines
#define NUSEDLINES 6

// =============================================================================
// Global variables
// =============================================================================

LCD_PCD8544_screen_t gLcdScreen;

// Buffer of LCD_PCD8544_NLINES lines which are displayed on the screen
// Each line is limited to LCD_PCD8544_LINEWIDTH symbols
char g_LCD_PCD8544_lines[LCD_PCD8544_NLINES][LCD_PCD8544_LINEWIDTH];

// Just a zero array used to clear the screen
// Filled with zeros during once initialization
static unsigned char gZeroArray[LCD_PCD8544_NLINES*LCD_PCD8544_LINEWIDTH*LCD_PCD8544_CHAR_WIDTH];

// =============================================================================

// Initialization routine:
//TODO write comments
void LCD_PCD8544_init(LCD_PCD8544_screen_t* scr);

HAL_StatusTypeDef LCD_PCD8544_write_bytes(LCD_PCD8544_screen_t* scr,
                                          unsigned char vIndex, unsigned char hIndex,
                                          unsigned char* bytes, unsigned short int size);

// Set the cursor to (0,0) and send LCD_PCD8544_NLINES*LCD_PCD8544_LINEWIDTH*LCD_PCD8544_CHAR_WIDTH zero bytes of data which
// are written into the LCD controller RAM
HAL_StatusTypeDef LCD_PCD8544_clear_ram(LCD_PCD8544_screen_t* scr);

HAL_StatusTypeDef LCD_PCD8544_write_char(LCD_PCD8544_screen_t* scr, unsigned char vIndex, unsigned char hIndex, const char val);

// color: 0 - clear (white), 1 - fill (black)
HAL_StatusTypeDef LCD_PCD8544_fill_block(LCD_PCD8544_screen_t* scr, unsigned char vIndex, unsigned char hIndex, unsigned char color);

HAL_StatusTypeDef LCD_PCD8544_write_str(LCD_PCD8544_screen_t* scr, unsigned char vIndex, unsigned char hIndex, const char* string);

HAL_StatusTypeDef LCD_PCD8544_write_line(LCD_PCD8544_screen_t* scr, unsigned char vIndex, const char* string);

// This implementation loops throught the lines and executes each line separately.
void LCD_PCD8544_push_line(LCD_PCD8544_screen_t* scr, const char* string);

// This implementation is supposed to be faster. It translates all the strings into
// one data array and pushed it as a single block.
HAL_StatusTypeDef LCD_PCD8544_push_line2(LCD_PCD8544_screen_t* scr, const char* string);

HAL_StatusTypeDef LCD_PCD8544_stack_line(LCD_PCD8544_screen_t* scr, const char* string);

// =============================================================================

#endif /* __LCD_PCD8544_H */
