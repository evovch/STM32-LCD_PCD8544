# STM32-LCD_PCD8544

Configure your MCU using STM32CubeMX.
SPI baud rate has to be 4 MBits/s. Adjust it using the Prescaler depending on you MCU base frequency.
Add 3 GPIO_Outputs for CE, DC, and RST pins of the LCD. It is convenient to locate them close to each other.

#include "lcd_pcd8544.h"

...

  extern LCD_PCD8544_screen_t gLcdScreen;

  // Assign pins
  gLcdScreen.mPinout.mSpiHandle = &hspi3;
  gLcdScreen.mPinout.mLcdSpiRstPort = LCD_RST_GPIO_Port;
  gLcdScreen.mPinout.mLcdSpiRstPin = LCD_RST_Pin;
  gLcdScreen.mPinout.mLcdSpiCePort = LCD_CE_GPIO_Port;
  gLcdScreen.mPinout.mLcdSpiCePin = LCD_CE_Pin;
  gLcdScreen.mPinout.mLcdSpiDcPort = LCD_DC_GPIO_Port;
  gLcdScreen.mPinout.mLcdSpiDcPin = LCD_DC_Pin;
  gLcdScreen.mPinout.mLcdSpiDinPort = LCD_SPI_DIN_GPIO_Port;
  gLcdScreen.mPinout.mLcdSpiDinPin = LCD_SPI_DIN_Pin;
  gLcdScreen.mPinout.mLcdSpiClkPort = LCD_SPI_CLK_GPIO_Port;
  gLcdScreen.mPinout.mLcdSpiClkPin = LCD_SPI_CLK_Pin;
  gLcdScreen.mPinout.mLcdErrorLedPort = ONBOARD_LED_2_GPIO_Port;
  gLcdScreen.mPinout.mLcdErrorLedPin = ONBOARD_LED_2_Pin;

  // Initialize
  LCD_PCD8544_init(&gLcdScreen);

  LCD_PCD8544_write_line(&gLcdScreen, 0, "Privet!");
  
...
