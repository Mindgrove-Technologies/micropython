/**
 * Project                           : Secure IoT SoC
 * Name of the file                  : qspi_driver.c
 * Brief Description of file         : This is a Baremetal QSPI Driver source file for Mindgrove Silicon's I2C Peripheral.
 * Name of Author                    : Vishwajith.N.S 
 * Email ID                          : vishwajith@mindgrovetech.in
 * 
 * 
 * 
 * @file qspi_driver.c
 * @author Vishwajith .N.S (vishwajith@mindgrovetech.in)
 * @brief This is a Baremetal QSPI Driver source file for Mindgrove Silicon's QSPI Peripheral.
 * @version 1.0
 * @date 2023-07-08
 * 
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2023. All rights reserved.
 * 
 */
#include<stdint.h>
#include "qspi.h"
#include "gptimer.h"

#define QUADSPI_Reg(x) ((QUADSPI_Type*)(QUADSPI0_BASE + ((x) * QSPI_OFFSET)))

/**
 * @fn QSPI_Transaction(uint32_t instance_number,qspi_msg *msg)
 * 
 * @brief Used to perfom QSPI transaction with necessary setting.
 * @param qspinum The parameter \a qspinum is an unsigned integer that represents the QSPI instance number.
 * @param msg The parameter \a msg is a structure which has all settings to initiate a transaction.
 * 
 * @return SUCCESS if operation is successful,ENODEV if invalid instance number and ELENEXCEED is length of read and write parameters exceeded.
 */

uint32_t QSPI_Transaction(uint32_t instance_number,qspi_msg *msg){
    if(instance_number>1)
      return ENODEV;
    QUADSPI_Reg(instance_number)->CR=(CR_PRESCALER(msg->PRESCALER) |CR_PMM(msg->PMM) | CR_APMS(msg->APMS) | CR_TOIE(msg->TOIE) |\
      CR_SMIE(msg->SMIE) | CR_FTIE(msg->FTIE) | CR_TCIE(msg->TCIE) | CR_TEIE(msg->TEIE) | CR_TCEN(msg->TCEN) | CR_EN(1));
    QUADSPI_Reg(instance_number)->DCR = (DCR_FSIZE(msg->FMEM_SIZE) | DCR_CKMODE(msg->CLK_MODE) | DCR_CSHT(msg->csht));    
    uint32_t temp;
    do{
        temp = QUADSPI_Reg(instance_number)->SR;
        temp &= SR_BUSY;
    }while(temp==SR_BUSY);//check for busy status
    QUADSPI_Reg(instance_number)->FCR =(FCR_CTOF|FCR_CSMF|FCR_CTCF|FCR_CTEF);//clear flags
if((msg->functional_mode == CCR_FMODE_APM)){
    QUADSPI_Reg(instance_number)->PSMKR =msg->status_mask;
    QUADSPI_Reg(instance_number)->PSMAR =msg->status_match;
}
    QUADSPI_Reg(instance_number)->DLR = msg->length-1;
    temp = (CCR_INSTRUCTION(msg->instruction) | CCR_IMODE(msg->instruction_mode) | CCR_ADMODE(msg->address_mode) | CCR_ADSIZE(msg->address_size) |\
    CCR_ABMODE(msg->alternate_byte_mode) | CCR_ABSIZE(msg->alternate_byte_size) | CCR_DCYC(msg->dummy_cycles) | CCR_DUMMY_CONFIRMATION(msg->dummy_mode) |\
    CCR_DMODE(msg->data_mode) | CCR_FMODE(msg->functional_mode) | CCR_SIOO(msg->sioo) | CCR_DUMMY_BIT(msg->dummy_bit) | CCR_MM_MODE(msg->mm_mode));
    QUADSPI_Reg(instance_number)->CCR = temp;
    QUADSPI_Reg(instance_number)->ABR = msg->alternate_byte;
    QUADSPI_Reg(instance_number)->AR = msg->address;

    uint8_t i = 0;
    uint32_t status_reg;
      // QUADSPI_Reg(instance_number)->CR&= ~(CR_FTHRES(15));
    if(msg->functional_mode == CCR_FMODE_INDIRECT_WRITE && msg->length!= 0)
    {
      while(1){
        status_reg = QUADSPI_Reg(instance_number)->SR;
        status_reg &= SR_FTF;
        if(status_reg){
          QUADSPI_Reg(instance_number)->DR.data_8 = msg->data_buffer[i];
          i++;
            if(i == msg->length)
              break;
        }
      }  
    }
    else if(msg->functional_mode == CCR_FMODE_INDIRECT_READ)
    {
      while(1){
        status_reg = QUADSPI_Reg(instance_number)->SR;
        status_reg &= SR_FTF;
        if(status_reg){
          msg->data_buffer[i] = QUADSPI_Reg(instance_number)->DR.data_8;
          i++;
            if(i == msg->length)
              break;
        }
      }
    }
    if((msg->functional_mode == CCR_FMODE_MMM)||(msg->functional_mode == CCR_FMODE_APM)){
      // QUADSPI_Reg(instance_number)->CR|=(CR_FTHRES(msg->fthresh));
      return SUCCESS;
    }
    if((msg->functional_mode == CCR_FMODE_INDIRECT_READ) ||(msg->functional_mode == CCR_FMODE_INDIRECT_WRITE) )
    {
      do{
      temp = QUADSPI_Reg(instance_number)->SR;
      temp &= SR_TCF;
    }while(temp == 0);

    QUADSPI_Reg(instance_number)->CR&= ~CR_EN(1);
    } 
    return SUCCESS;
}
uint8_t QSPI_SMF_Flag_Check(uint8_t instance_number,uint8_t wait_till_match){
  uint32_t status_reg;
  do{
    status_reg = QUADSPI_Reg(instance_number)->SR;
    if(status_reg & 0x08) 
    {
      return 1;
    } 
  }while(wait_till_match);
  return 0;
}

