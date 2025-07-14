#include "i2c.h"
#include <stdint.h>
void checking()
{
    printf("Starting Testing !!!");
  I2C_Init(0,100000);
  uint8_t data = 0x00,seconds;
  uint8_t i2c_instance = 0x00;
  uint8_t slave_address = 0x68;
  while(1)
  {
    I2C_Transmit(i2c_instance,slave_address,&data,1,START_BIT|STOP_BIT);//Setting reg address to read seconds from RTC
    I2C_Recieve(i2c_instance,slave_address,&seconds,1,START_BIT|STOP_BIT);//Reading from given register address from RTC
    printf("Seconds: %d\n\r",seconds); 
  }
}