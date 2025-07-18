/**
 * Project                               : Secure IoT SoC
 * Name of the file                      : uart_driver.c
 * Brief Description of file             : This is a Baremetal UART Driver file for Mindgrove Silicon's UART Peripheral.
 * Name of Author                        : Kapil Shyam. M, Santhosh Pavan. S, Vignesh Kumar. J 
 * Email ID                              : <kapilshyamm@gmail.com>  <santhosh@mindgrovetech.in>  <vigneshkumar@mindgrovetech.in>
 * 
 * @file uart.c
 * @brief Contains the driver routines for UART interface.
 * @details The UART driver .has software functions to configure, transmit
 * and receive over UART interface.
 * @version 1.0
 * @date 2024-10-08
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2024. All rights reserved.
 * 
 */

#include "uart.h"
#include "secure_iot.h"

#define STS_CAN_TAKE_INPUT  (1 << 10)
#define STS_OUTP_READY      (1 << 9)
#define STS_RX_THRESHOLD    (1 << 8)
#define BREAK_ERROR	    (1 << 7)
#define FRAME_ERROR	    (1 << 6)
#define OVERRUN        	    (1 << 5)
#define PARITY_ERROR        (1 << 4)
#define STS_RX_FULL 	    (1 << 3)
#define STS_RX_NOT_EMPTY    (1 << 2)
#define STS_TX_FULL 	    (1 << 1)
#define STS_TX_EMPTY 	    (1 << 0)

extern void *interrupt_arg[];

volatile UART_Type *uart_instance[MAX_UART_COUNT];/*! The above code is defining an array of pointers to structures of type "UART_Type" with a maximum
                                                    size of "MAX_UART_COUNT". The "volatile" keyword indicates that the contents of the structure
                                                    pointed to by each element of the array may change at any time, even if there is no explicit
                                                    modification in the code. */

/* UART control register */
#define UART_PULLUP_EN(x) 	( (x & 1) << 13)
#define UART_RX_THRESH(x)	( (x & 7) << 10)	/*! 00 - None; 01 - 8bits; 10 - 16bits; 11 - 32bits; 100 - 64bits */
#define UART_TX_THRESH(x)	( (x & 7) << 7 )	/*! 00 - None; 01 - 8bits; 10 - 16bits; 11 - 32bits; 100 - 64bits */

#define UART_CHARSIZE(x)	( (x & 3) << 5) 	/*! Maximum length 32 bits */
#define PARITY(x) 		( (x & 3) << 3 ) 	/*! 00 --- No parity; 01 -Odd Parity; 10 - Even Parity;  11 - Unused */
#define STOP_BITS(x) 		( (x & 3) << 1) 	/*! 00 - 1 stop bits, 01 - 1.5 stop bits; 10 - 2 stop bits; 11 unused */

uint8_t UART_Available(UART_Config_t *uart_config)
{
	return (uart_instance[uart_config->uart_num]->STATUS_REG & STS_RX_NOT_EMPTY);
}

uint8_t UART_Rx_Polling(UART_Config_t *uart_config)
{
  return (((uart_instance[uart_config->uart_num]->STATUS_REG) & (STS_RX_NOT_EMPTY)) >> 2);
}


uint8_t UART_Interrupt_Enable(UART_Config_t *uart_config, uint16_t interrupt)
{
	log_debug("\nEnabling uart interrupt for this interrupt id%d",interrupt);
	uart_instance[uart_config->uart_num]->INTR_EN = interrupt ;
	return SUCCESS;
}

uint8_t UART_Init(UART_Config_t const *uart_config)
{
	log_info("\nInitializing uart instance %d",uart_config->uart_num);
	
  	switch (uart_config->uart_num)
	{	
		case 0:
		    uart_instance[uart_config->uart_num] = UART0;
			break;
		case 1:
			uart_instance[uart_config->uart_num] = UART1;
			break;
		case 2:
			uart_instance[uart_config->uart_num] = UART2;
			break;
		case 3:
			uart_instance[uart_config->uart_num] = UART3;
			PINMUX_EnableUART(uart_config->uart_num);
			break;
		case 4:
			uart_instance[uart_config->uart_num] = UART4;
			PINMUX_EnableUART(uart_config->uart_num);
			break;

		default:
			log_error("\nInvalid UART instance %d. This SoC supports only UART-0 to UART-2, and UART-0 is already in use for printing ", uart_config->uart_num);
       		return ENOINST;
			break;
	}

   if((uart_config->uart_num>0) && (uart_config->uart_num <= 4))
	{
   		UART_Set_Baudrate(uart_config);
   		UART_Config(uart_config);
	}
	log_debug("\nUART%d Initialized..", uart_config->uart_num);
		

	return SUCCESS;
  }


#undef getchar
int getchar()
{
	while((((UART_Type*)(0x11300))->STATUS_REG & STS_RX_NOT_EMPTY) == 0);
	return (((UART_Type*)(0x11300))->RX_REG.data_8);
}

int putchar(int ch)
{
	while(((UART_Type*)(0x11300))->STATUS_REG & STS_TX_FULL);

	((UART_Type*)(0x11300))->TX_REG.data_8 = ch;

	return SUCCESS;
}

uint8_t UART_Set_Baudrate(UART_Config_t *uart_config)
{ 
	
	log_info("\nInitializing baudrate as %d to uart instance %d",uart_config->baudrate, uart_config->uart_num);
	float baud_count = 0.0;
	unsigned short baud_value = 0;
    baud_count = CLOCK_FREQUENCY_BASE  / (16.0 * uart_config->baudrate);
	if( (baud_count - (int) baud_count) > 0.5 )
		baud_value = (((short) baud_count) + 1) ;
	else
		baud_value =  (short) baud_count;

	uart_instance[uart_config->uart_num]->BAUD_REG = (short) baud_value;
	return SUCCESS;
}

void UART_Set_Baudrate_Sim(UART_Config_t *uart_config)
{
	float baud_count = 0.0;
    unsigned short baud_value = 0;
	baud_count = CLOCK_FREQUENCY_BASE  / (16.0 * uart_config->baudrate); //clock frequency sim is changed here
	if( (baud_count - (int) baud_count) > 0.5 )
		baud_value = (((short) baud_count) + 1) ;
	else
		baud_value =  (short) baud_count;

	uart_instance[uart_config->uart_num]->BAUD_REG = (short) baud_value;
}

uint8_t UART_Config( UART_Config_t *uart_config)
{
	uart_instance[uart_config->uart_num]->CTRL = (UART_PULLUP_EN(uart_config->pullup) | UART_RX_THRESH(uart_config->receive_mode) | UART_TX_THRESH(uart_config->transfer_mode) | \
												  UART_CHARSIZE(uart_config->char_size) | PARITY(uart_config->parity) | STOP_BITS(uart_config->stop_bits));
	uart_instance[uart_config->uart_num]->DELAY_REG = uart_config->delay;
	return SUCCESS;
}

uint8_t UART_RX_Threshold(UART_Config_t *uart_config, uint8_t rxthreshold)
{
	uart_instance[uart_config->uart_num]->RX_THRESHOLD = rxthreshold;
	return SUCCESS;
}

uint8_t UART_Write(UART_Config_t *uart_config , struct uart_buf *rx_bufs)
{
	log_info("\nTransmitting data via uart instance %d",uart_config->uart_num);
	printf("\nTransmitting data via uart instance %d",uart_config->uart_num);
	uint32_t i = 0;
	uint32_t length = rx_bufs->len;
	Data temp;

	switch (uart_config->transfer_mode)
	{
		case DATA_SIZE_8:
			
			do
			{
				temp.data_8 = ((uint8_t*)(rx_bufs->uart_data))[i++];
				while(uart_instance[uart_config->uart_num]->STATUS_REG & STS_TX_FULL);
				uart_instance[uart_config->uart_num]->TX_REG.data_8 = temp.data_8;
			} while((--length)!= 0);
		break;

		case DATA_SIZE_16:
			
			do
			{ 
				temp.data_16 = ((uint16_t*)(rx_bufs->uart_data))[i++];
				while(!(uart_instance[uart_config->uart_num]->STATUS_REG & STS_CAN_TAKE_INPUT));
				uart_instance[uart_config->uart_num]->TX_REG.data_16 = temp.data_16;
			} while((--length)!= 0);
		break;
		case DATA_SIZE_32:
			
			do
			{
				temp.data_32 = ((uint32_t*)(rx_bufs->uart_data))[i++];
				while(!(uart_instance[uart_config->uart_num]->STATUS_REG & STS_CAN_TAKE_INPUT));
				uart_instance[uart_config->uart_num]->TX_REG.data_32 = temp.data_32;
			} while((--length)!= 0);
		break;
		default:

			log_error("\n\rEntered wrong transfer mode ");
			do
			{
				temp.data_8 = ((uint8_t*)(rx_bufs->uart_data))[i++];
				while(uart_instance[uart_config->uart_num]->STATUS_REG & STS_TX_FULL);
				uart_instance[uart_config->uart_num]->TX_REG.data_8 = temp.data_8;
			} while((--length)!= 0);

		break;
	}



  return SUCCESS;
}

uint8_t UART_Flush(UART_Config_t *uart_config)
{
	__attribute__((unused)) uint32_t temp;

	while ((uart_instance[uart_config->uart_num]->STATUS_REG & STS_RX_NOT_EMPTY))
	{
		temp = uart_instance[uart_config->uart_num]->RX_REG.data_8 ;
	}

	return SUCCESS;
}

uint8_t UART_Read_Character(UART_Config_t *uart_config , struct uart_buf *rx_bufs)
{
	log_info("\nReading a character from Rx buffer of instance %d",uart_config->uart_num);	

	switch (uart_config->receive_mode)
	{
		case DATA_SIZE_8:

			while (!(uart_instance[uart_config->uart_num]->STATUS_REG & STS_RX_NOT_EMPTY));
			*(uint8_t*)(rx_bufs->uart_data) = uart_instance[uart_config->uart_num]->RX_REG.data_8;
			log_debug("\nSuccessfully read a character from software buffer of instance %d",uart_config->uart_num);	
			break;
		case DATA_SIZE_16:

			while (!(uart_instance[uart_config->uart_num]->STATUS_REG & STS_OUTP_READY));
			*(uint16_t*)(rx_bufs->uart_data)  = uart_instance[uart_config->uart_num]->RX_REG.data_16;
			log_debug("\nSuccessfully read a character from software buffer of instance %d",uart_config->uart_num);	
			break;
		case DATA_SIZE_32:

			while (!(uart_instance[uart_config->uart_num]->STATUS_REG & STS_OUTP_READY));
			*(uint32_t*)(rx_bufs->uart_data)  = uart_instance[uart_config->uart_num]->RX_REG.data_32;
			log_debug("\nSuccessfully read a character from software buffer of instance %d",uart_config->uart_num);	
			break;
		default:
			log_error("\n\rEntered wrong receive mode ");
			break;
	}
	
	return SUCCESS;
}

uint8_t UART_Read_String(UART_Config_t *uart_config , struct uart_buf *rx_bufs)
{
	log_info("\nReading the string of instance %d\n\r",uart_config->uart_num);	
	uint32_t i = 0;
	uint32_t len =(rx_bufs->len);
	while((len--)>0)
	{
		switch (uart_config->receive_mode)
		{
			case DATA_SIZE_8:
				while (!(uart_instance[uart_config->uart_num]->STATUS_REG & STS_RX_NOT_EMPTY));
				((uint8_t*)(rx_bufs->uart_data))[i++] = uart_instance[uart_config->uart_num]->RX_REG.data_8;
				if(len==0)
				{
					((uint8_t*)(rx_bufs->uart_data))[i]=0x00;
				}
				break;
			case DATA_SIZE_16:
				while (!(uart_instance[uart_config->uart_num]->STATUS_REG & STS_OUTP_READY));
				((uint16_t*)(rx_bufs->uart_data))[i++]  = uart_instance[uart_config->uart_num]->RX_REG.data_16;
				if(len==0)
				{
					((uint16_t*)(rx_bufs->uart_data))[i]=0x00;
				}
				break;
			case DATA_SIZE_32:
				while (!(uart_instance[uart_config->uart_num]->STATUS_REG & STS_OUTP_READY));
				((uint32_t*)(rx_bufs->uart_data))[i++]  = uart_instance[uart_config->uart_num]->RX_REG.data_32;
				if(len==0)
				{
					((u_int32_t*)(rx_bufs->uart_data))[i]=0x00;
				}
				break;
		
			default:
				log_error("\n\rEntered wrong receive mode ");
				break;
		}
	}
	log_debug("\nData has been stored successfully");		
	return SUCCESS;
}

uint8_t UART_Write_Wait(UART_Config_t *uart_config)
{
	while((uart_instance[uart_config->uart_num]->STATUS_REG &STS_TX_EMPTY) ==0);
	return SUCCESS;
}