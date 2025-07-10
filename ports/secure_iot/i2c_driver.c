/**
 * Project                           : Secure IoT SoC
 * Name of the file                  : i2c_driver.c
 * Brief Description of file         : This is a Baremetal I2C Driver source file for Mindgrove Silicon's I2C Peripheral.
 * Name of Author                    : Vishwajith.N.S 
 * Email ID                          : vishwajith@mindgrovetech.in
 * 
 * 
 * 
 * @file i2c_driver.c
 * @author Vishwajith .N.S (vishwajith@mindgrovetech.in)
 * @brief This is a Baremetal I2C Driver source file for Mindgrove Silicon's I2C Peripheral.
 * @version 1.0
 * @date 2024-10-17
 * 
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2024. All rights reserved.
 * 
 */

#include "i2c.h"
#include"gptimer.h"
/*Necessary macros for I2C driver*/
#define I2C_PIN 0x80
#define I2C_ESO 0x40
#define I2C_ES1 0x20
#define I2C_ES2 0x10
#define I2C_ENI 0x08
#define I2C_STA 0x04
#define I2C_STO 0x02
#define I2C_ACK 0x01
#define I2C_INI 0x40   
#define I2C_STS 0x20
#define I2C_BER 0x10
#define I2C_AD0 0x08
#define I2C_LRB 0x08
#define I2C_AAS 0x04
#define I2C_LAB 0x02
#define I2C_BB  0x01
#define I2C_START         (I2C_PIN | I2C_ESO | I2C_STA | I2C_ACK)
#define I2C_STOP          (I2C_PIN | I2C_ESO | I2C_STO | I2C_ACK)
#define I2C_REPSTART      (                 I2C_ESO | I2C_STA | I2C_ACK)
#define I2C_IDLE          (I2C_ESO                  | I2C_ACK)
#define I2C_NACK          (I2C_ESO)
#define I2C_DISABLE       (I2C_PIN|I2C_ACK)
#define I2C_OFFSET 0x100


/* Struct to access I2C registers as 32 bit registers */
#define I2C_REG(x) ((I2C_Type*)(I2C0_BASE + (x)*I2C_OFFSET))
//changes depening on the i2c number

uint32_t I2C_Init(uint8_t instance_number,uint32_t clock_frequency) 
{ 
  if(instance_number>1 || instance_number<0)
	  return ENODEV;
    //so 2 i2cs
  I2C_REG(instance_number)->CTRL = I2C_PIN;//serial interface off -> equivalent to 0x80 in S1'
  //Disables the serial interface by setting the CTRL register to I2C_PIN.
  uint32_t scl_div = CLOCK_FREQUENCY_BASE/(2*2*clock_frequency); //the used clock frequency
  I2C_REG(instance_number)->S2 = 1;//setting up clock ->equivalent to setting up register S2
  I2C_REG(instance_number)->SCL = scl_div;//assigning the clock value
  I2C_REG(instance_number)->CTRL = I2C_IDLE;//Enable Serial Interface thats it
  //Enables the serial interface by setting the CTRL register to I2C_IDLE.
  return SUCCESS;
}

uint32_t I2C_Transmit(uint32_t instance_number,uint8_t slave_address,uint8_t *data,uint8_t length,uint8_t mode)
{
  if(instance_number>1 || instance_number<0)
    return ENODEV; //error no device
  if(mode & START_BIT)
  while (!(I2C_REG(instance_number)->STATUS_b.STATUS_BB));//wait till bus is free
  // if a START condition is requested, waits for the bus to be free (STATUS_BB).
  I2C_REG(instance_number)->S0=(slave_address<<1);//write data in data register
  //data transmission register aam
  //Loads the slave address (with write bit) into the data register.
  if(mode & START_BIT)
  I2C_REG(instance_number)->CTRL = I2C_START;// as soon as start is initiated after start bit is given slave address along with r/~w is transmitted
  //mode :flags indicating whether to send START/STOP conditions
  while (((I2C_REG(instance_number)->CTRL_b.CTRL_PIN)!=0x00));// wait till the eight bits completely get transmitted
  //sw indication for all bits getting sent.
  
  if(!(I2C_REG(instance_number)->STATUS_b.STATUS_AD0_LRB))//check whether ack is receieved from slave
  {
    //if 0 
    log_debug("\nAck received for slave address");
        
  }
  else
  {
    log_error("\nAck not received for slave address: %x",slave_address);
    I2C_REG(instance_number)->CTRL = I2C_STOP;//verum stop bit mattum will be set , defined as macro
    return ENOACKDEV;
    //error no ack
  }

  for (uint32_t i=0;i<length;i++)
  {
    I2C_REG(instance_number)->S0=data[i];// write the data in data register
    //sends data bit by bit
    //data->pointer to the data
    while (((I2C_REG(instance_number)->CTRL_b.CTRL_PIN)!=0x00));// wait till the eight bits completely get transmitted
    if(!(I2C_REG(instance_number)->STATUS_b.STATUS_AD0_LRB))//check whether ack is receieved from slave
    {
      log_debug("\nAck received after writing data");
    }
    else
    {
      log_error("\nAck not received after writing data");
      I2C_REG(instance_number)->CTRL = I2C_STOP;
      return ENOACK;
    }
  }
  if(mode & STOP_BIT)
    I2C_REG(instance_number)->CTRL = I2C_STOP;
    //in case of byte write
  else{
    I2C_REG(instance_number)->CTRL = I2C_REPSTART;
    while (((I2C_REG(instance_number)->CTRL_b.CTRL_PIN)!=0x00));// wait till the eight bits completely get transmitted
  }
  return SUCCESS;
}

uint32_t I2C_Recieve(uint32_t instance_number,uint8_t slave_address,uint8_t *data,uint8_t length,uint8_t mode)
{
  if(instance_number>1 || instance_number<0)
    return ENODEV;//right uhh 1,0
  if(mode & START_BIT)
    while (!(I2C_REG(instance_number)->STATUS_b.STATUS_BB));//wait till bus is free
  I2C_REG(instance_number)->S0=(slave_address<<1)|1;//write data in data register
  //read from the slave to the master so read bit set 1
  if(mode & START_BIT)
  I2C_REG(instance_number)->CTRL = I2C_START;// as soon as start is initiated after start bit is given slave address along with r/~w is transmitted
  while (((I2C_REG(instance_number)->CTRL_b.CTRL_PIN)!=0x00));// wait till the eight bits completely get transmitted
  if(!(I2C_REG(instance_number)->STATUS_b.STATUS_AD0_LRB))//check whether ack is receieved from slave
  {
    log_debug("\nAck received for slave address");
  }
  else
  {
    log_error("\nAck not received for slave address: %x",slave_address);
    I2C_REG(instance_number)->CTRL = I2C_STOP;
    return ENOACKDEV;
  }
  uint8_t dummy_read;
  /*
  Performs a dummy read (required for some I2C peripherals).
  For each byte to be received:
  Waits for transmission to complete.
  If it's the last byte, issues a NACK to signal end of read.
  Reads received byte into the buffer.

*/
    for(int i=0;i<=length;i++)
    //length + 1
    { 
      if(i==0)
      {
        if(length == 1)
        I2C_REG(instance_number)->CTRL = I2C_NACK;
        dummy_read = I2C_REG(instance_number)->S0;
        //Reads the data register (S0) into a dummy variable to clear the hardware's buffer.
        continue;
      }
      while (((I2C_REG(instance_number)->CTRL_b.CTRL_PIN)!=0x00));// wait till the eight bits completely get transmitted
      if(i == length-1)
      I2C_REG(instance_number)->CTRL = I2C_NACK;//NACK after this byte, signaling to the slave that the master will stop reading.
      data[i-1] = I2C_REG(instance_number)->S0;//get bytes
    }
    if(mode & STOP_BIT)
    I2C_REG(instance_number)->CTRL = I2C_STOP;
    else{
    I2C_REG(instance_number)->CTRL = I2C_REPSTART;
    while (((I2C_REG(instance_number)->CTRL_b.CTRL_PIN)!=0x00));// wait till the eight bits completely get transmitted
    }
    return SUCCESS;
}