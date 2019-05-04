/*
 * lcd_pcd8544_ll.c
 *
 *  Created on: May 4, 2019
 *      Author: evovch
 */

#ifndef __LCD_PCD8544_LL_H
#define __LCD_PCD8544_LL_H

#include "spi.h" // for SPI_HandleTypeDef
#include "gpio.h" // for GPIO_TypeDef

// Setting this timeout to 1 causes problems
// SPI timeout in ms (?) provided to the HAL_SPI_Transmit method
#define SPI_TIMEOUT 2

// Width of a single character in pixels (6)
#define LCD_PCD8544_CHAR_WIDTH 6
// Number of lines on the screen (6)
#define LCD_PCD8544_NLINES 6
// Maximum string length on the screen (14)
#define LCD_PCD8544_LINEWIDTH 14

typedef struct {
	SPI_HandleTypeDef* mSpiHandle;

	GPIO_TypeDef* mLcdSpiRstPort;
	uint16_t mLcdSpiRstPin;
	GPIO_TypeDef* mLcdSpiCePort;
	uint16_t mLcdSpiCePin;
	GPIO_TypeDef* mLcdSpiDcPort;
	uint16_t mLcdSpiDcPin;
	GPIO_TypeDef* mLcdSpiDinPort;
	uint16_t mLcdSpiDinPin;
	GPIO_TypeDef* mLcdSpiClkPort;
	uint16_t mLcdSpiClkPin;
	GPIO_TypeDef* mLcdErrorLedPort;
	uint16_t mLcdErrorLedPin;

} LCD_PCD8544_pinout_t;

typedef struct {
	LCD_PCD8544_pinout_t mPinout;

	//TODO not used yet
	// Actually the copy of the LCD memory stored on the MCU
	unsigned char mRam[LCD_PCD8544_NLINES*LCD_PCD8544_LINEWIDTH*LCD_PCD8544_CHAR_WIDTH];

} LCD_PCD8544_screen_t;

// =============================================================================

// Send an RST pulse - required as the very first step before operating the LCD
inline void LCD_PCD8544_LL_reset(LCD_PCD8544_screen_t* scr) {
	HAL_GPIO_WritePin(scr->mPinout.mLcdSpiRstPort, scr->mPinout.mLcdSpiRstPin, GPIO_PIN_RESET);
	HAL_Delay(1); // 1ms // not really; HAL_Delay does not guarantee exact time
	HAL_GPIO_WritePin(scr->mPinout.mLcdSpiRstPort, scr->mPinout.mLcdSpiRstPin, GPIO_PIN_SET);
}

// Set DC to LOW - go into 'commands' mode
inline void LCD_PCD8544_LL_switch_to_commands_mode(LCD_PCD8544_screen_t* scr) {
	HAL_GPIO_WritePin(scr->mPinout.mLcdSpiDcPort, scr->mPinout.mLcdSpiDcPin, GPIO_PIN_RESET);
}

// Set DC to HIGH - go into 'data' mode
inline void LCD_PCD8544_LL_switch_to_data_mode(LCD_PCD8544_screen_t* scr) {
	HAL_GPIO_WritePin(scr->mPinout.mLcdSpiDcPort, scr->mPinout.mLcdSpiDcPin, GPIO_PIN_SET);
}

// Set CE to LOW - activate data transfer
inline void LCD_PCD8544_LL_begin_data_transfer(LCD_PCD8544_screen_t* scr) {
	HAL_GPIO_WritePin(scr->mPinout.mLcdSpiCePort, scr->mPinout.mLcdSpiCePin, GPIO_PIN_RESET);
}

// Set CE to HIGH - deactivate data transfer
inline void LCD_PCD8544_LL_end_data_transfer(LCD_PCD8544_screen_t* scr) {
	HAL_GPIO_WritePin(scr->mPinout.mLcdSpiCePort, scr->mPinout.mLcdSpiCePin, GPIO_PIN_SET);
}

// =============================================================================

// HAL_SPI_Transmit signature:
// HAL_StatusTypeDef HAL_SPI_Transmit (SPI_HandleTypeDef * hspi, uint8_t * pData, uint16_t Size, uint32_t Timeout)
// Note that 'pData' is basically 'unsigned char*' and 'Size' is 'unsigned short int'

HAL_StatusTypeDef LCD_PCD8544_LL_send_data(LCD_PCD8544_screen_t* scr, unsigned char* bytes, unsigned short int nBytes);

HAL_StatusTypeDef LCD_PCD8544_LL_send_byte(LCD_PCD8544_screen_t* scr, unsigned char byte);

// =============================================================================
// H=0 or H=1
// =============================================================================

// Instruction: 'Function set'
// Description: Power down control; entry mode; extended instruction set control (H)
// Only 1 least significant bit is used of all 3 arguments pd, v, and h
// template: 00100PVH
// pd: 0 - chip is active; 1 - chip is in power-down mode
// v:  0 - horizontal addressing; 1 - vertical addressing
// h:  0 - use basic instruction set; 1 - use extended instruction set
HAL_StatusTypeDef LCD_PCD8544_LL_set_function_set(LCD_PCD8544_screen_t* scr, unsigned char pd, unsigned char v, unsigned char h);

// Instruction: 'Write data'
// Description: Writes data to display RAM
// All 8 bits of the d argument are used
// template: DDDDDDDD
// This is the only command which has to be issued while the D/~C pin is set to HIGH.
// This is not handled here and has to be done before calling this method.
HAL_StatusTypeDef LCD_PCD8544_LL_write_data(LCD_PCD8544_screen_t* scr, unsigned char d);

// =============================================================================
// H=0
// =============================================================================

// Instruction: 'Display control'
// Description: Set display configuration
// Only 1 least significant bit is used both of d and e arguments
// template: 00001D0E
// de: 00 - display blank;
//     10 - normal mode;
//     01 - all display segments on;
//     11 - inverse video mode.
HAL_StatusTypeDef LCD_PCD8544_LL_set_display_control(LCD_PCD8544_screen_t* scr, unsigned char d, unsigned char e);

// Instruction: 'Set Y address of RAM'
// Description: Set Y-address of RAM; 0<=Y<=5
// Only 3 least significant bits of the y argument are used
// template: 01000YYY
HAL_StatusTypeDef LCD_PCD8544_LL_set_Y_address_of_RAM(LCD_PCD8544_screen_t* scr, unsigned char y);

// Instruction: 'Set X address of RAM'
// Description: Set X-address of RAM; 0<=X<=83
// Only 7 least significant bits of the x argument are used
// template: 1XXXXXXX
HAL_StatusTypeDef LCD_PCD8544_LL_set_X_address_of_RAM(LCD_PCD8544_screen_t* scr, unsigned char x);

// Set Y-address of RAM (0<=Y<=5) and then set X-address of RAM (0<=X<=83)
// Send two bytes using a single HAL_SPI_Transmit command
// Only 3 least significant bits of the y argument are used
// Only 7 least significant bits of the x argument are used
// template: 01000YYY
// template: 1XXXXXXX
HAL_StatusTypeDef LCD_PCD8544_LL_set_YX_address_of_RAM(LCD_PCD8544_screen_t* scr, unsigned char y, unsigned char x);

// =============================================================================
// H=1
// =============================================================================

// Instruction: 'Temperature control'
// Description: Set temperature coefficient (TCx)
// Only 2 least significant bits of the tc argument are used
// template: 000001TT
// tc: 00 - V_LCD temperature coefficient 0;
//     01 - V_LCD temperature coefficient 1;
//     10 - V_LCD temperature coefficient 2;
//     11 - V_LCD temperature coefficient 3.
HAL_StatusTypeDef LCD_PCD8544_LL_set_temperature_control(LCD_PCD8544_screen_t* scr, unsigned char tc);

// Instruction: 'Bias system'
// Description: Set bias system (BSx)
// Only 3 least significant bits of the bs argument are used
// template: 00010BBB
HAL_StatusTypeDef LCD_PCD8544_LL_set_bias_system(LCD_PCD8544_screen_t* scr, unsigned char bs);

// Instruction: 'Set VOP'
// Description: Write VOP to register
// Only 7 least significant bits of the vop argument are used
// template: 1VVVVVVV
HAL_StatusTypeDef LCD_PCD8544_LL_set_VOP(LCD_PCD8544_screen_t* scr, unsigned char vop);

// =============================================================================

#endif /* __LCD_PCD8544_LL_H */
