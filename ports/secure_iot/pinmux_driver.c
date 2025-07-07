/****************************************************************************
 * Project                               : shakti devt board
 * Name of the file                      : pinmux_driver.c
 * Brief Description of file             : src  file for uart
 * Name of Author                        : Kotteeswaran and Niketan Shahapur
 * Email ID                              : <kottee.1@gmail.com>  <niketanshahpur@gmail.com>

 Copyright (C) 2019 IIT Madras. All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *****************************************************************************/
/**
  @file pinmux_driver.c
  @brief Contains the driver routines for PINMUX.
  @details The pinmux driver file contains functions to enable the required peripheral.
 */

//PINMUX 0-7   :: GPIO 0-7 and PWM 0-7
//PINMUX 8-13  :: GPIO 17-22 and PWM 8-13
//PINMUX 14    :: SPI2_MOSI and GPIO 32
//PINMUX 15    :: SPI2_MISO and GPIO 33
//PINMUX 16    :: SPI2_NCS and GPIO 34
//PINMUX 17    :: SPI3_MOSI and GPIO 35
//PINMUX 18    :: SPI3_MISO and GPIO 36
//PINMUX 19    :: SPI3_NCS and GPIO 37
//PINMUX 20    :: GPIO 8 and UART 3 TX
//PINMUX 21    :: GPIO 9 and UART 3 RX
//PINMUX 22    :: GPIO 11 and UART 4 TX
//PINMUX 23    :: GPIO 15 and UART 4 RX
//PINMUX 24    :: GPIO 38 and GPTIMER0
//PINMUX 25    :: GPIO 39 and GPTIMER1
//PINMUX 26    :: GPIO 40 and GPTIMER2
//PINMUX 27    :: GPIO 41 and GPTIMER3
//PINMUX 28    :: GPIO 42 and JTAG_TDI
//PINMUX 29    :: GPIO 43 and JTAG_TMS
//PINMUX 30    :: GPIO 44 and JTAG_TDO


#define PIN_MUX_DRIVER 1
#include "pinmux.h"

unsigned int* pinmux_config_reg = (unsigned int* ) PINMUX0_BASE;


/**
 * @fn The function int PINMUX_EnableGPIO(int num) sets the pinmux configuration for a GPIO pin if the pin number is
 * in the range 0-7 or 17-22 or 32-40, otherwise it logs an error message.
 * 
 * @param num The `num` parameter in the `PINMUX_EnableGPIO` function represents the GPIO pin number
 * that you want to enable for pin multiplexing. The function checks if the provided GPIO pin number is
 * in the range 0-7 or 17-22 or 32-40.
 * 
 * @return The function `PINMUX_EnableGPIO` will return either `SUCCESS` if the input `num` is in the range 0-7 or 17-22 or 32-40
 * and the pinmux configuration is set to 0, or it will return `ENODEV` if the input `num` is
 * not within the range.
 */
uint32_t PINMUX_EnableGPIO(uint8_t num)
{
  if((num >= 0 && num < 8) || (num >= 17 && num < 23) || (num >= 32 && num < 45))
  {
    if (num <= 7) 
    *(pinmux_config_reg + num) = 0;
    else if (num == 8)
      *(pinmux_config_reg + 20) = 0;
    else if (num == 9)
      *(pinmux_config_reg + 21) = 0;
    else if (num == 11)
      *(pinmux_config_reg + 22) = 0;
    else if (num == 15)
      *(pinmux_config_reg + 23) = 0;
    else if((num >= 17) && (num < 23))
      *(pinmux_config_reg + num - 9) = 0;
    else if((num >= 32) && (num <= 37))
      *(pinmux_config_reg + num - 18) = 1;
    else if((num >= 38) && (num < 45))
      *(pinmux_config_reg + num - 14) = 0;
    return SUCCESS;
  }
  else
  {
    log_error("Pinmuxed GPIOs are 0-9, 11, 15, 17-22 or 32-44");
    return ENODEV;
  }
}


/**
 * @fn The function int PINMUX_EnablePWM(int num) enables PWM functionality for a specified pin if the pin number is
 * less than 14, otherwise it logs an error message.
 * 
 * @param num The `num` parameter in the `PINMUX_EnablePWM` function represents the PWM channel number
 * that you want to enable. The function checks if the provided channel number is less than 14. If it
 * is, it sets the corresponding pinmux configuration register to 1 to enable the PWM channel and
 * 
 * @return The function will return either SUCCESS if the pinmuxing was successful and the number is
 * less than 14, or it will return ENODEV if the number is 14 or greater, indicating that the maximum
 * pinmuxed PWMs are 14.
 */
uint32_t PINMUX_EnablePWM(uint8_t num)
{
  if (num < 14)
  {
    *(pinmux_config_reg + num) = 1;
    return SUCCESS;
  }
  else{
    log_error("Max pinmuxed PWMs are 14");
    return ENODEV;
  }
}


/**
 * @fn The function int ENABLE_Pwm(int count, int enable_pwm[]) enables PWM functionality for a specified number of pins, with a maximum
 * limit of 14 pins.
 * 
 * @param count The `count` parameter in the `ENABLE_Pwm` function represents the number of PWM pins
 * that you want to enable. The function checks if the count is less than 14, and if so, it proceeds to
 * enable the specified PWM pins. If the count is 14 or more, it
 * @param enable_pwm The `enable_pwm` parameter is an array of integers that contains the indexes of
 * the PWM pins that need to be enabled. The function `ENABLE_Pwm` takes the total count of PWM pins to
 * be enabled and the array `enable_pwm` as input parameters. It then enables the specified PWM pins
 * 
 * @return The function `ENABLE_Pwm` will return either `SUCCESS` if the count is less than 14 and the
 * PWM pins are successfully enabled, or `ENODEV` if the count is 14 or greater indicating that the
 * maximum pinmuxed PWMs are already reached.
 */
uint32_t ENABLE_Pwm(uint8_t count, uint32_t enable_pwm[])
{
  if (count < 14)
  {
    for(int i=0; i< count; i++){
      *(pinmux_config_reg + enable_pwm[i]) = 1;
    }
    return SUCCESS;
  }
  else{
    log_error("Max pinmuxed PWMs are 14");
    return ENODEV;
  }
}


/**
 * @fn The function int pinmux_enable_all_pwm() enables PWM functionality on multiple pins.
 * 
 * @return SUCCESS
 */
uint32_t PINMUX_EnableAllPWM()
{
  for(int i=0; i<14; i++)
    PINMUX_EnablePWM(i);

  return SUCCESS;
}


/**
 * @fn The function int PINMUX_EnableAllGPIO() enables GPIO pins 0 to 7 , 17 to 22 and 32 to 44.
 * 
 * @return The function `PINMUX_EnableAllGPIO()` is returning the value `SUCCESS`.
 */
uint32_t PINMUX_EnableAllGPIO()
{
  for(int i=0; i<8; i++)
    PINMUX_EnableGPIO(i);
  
  for(int i=17; i<23; i++)
    PINMUX_EnableGPIO(i);
  
  for(int i=32; i<45; i++)
    PINMUX_EnableGPIO(i);

  return SUCCESS;
}

/**
 * @fn The function PINMUX_EnableSPI() enables one of the SPI peripheral(SPI2 or SPI3).
 * 
 * @param num The `num` parameter in the `PINMUX_EnableSPI` function represents the SPI number
 * that you want to enable for pin multiplexing. The function checks if the provided SPI number is
 * 2 or 3. If it is,the pinmux configuration register is set to 0.
 * 
 * @return Returns the value `SUCCESS` if SPI number is 2 or 3.Returns `ENODEV` if the SPI number is invalid. 
 */
uint32_t PINMUX_EnableSPI(uint8_t num)
{
  if (num == 2 || num == 3)
  {
    if(num == 2)
    {
      for(int i=0; i< 4; i++)
        *(pinmux_config_reg + 14 + i) = 0;
    }
    if(num == 3)
    {
       for(int i=0; i< 4; i++)
        *(pinmux_config_reg + 18 + i) = 0;
    }
    return SUCCESS;
  }
  else{
    log_error("The only SPI options available for pinmux are 2 and 3.");
    return ENODEV;
  }
}

/**
 * @fn The function PINMUX_EnableUART() enables one of the UART peripheral(UART3 or UART4).
 * 
 * @param num The `num` parameter in the `PINMUX_EnableUART` function represents the UART number
 * that you want to enable for pin multiplexing. The function checks if the provided UART number is
 * 3 or 4. If it is, the pinmux configuration register is set to 0.
 * 
 * @return Returns the value `SUCCESS` if UART number is 3 or 4.Returns `ENODEV` if the UART number is invalid. 
 */
uint32_t PINMUX_EnableUART(uint8_t num)
{
  if (num == 3 || num == 4)
  {
    if(num == 3)
    {
      for (int i=0; i<2; i++)
        *(pinmux_config_reg + 20 + i) = 1;
    }
    if(num == 4)
    {    
      for (int i=0; i<2; i++)
        *(pinmux_config_reg + 22 + i) = 1;
    }
    return SUCCESS;
  }
  else{
    log_error("The only UART options available for pinmux are 3 and 4.");
    return ENODEV;
  }
}

/**
 * @fn The function int PINMUX_EnableJTAG() enables JTAG_TRST.
 * 
 * @return The function `PINMUX_EnableJTAG()` is returning the value `SUCCESS`.
 */
uint32_t PINMUX_EnableJTAG()
{
  for (int i=0; i<4; i++){
     *(pinmux_config_reg + 28 + i) = 1;
     }
  return SUCCESS;
}

uint32_t PINMUX_EnableGPTimer(uint8_t num)
{
  if(num<5)
  {
    switch(num)
    {
      case 0:
     *(pinmux_config_reg + 24) = 1;
     case 1:
     *(pinmux_config_reg + 25) = 1;
     case 2:
     *(pinmux_config_reg + 26) = 1;
     case 3:
     *(pinmux_config_reg + 27) = 1;
    }
    return SUCCESS;
  }
  else
  {
    log_error("The only GPTIMER pins available are 1-4.");
    return ENODEV;
  }
}
