# STM32-LCD_PCD8544

Configure your MCU using STM32CubeMX.

SPI baud rate has to be 4 MBits/s. Adjust it using the Prescaler depending on you MCU base frequency.

[figure1](./STM32CubeMX_spi_conf.png)

Add 3 GPIO_Outputs for CE, DC, and RST pins of the LCD. It is convenient to locate them close to each other.

[figure2](./Pins.jpg)

Suitable for (tested on) the very popular cheap Chinese boards [STM32F103C8T6 "Blue Pill"](https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill), [STM32FxCxT6 Board v5.02](https://stm32-base.org/boards/STM32F103C8T6-STM32FxCxT6-Board-V5.02), and [STM32F407VET6 - STM32 F4VE V2.0](https://stm32-base.org/boards/STM32F407VET6-STM32-F4VE-V2.0).

Example of usage:
=================

```

#include "lcd_pcd8544.h"

...

  // LCD
  extern LCD_PCD8544_screen_t gLcdScreen;

  // LCD - assign SPI interface and pins
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
  gLcdScreen.mPinout.mLcdErrorLedPort = LCD_ERROR_LED_GPIO_Port;
  gLcdScreen.mPinout.mLcdErrorLedPin = LCD_ERROR_LED_Pin;

  // LCD - initialize
  LCD_PCD8544_init(&gLcdScreen);

  // LCD - welcome screen
  LCD_PCD8544_write_line(&gLcdScreen, 0, "Privet!");
  HAL_Delay(1000);
  
...

```
