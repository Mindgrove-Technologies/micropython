#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
//for supressing warnings on integer to pointer and pointer to integer conversion in code
//void (*fptr)(void) = (void *)0x10008000; // Warning suppressed
//Maybe issues in assigning fixed memeory addresses to memory devices
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
//Warnings about calling a function without a prototype.
//why here though?
#pragma GCC diagnostic ignored "-Wreturn-mismatch"
//for supressing warnings when a function returns a type different from what was declared
/**
 * Project                           : Secure IoT SoC
 * Name of the file                  : spi_driver.c
 * Brief Description of file         : This is a Baremetal SPI Driver file for Mindgrove Silicon's I2C Peripheral.
 * Name of Author                    : Santhosh Pavan S, Ajay Kumar M, Kapil Shyamm M, Kishore J
 * Email ID                          : kishore@mindgrovetech.in, kapilshayamm@mindgrovetech.in, santhosh@mindgrovetech.in, ajaykumarmanoharr@mindgrovetech.in
 * 
 * 
 * 
 * @file spi_driver.c
 * @author Santhosh Pavan S, Kapil shayamm M, Ajay kumar M, Kishore J
 * @brief This is a Baremetal SPI Driver file for Mindgrove Silicon's I2C Peripheral.
 * @version 0.2
 * @date 2024-09-14
 * 
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2023. All rights reserved.
 * 
 */

#include "spi.h"

SPI_Type *sspi_instance[SSPI_MAX_COUNT];

//SSPIx Clock Control Register
#define SPI_CLK_POLARITY(x)   (x<<0)
#define SPI_CLK_PHASE(x)      (x<<1)
#define SPI_PRESCALE(x)       (x<<2)
#define SPI_SS2TX_DELAY(x)    (x<<10)
#define SPI_TX2SS_DELAY(x)    (x<<18)

//SSPIx Communication Control Register
#define SPI_MODE(x)          (x<<0)  
#define SPI_ENABLE(x)          (x<<1)
#define SPI_LSB_FIRST(x)       (x<<2)
#define SPI_FLUSH              (1<<3)
#define SPI_COMM_MODE(x)       (x<<4)
#define SPI_TOTAL_BITS_TX(x)   (x<<6)
#define SPI_TOTAL_BITS_RX(x)   (x<<14)
#define SPI_OUT_EN_SCLK        (1<<22)
#define SPI_OUT_EN_NCS         (1<<23)
#define SPI_OUT_EN_MISO        (1<<24)
#define SPI_OUT_EN_MOSI        (1<<25)
#define SPI_DMA_TX_SIZE(x)     (x<<26)
#define SPI_DMA_RX_SIZE(x)     (x<<29)

//SSPIx Communication Status Register
#define SPI_BUSY             (1<<0)
#define SPI_TX_EN            (1<<1)
#define SPI_RX_NOT_EN        (1<<2)
#define SPI_TX_FIFO(x)       (x<<3)
#define SPI_RX_FIFO(x)       (x<<6)
#define SPI_OVR              (1<<9)
//for overflow

//SSPIx FIFO Status Register
#define SPI_TX_EMPTY          (1<<0)
#define SPI_TX_DUAL           (1<<1)
#define SPI_TX_QUAD           (1<<2)
#define SPI_TX_OCTAL          (1<<3)
#define SPI_TX_HALF           (1<<4)       
#define SPI_TX_24             (1<<5)
#define SPI_TX_28             (1<<6)
#define SPI_TX_30             (1<<7)
#define SPI_TX_FULL           (1<<8)

#define SPI_RX_EMPTY          (1<<9)
#define SPI_RX_DUAL           (1<<10)
#define SPI_RX_QUAD           (1<<11)
#define SPI_RX_OCTAL          (1<<12)
#define SPI_RX_HALF           (1<<13)
#define SPI_RX_24             (1<<14)
#define SPI_RX_28             (1<<15)
#define SPI_RX_30             (1<<16)
#define SPI_RX_FULL           (1<<17)

//SSPIx Interrupt Enable Register
#define SPI_TX_EMPTY_INTR_EN    (1<<0)
#define SPI_TX_DUAL_INTR_EN     (1<<1)
#define SPI_TX_QUAD_INTR_EN     (1<<2)
#define SPI_TX_OCTAL_INTR_EN    (1<<3)
#define SPI_TX_HALF_INTR_EN     (1<<4)
#define SPI_TX_24_INTR_EN       (1<<5)
#define SPI_TX_28_INTR_EN       (1<<6)
#define SPI_TX_30_INTR_EN       (1<<7)
#define SPI_TX_FULL_INTR_EN     (1<<8)

#define SPI_RX_EMPTY_INTR_EN    (1<<9)
#define SPI_RX_DUAL_INTR_EN     (1<<10)
#define SPI_RX_QUAD_INTR_EN     (1<<11)
#define SPI_RX_OCTAL_INTR_EN    (1<<12)
#define SPI_RX_HALF_INTR_EN     (1<<13)
#define SPI_RX_24_INTR_EN       (1<<14)
#define SPI_RX_28_INTR_EN       (1<<15)
#define SPI_RX_30_INTR_EN       (1<<16)
#define SPI_RX_FULL_INTR_EN     (1<<17)
#define SPI_RX_OVERRUN_INTR_EN  (1<<18)

// SSPIx NCS Control Register
#define SPI_NCS_SELECT(x)	(x<<0)
#define SPI_NCS_SW(x)		(x<<1) /* Default value of rg_ncs_sw is one */


int SPI_Init(SPI_Config_t *config)
{ 
  if (config->spi_number < SSPI_MAX_COUNT & config->spi_number >= 0){
    switch (config->spi_number)
    {
    case 0:
      sspi_instance[config->spi_number] = SPI0;
      //#define SPI0  ((SPI_Type*)  SPI0_BASE)
      //each spi type differs by 100U ,They are of SPI type (The register map)
      //assigning the pointer to the used spi 
      break;
    case 1:
      sspi_instance[config->spi_number] = SPI1;
      break;
    case 2:
      sspi_instance[config->spi_number] = SPI2;
      break;
    case 3:
      sspi_instance[config->spi_number] = SPI3;
      break;
    default:
      break;
    }
    log_debug("\nSPI%d Initialized..", config->spi_number);
    return SUCCESS;
  }
  else{
    log_error("\nInvalid SPI instance %d. This SoC supports only SPI-0 to SPI-3", config->spi_number);
    return ENODEV;
  }
}


int SPI_Busy(SPI_Config_t *config){
  uint16_t temp; 
  while(1){
    temp = sspi_instance[config->spi_number]->COMM_STATUS;
    temp =  temp & SPI_BUSY;
    //zeroth bit of communication status register
    log_debug("\nchecking SPI%d busy", config->spi_number);
    if (temp == SPI_BUSY){
      log_info("\nSPI%d is busy", config->spi_number);
      //stuck here till spi is free
    }
    else {
      break;
    }
  }
  log_debug("\nSPI%d is not busy", config->spi_number);
  return SUCCESS;
}


int SCLK_Config(SPI_Config_t *config){
  SPI_Busy(config);
  if ((config->pol == 0 && config->pha == 1)||(config->pol == 1 && config->pha == 0)){
    log_info("\nInvalid Clock Configuration (NO FALLING EDGE). Hence Exiting..!");
    return EINVAL;
    //maybe it doesnt support falling edge
  }
  else{
    //wth
    sspi_instance[config->spi_number] -> CLK_CTRL =  SPI_TX2SS_DELAY(config->hold_time) | SPI_SS2TX_DELAY(config->setup_time) | SPI_PRESCALE(config->prescale)| SPI_CLK_POLARITY(config->pol) | SPI_CLK_PHASE(config->pha);
    return SUCCESS;
  }
}


int SPI_Configure_Clock_In_Hz(SPI_Config_t *config){
  uint8_t prescaler = (CLOCK_FREQUENCY / config->bit_rate) - 1;
  if(config->bit_rate >= CLOCK_FREQUENCY){
    log_info("\n Invalid bit rate value. Bit rate should be less than CLOCK_FREQUENCY");
    //cant go to that speed.
    return EINVAL;
  }
  uint32_t temp = sspi_instance[config->spi_number] -> CLK_CTRL & (~SPI_PRESCALE(0xFF) );
  sspi_instance[config->spi_number] -> CLK_CTRL = temp | SPI_PRESCALE(prescaler);
  //configuring clock speed in pre scalar value
  return SUCCESS;
}


int SPI_CTRL_Config(SPI_Config_t *config){
  int out_en=0;
  if (config->master_mode==MASTER){
    out_en = SPI_OUT_EN_SCLK | SPI_OUT_EN_NCS | SPI_OUT_EN_MOSI ;
    //master give wsthe output
    }
  else{
    out_en = SPI_OUT_EN_MISO;
    //slave gives the output
  }
  SPI_Busy(config);
  sspi_instance[config->spi_number]->CTRL = \
        SPI_MODE(config->master_mode) | SPI_LSB_FIRST(config->lsb_first) | SPI_COMM_MODE(config->comm_mode) | \
        SPI_TOTAL_BITS_TX(config->spi_size) | SPI_TOTAL_BITS_RX(config->spi_size) | \
        out_en;
        //depends on the master or slave
  
  return SUCCESS;
}


int SPI_Enable(SPI_Config_t *config){
  uint32_t temp = sspi_instance[config->spi_number]->CTRL;
  SPI_Busy(config);
  //If busy wait if not busy set up the value of the enable bit
  sspi_instance[config->spi_number]->CTRL = temp | SPI_ENABLE(ENABLE);

  return SUCCESS;
}


int SPI_Disable(SPI_Config_t *config){
  uint32_t temp = sspi_instance[config->spi_number]->CTRL;
  uint32_t disable = ~SPI_ENABLE(ENABLE);
  //sspi_busy(config->spi_number);
  sspi_instance[config->spi_number]->CTRL = temp & disable; //| SPI_ENABLE(DISABLE);

  return SUCCESS;
}


int SPI8_Transmit_Data(SPI_Config_t *config, uint8_t data){
  volatile int temp = SPI_Check_Tx_Fifo_32(config);
  if(temp == 0){
    sspi_instance[config->spi_number]->TX.data_8 = data;
    //TX union -> Added values as numbers

    log_debug("tx_data = %d\n", data);
  }
  else{
    log_debug("\nTX FIFO is full");
    return ENOBUFS;
  }
  // SPI_Wait_Till_Tx_Complete(config);
  return SUCCESS;
}


int SPI16_Transmit_Data(SPI_Config_t *config, uint16_t data){
  volatile int temp = SPI_Check_Tx_Fifo_30(config);
  if (temp == 0){
    sspi_instance[config->spi_number]->TX.data_16 = data;
    log_debug("tx_data = %d\n", data);
  }
  else{
    log_debug("\nTX FIFO has not enough space.");
    return ENOBUFS;//no buffer space available
  }
  // SPI_Wait_Till_Tx_Complete(config);
  return SUCCESS;
}


int SPI32_Transmit_Data(SPI_Config_t *config, uint32_t data){
  volatile int temp = SPI_Check_Tx_Fifo_28(config);
  if (temp == 0){
    sspi_instance[config->spi_number]->TX.data_32 = data;
    log_debug("tx_data = %d\n", data);
  }
  else{
    log_debug("\nTX FIFO has not enough space.");
    return ENOBUFS;
  }
  printf("\n%x",sspi_instance[config->spi_number]->FIFO_STATUS);
  // SPI_Wait_Till_Tx_Complete(config);
  return SUCCESS;
}


int SPI_Wait_Till_Tx_Not_En(SPI_Config_t *config){
  uint16_t temp;
  while(1){
    temp = sspi_instance[config->spi_number]->COMM_STATUS;
    temp = temp & SPI_TX_EN;
    if (temp == 0){
      break;
    }
  }

  return SUCCESS;
}


int SPI_Wait_Till_Tx_Complete(SPI_Config_t *config){

    uint32_t temp;
    int i = 0;
    log_debug("\nChecking TX FIFO is empty.");
    while(1){
      // SPI_Wait_Till_Tx_Not_En(config);
      temp = sspi_instance[config->spi_number]->FIFO_STATUS;
      temp = temp & SPI_TX_EMPTY;
      if (temp == 0){
        // sspi_enable(spi_number);
        if (i == 0){
          log_debug("\nTX FIFO is not empty, waiting till the FIFO gets empty.");
        }
        else{
          log_debug("\nTX FIFO is not empty");
        }
        i += 1;
      }
      else{
        // uint32_t temp_1 = sspi_instance[spi_number]->comm_status;
        while ((sspi_instance[config->spi_number]->COMM_STATUS) & SPI_BUSY);
        break;
      }
    }
    log_debug("\nTX FIFO is empty");
    return SUCCESS;
}


int SPI_Check_Tx_Fifo_32(SPI_Config_t *config){
  uint32_t temp;
  log_debug("\nChecking if TX FIFO is full.");
  temp = sspi_instance[config->spi_number]->FIFO_STATUS;
  temp = temp & SPI_TX_FULL;
  if (temp == SPI_TX_FULL){
    log_debug("\nTX FIFO is full, waiting till fifo gets not full.");
    //So tgat we dont  keep transmitting and to ensure that data is not lost.
    return ENOBUFS;
  }
  else{
    log_debug("\nTX FIFO is not full.");
    return SUCCESS;
  }
}


int SPI_Check_Tx_Fifo_30(SPI_Config_t *config){
  uint32_t temp;
  uint16_t temp1;
  log_debug("\nChecking if TX FIFO is less than 30.");
  temp = sspi_instance[config->spi_number]->FIFO_STATUS;
  temp = temp & SPI_TX_30;
  temp1 = sspi_instance[config->spi_number]->COMM_STATUS;
  temp1 = temp1 & SPI_TX_FIFO(7);
  if ((temp == SPI_TX_30 && temp1 == SPI_TX_FIFO(7)) || (temp1 < SPI_TX_FIFO(7))){
    log_debug("\nTX FIFO is not 30.");
    return SUCCESS;
  }
  else{
    log_debug("\nTX FIFO contains less than or equal to 30.");
    return ENOBUFS;
  }
}


int SPI_Check_Tx_Fifo_28(SPI_Config_t *config){
  uint32_t temp;
  uint16_t temp1;
  log_debug("\nChecking if TX FIFO is less than 28.");
  temp = sspi_instance[config->spi_number]->FIFO_STATUS;
  temp = temp & SPI_TX_28;
  temp1 = sspi_instance[config->spi_number]->COMM_STATUS;
  temp1 = temp1 & SPI_TX_FIFO(7);
  if ((temp == SPI_TX_28 && temp1 == SPI_TX_FIFO(6)) || (temp1 < SPI_TX_FIFO(6))){
    log_debug("\nTX FIFO is not 28.");
    return SUCCESS;
  }
  else{
    log_debug("\nTX FIFO contains less than or equal to 28.");
    return ENOBUFS;
  }
}


int SPI_Wait_Till_Rxfifo_Not_Empty(SPI_Config_t *config){
  uint32_t temp;
  while(1){
    temp = sspi_instance[config->spi_number]->FIFO_STATUS;
    temp = temp & SPI_RX_EMPTY;
    if (temp == 0){
      log_debug("\nRX FIFO is not empty.");
      break;
    }
  }

  return SUCCESS;
}


int SPI_Wait_Till_Rxfifo_2(SPI_Config_t *config){
  uint32_t temp;
  uint16_t temp1;
  while(1){
    temp = sspi_instance[config->spi_number]->FIFO_STATUS;
    temp = temp & SPI_RX_DUAL;
    temp1 = sspi_instance[config->spi_number]->COMM_STATUS;
    temp1 = temp1 & SPI_RX_FIFO(7);
    if((temp1 >= SPI_RX_FIFO(1))){
      log_debug("\nRX FIFO is more than or equal to 2 bytes.");
      break;
    }
  }

  return SUCCESS;
}


int SPI_Wait_Till_Rxfifo_4(SPI_Config_t *config){
  uint32_t temp;
  uint16_t temp1;
  while(1){
    temp = sspi_instance[config->spi_number]->FIFO_STATUS;
    temp = temp & SPI_RX_QUAD;
    temp1 = sspi_instance[config->spi_number]->COMM_STATUS;
    temp1 = temp1 & SPI_RX_FIFO(7);
    if (temp1 >= SPI_RX_FIFO(2)){
      log_debug("\nRX FIFO is more than or equal to 4 bytes.");
      break;
    }
  }

  return SUCCESS;
}


uint8_t SPI8_Receive_Data(SPI_Config_t *config){
  uint8_t rx_data;
  SPI_Wait_Till_Rxfifo_Not_Empty(config);
  rx_data = sspi_instance[config->spi_number]->RX.data_8;
  return rx_data;
}


uint16_t SPI16_Receive_Data(SPI_Config_t *config){
  uint16_t rx_data;
  SPI_Wait_Till_Rxfifo_2(config);
  rx_data = sspi_instance[config->spi_number]->RX.data_16;
  return rx_data; //why are we returning that data
}


uint32_t SPI32_Receive_Data(SPI_Config_t *config){
  uint32_t rx_data;
  SPI_Wait_Till_Rxfifo_4(config);
  rx_data = sspi_instance[config->spi_number]->RX.data_32;
  return rx_data;
}


int INTER_Enable_Config(SPI_Config_t *config){
  sspi_instance[config->spi_number]->INTR_EN = config->value;

  return SUCCESS;
}


void sspi_rx_fifo_flush (SPI_Config_t *config) {
    sspi_instance[config->spi_number]->CTRL |= SPI_FLUSH;
    //set that bit
    sspi_instance[config->spi_number]->CTRL &= ~(SPI_FLUSH);
    //clear that bit
}


/* Use these function for DMA transaction
   val 0x01 -> 8 bits
   val 0x02 -> 16 bits
   val 0x03 -> 32 bits
   val 0x04 -> 64 bits */

/* Use this for SIMPLEX_TX spi mode */
void wr_dma_tx_size_into_spi(SPI_Config_t *config, int tx_size) {

    uint32_t temp = sspi_instance[config->spi_number]->CTRL;
    uint32_t mask = (SPI_DMA_RX_SIZE(0x07) | SPI_OUT_EN_MOSI | SPI_OUT_EN_MISO | SPI_OUT_EN_NCS | \
		     SPI_OUT_EN_SCLK | SPI_TOTAL_BITS_RX(0xFF) | SPI_TOTAL_BITS_TX(0xFF) | \
		     SPI_COMM_MODE(0x03) | SPI_FLUSH | SPI_LSB_FIRST(1) | SPI_ENABLE(1) | SPI_MODE(1));

    sspi_instance[config->spi_number]->CTRL = (temp & mask) | SPI_DMA_TX_SIZE((tx_size & 0x07));
}


/* Use this for SIMPLEX_RX spi mode */
void wr_dma_rx_size_into_spi(SPI_Config_t *config, int rx_size) {

    uint32_t temp = sspi_instance[config->spi_number] -> CTRL;
    uint32_t mask = (SPI_DMA_TX_SIZE(0x07) | SPI_OUT_EN_MOSI | SPI_OUT_EN_MISO | SPI_OUT_EN_NCS | \
		     SPI_OUT_EN_SCLK | SPI_TOTAL_BITS_RX(0xFF) | SPI_TOTAL_BITS_TX(0xFF) | \
		     SPI_COMM_MODE(0x03) | SPI_FLUSH | SPI_LSB_FIRST(1) | SPI_ENABLE(1) | SPI_MODE(1));

    sspi_instance[config->spi_number] -> CTRL = (temp & mask) | SPI_DMA_RX_SIZE((rx_size & 0x07));
}


/* Use this for HALF_DUPLEX and FULL_DUPLEX spi mode */
void wr_dma_tx_rx_size_into_spi(SPI_Config_t *config, int tx_size, int rx_size) {

    uint32_t temp = sspi_instance[config->spi_number] -> CTRL;
    uint32_t mask = (SPI_OUT_EN_MOSI | SPI_OUT_EN_MISO | SPI_OUT_EN_NCS | SPI_OUT_EN_SCLK | \
		     SPI_TOTAL_BITS_RX(0xFF) | SPI_TOTAL_BITS_TX(0xFF) | SPI_COMM_MODE(0x03) | \
		     SPI_FLUSH | SPI_LSB_FIRST(1) | SPI_ENABLE(1) | SPI_MODE(1));

    sspi_instance[config->spi_number] -> CTRL = (temp & mask) | SPI_DMA_TX_SIZE((tx_size & 0x07)) | \
						 SPI_DMA_RX_SIZE((rx_size & 0x07));
}


/*
 * This function is to choose between existing spi ncs and the new software control ncs (SPI_NCS_SELECT).
 * If 1, it chooses software control ncs (SPI_NCS_SELECT)
 * If 0, spi ncs
 */
void sspi_ncs_select(SPI_Config_t *config, int val) {
    sspi_instance[config->spi_number] -> NCS_CTRL = SPI_NCS_SW(1) | SPI_NCS_SELECT((val & 0x1));
}


/* This function sets the ncs_out, according to the SPI_NCS_SW field value on ncs control register
 * val 1 -> high
 * val 0 -> low
 */
void sspi_ncs_sw_control(SPI_Config_t *config, int val) {
    uint8_t temp = sspi_instance[config->spi_number] -> NCS_CTRL;
    uint8_t mask = SPI_NCS_SELECT(1);
    sspi_instance[config->spi_number] -> NCS_CTRL = (temp & mask) | SPI_NCS_SW((val & 0x1));
}


int SPI_Configure(SPI_Config_t *config,int which_ncs)
//0 hw
//1 sw
{
  SPI_Init(config);
  SCLK_Config(config);
  sspi_ncs_select(config, which_ncs);
  SPI_CTRL_Config(config);
  return SUCCESS;
}


int SPI_Transceive(SPI_Config_t *config, const struct spi_buf_set *tx_bufs,
			                    const struct spi_buf_set *rx_bufs){
  
int len = tx_bufs ? tx_bufs->buffers->len : rx_bufs->buffers->len;  

switch (config->comm_mode) {
    case FULL_DUPLEX:
        switch (config->bits) {
            case DATA_SIZE_8:
                for (int i = 0; i < len; i++) {
                    SPI8_Transmit_Data(config, ((uint8_t*)(tx_bufs->buffers->buf))[i]);
                    ((uint8_t*)(rx_bufs->buffers->buf))[i] = SPI8_Receive_Data(config);
                  }
            break;

            case DATA_SIZE_16:
                for (int i = 0; i < len; i++) {
                    SPI16_Transmit_Data(config, ((uint16_t*)(tx_bufs->buffers->buf))[i]);
                    ((uint16_t*)(rx_bufs->buffers->buf))[i] = SPI16_Receive_Data(config);
                  }
            break;

            case DATA_SIZE_32:
                for (int i = 0; i < len; i++) {
                    SPI32_Transmit_Data(config, ((uint32_t*)(tx_bufs->buffers->buf))[i]);
                    ((uint32_t*)(rx_bufs->buffers->buf))[i] = SPI32_Receive_Data(config);
                  }
            break;
          }
    break;

    case SIMPLEX_TX:
        switch (config->bits) {
            case DATA_SIZE_8:
                for (int i = 0; i < len; i++) {
                    SPI8_Transmit_Data(config, ((uint8_t*)(tx_bufs->buffers->buf))[i]);
                  }
            break;

            case DATA_SIZE_16:
                for (int i = 0; i < len; i++) {
                    SPI16_Transmit_Data(config, ((uint16_t*)(tx_bufs->buffers->buf))[i]);
                  }
            break;

            case DATA_SIZE_32:
                for (int i = 0; i < len; i++) {
                    SPI32_Transmit_Data(config, ((uint32_t*)(tx_bufs->buffers->buf))[i]);
                 }
            break;
          }
    break;

    case SIMPLEX_RX:
        switch (config->bits) {
            case DATA_SIZE_8:
                for (int i = 0; i < len; i++) {
                    ((uint8_t*)(rx_bufs->buffers->buf))[i] = SPI8_Receive_Data(config);
                  }
            break;

            case DATA_SIZE_16:
                for (int i = 0; i < len; i++) {
                    ((uint16_t*)(rx_bufs->buffers->buf))[i] = SPI16_Receive_Data(config);
                  }
            break;

            case DATA_SIZE_32:
                for (int i = 0; i < len; i++) {
                    ((uint32_t*)(rx_bufs->buffers->buf))[i] = SPI32_Receive_Data(config);
                  }
            break;
          }
    break;

    case HALF_DUPLEX:
        switch (config->bits) {
            case DATA_SIZE_8:
                for (int i = 0; i < len; i++) {
                    SPI8_Transmit_Data(config, ((uint8_t*)(tx_bufs->buffers->buf))[i]);
                    ((uint8_t*)(rx_bufs->buffers->buf))[i] = SPI8_Receive_Data(config);
                  }
            break;

            case DATA_SIZE_16:
                for (int i = 0; i < len; i++) {
                    SPI16_Transmit_Data(config, ((uint8_t*)(tx_bufs->buffers->buf))[i]);
                    ((uint8_t*)(rx_bufs->buffers->buf))[i] = SPI16_Receive_Data(config);
                  }
            break;

            case DATA_SIZE_32:
                for (int i = 0; i < len; i++) {
                    SPI32_Transmit_Data(config, ((uint8_t*)(tx_bufs->buffers->buf))[i]);
                    ((uint8_t*)(rx_bufs->buffers->buf))[i] = SPI32_Receive_Data(config);
                  }
            break;
          }
    break;
  } 
return SUCCESS; 
}

#pragma GCC diagnostic pop
//these pragmas applicable only in this range :)