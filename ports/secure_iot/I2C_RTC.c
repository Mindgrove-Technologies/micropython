/**
 * @file I2C_RTC.c
 * @author Vishwajith.N.S (vishwajith@mindgrovetech.in)
 * @brief Example code to use I2C API's with RTC 
 * @version 1.0
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2024. All rights reserved.
 * 
 */

#include "io.h"         /*Included to access functions for basic IO operations such as printf,etc*/
#include "i2c.h"        /*Included to access I2C peripheral API's*/

void main(){
  printf ("RTC\n\r");
  I2C_Init(0,100000);
  uint8_t data = 0x00,seconds;
  uint8_t i2c_instance = 0x00;
  uint8_t slave_address = 0x68;
  while(1)/*Read time from RTC continuously*/
  {
   	I2C_Transmit(i2c_instance,slave_address,&data,1,START_BIT|STOP_BIT);//Setting reg address to read seconds from RTC
	  I2C_Recieve(i2c_instance,slave_address,&seconds,1,START_BIT|STOP_BIT);//Reading from given register address from RTC
    printf("Seconds: %d\n\r",seconds); 
  }
}
