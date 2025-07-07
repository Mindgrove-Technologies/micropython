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
#include "secure_iot.h"
#ifndef UART_DRIVER_H
#define UART_DRIVER_H
#include <stdio.h>
#include "uart.h"
//src -->uart transmit
CircularBuffer uart_buffers[3];
// uint8_t buffer[BUFFER_SIZE];
//uint16_t head;
//uint16_t tail;
volatile UART_Type *uart_instance[MAX_UART_COUNT]; //creates an array
//This is a pointer to the register structure.
UART_Config_t uart_b={
    .uart_num = 0, //0 or 1/2 depending on what you're using
    .baudrate = 115200,
    .char_size = 8,
    .stop_bits = 1,
    .parity = 0,
	.delay=1, //not needed
	.rxthreshold=10, //not needed 
    .buffer_enable = 0
};
UART_Config_t *UART_Config_1=&uart_b;
//I am assigning a address value here 

#define STS_RX_THRESHOLD    0x1 << 8
#define BREAK_ERROR	    1 << 7
#define FRAME_ERROR	    1 << 6
#define OVERRUN        	    1 << 5
#define PARITY_ERROR        1 << 4
#define STS_RX_FULL 	    1 << 3
#define STS_RX_NOT_EMPTY    1 << 2
#define STS_TX_FULL 	    1 << 1
#define STS_TX_EMPTY 	    1 << 0
extern void *interrupt_arg[];
//for now its 3 --> Different UART_Type object for each type of uart
/*! The above code is defining an array of pointers to structures of type "UART_Type" with a maximum
size of "MAX_UART_COUNT". The "volatile" keyword indicates that the contents of the structure
pointed to by each element of the array may change at any time, even if there is no explicit
modification in the code. */

// #define BUFFER_SIZE 128 // Size of the buffer
// typedef struct {
//     uint8_t buffer[BUFFER_SIZE];
//     uint16_t head;
//     uint16_t tail;
// } CircularBuffer;

//extern CircularBuffer uart_buffers[3]={0};

void buffer_init(const UART_Config_t *uart_config) {
	//Initializes the software circular buffer for the specified UART instance by resetting head and tail indices.
	//initiall head &tail =0
    uart_buffers[uart_config->uart_num].head = 0;
    uart_buffers[uart_config->uart_num].tail = 0;
}

int buffer_is_full(CircularBuffer *cb) {
    return ((cb->head + 1) % BUFFER_SIZE) == cb->tail;
	 //The modulo % BUFFER_SIZE ensures that the indices "loop back" to the start instead of going out of bounds.
}

int buffer_is_empty(CircularBuffer *cb) {
    return cb->head == cb->tail;
	//checking if the buffer is empty
} 

uint8_t UART_Available(UART_Config_t *uart_config)
{
	return (uart_instance[uart_config->uart_num]->STATUS_REG & STS_RX_NOT_EMPTY);
	//for accessing the status register of the uart
	//status not empty flag
}

uint8_t UART_Rx_Polling(UART_Config_t *uart_config)
{
  return (((uart_instance[uart_config->uart_num]->STATUS_REG) & (STS_RX_NOT_EMPTY)) >> 2);
  //to check if the returned value is 0 or 1
}

uint8_t UART_Receive(void*ptr)
{
	//This function is set as a generic interrupt handler.
	//Many interrupt systems (like PLIC) expect a function pointer with a void * argument so that it can pass any custom data (like the specific UART instance).
	//This makes the interrupt handling flexible and modular—you can pass different structs or context-specific info.
	UART_Config_t *uart_config = (UART_Config_t *)ptr;
    //why are we inputing as a void pointer
    if (!buffer_is_full(&uart_buffers[uart_config->uart_num])) {
		log_debug("\nSoftware buffer is not full");
		uint8_t temp=uart_instance[uart_config->uart_num]->RX_REG.data_8;
		
        uart_buffers[uart_config->uart_num].buffer[uart_buffers[uart_config->uart_num].head] = temp;  // Store the received data in the buffer
        uart_buffers[uart_config->uart_num].head = (uart_buffers[uart_config->uart_num].head + 1) % BUFFER_SIZE;  // Move the head forward
		
    } else {
		log_error("\nSoftware buffer is full");
		return ECBUFFULL; //Error code for buffer full
    }
	 return SUCCESS;
}

uint8_t UART_Interrupt_Enable(UART_Config_t *uart_config,unsigned short interrupt)
{
	log_debug("\nEnabling uart interrupt for this interrupt id%d",interrupt);
	uart_instance[uart_config->uart_num]->INTR_EN = interrupt ; //The total interrupt register
	//This enables UART hardware interrupts by writing the desired interrupt mask to the UART's interrupt enable register
	//Interrupt enable registers 
	return SUCCESS;
}

uint8_t UART_Init(UART_Config_t const *uart_config)
{
	// //my comedy implementation :)
	// 	CircularBuffer c1;
	// 	uint8_t array[128]={0};
	// 	for(int i=0;i<3;i++){
    //     for (int j = 0; j < BUFFER_SIZE; j++) {
    //         c1.buffer[j] = array[j];
    //     }

    //     // OR, if you're testing something in the loop:
    //     c1.head = 0;
    //     c1.tail = 0;
	// 	uart_buffers[i]=c1;
	// 	}
	//printf("wtf");
	log_info("\nInitializing uart instance %d",uart_config->uart_num);
	
  	switch (uart_config->uart_num)
	{	
		case 0:
		    uart_instance[uart_config->uart_num] = UART0;
			//A defined uart type
			break;
		case 1:
			uart_instance[uart_config->uart_num] = UART1;
			//A defined uart type
			break;
		case 2:
			uart_instance[uart_config->uart_num] = UART2;
			//A defined uart type
			break;
		default:
			log_error("\nInvalid UART instance %d. This SoC supports only UART-0 to UART-2, and UART-0 is already in use for printing ", uart_config->uart_num);
       		return ENOINST;
			break;
	}
				
	log_debug("\nUART%d Initialized..", uart_config->uart_num);
	
   if((uart_config->uart_num>0) && (uart_config->uart_num <= 2))
	{
   		UART_Config(uart_config);
		UART_Set_Baudrate(uart_config);
   		if (uart_config->buffer_enable==1) //If the user requested software buffering:
   		{
			//Associate interrupt IDs with handler functions (ISR = Interrupt Service Routine).
			//Set priorities and thresholds to control how and when interrupts are serviced.
   		 	log_debug("\nEnabling UART Buffer ");
			PLIC_Config_t plic_config;
			plic_config.interrupt_id= UART0_IRQn + uart_config->uart_num;
			plic_config.fptr = UART_Receive; //wot?
			interrupt_arg[UART0_IRQn + uart_config->uart_num]=(void *)uart_config;
			plic_config.priority_value=PLIC_PRIORITY_3;
			IRQ_Connect(&plic_config);
			UART_Interrupt_Enable(uart_config,ENABLE_RX_NOT_EMPTY);
			buffer_init(uart_config);
			/*PLIC_Config_t plic_config;  ---> Plic configutration object
			plic_config.interrupt_id = UART0_IRQn + uart_config->uart_num;  ---->To get the corresponding interrupt value of the interupt
			plic_config.fptr = UART_Receive; //The function to be triggered on the successful completion of the uart
			plic_config.priority_value = PLIC_PRIORITY_3; 
   			IRQ_Connect(&plic_config); //Craeting of the uart interrupt with the priority 
			*/
   		}
	}
		log_debug("\nUART%d Initialized..", uart_config->uart_num);
		

	return SUCCESS;
  }


#undef getchar
int getchar()
{
	//only applicable for uart0 --> for debug purposes
	while((((UART_Type*)(0x11300))->STATUS_REG & STS_RX_NOT_EMPTY) == 0);
	//If rx is not empty
	return (((UART_Type*)(0x11300))->RX_REG.data_8);
}

int putchar(int ch)
{
	
	while(((UART_Type*)(0x11300))->STATUS_REG & STS_TX_FULL); //If tx is full ---> Wait here

	((UART_Type*)(0x11300))->TX_REG.data_8 = ch;//Getting one character input

	return SUCCESS;
}
uint8_t UART_Set_Baudrate(UART_Config_t *uart_config)
{ 
	
	log_info("\nInitializing baudrate as %d to uart instance %d",uart_config->baudrate, uart_config->uart_num);
	float baud_count = 0.0;
	unsigned short baud_value = 0;
	//baud count value is the value stored in registers
    baud_count = CLOCK_FREQUENCY_BASE  / (16.0 * uart_config->baudrate); //--->upto 2^32
	if( (baud_count - (int) baud_count) > 0.5 ) //that approximate decimal value 0 -2^16
		baud_value = (((short) baud_count) + 1) ;
	else
		baud_value =  (short) baud_count;

	uart_instance[uart_config->uart_num]->BAUD_REG = (short) baud_value; //already defined short but ok
	//2 bytes value is stored here
	return SUCCESS;
}

void UART_Set_Baudrate_Sim(UART_Config_t *uart_config)
//For simulation only
{
	float baud_count = 0.0;
    unsigned short baud_value = 0;
	baud_count = CLOCK_FREQUENCY_BASE  / (16.0 * uart_config->baudrate); //clock frequency sim is changed here
	if( (baud_count - (int) baud_count) > 0.5 )
		baud_value = (((short) baud_count) + 1) ;
	else
		baud_value =  (short) baud_count;

	uart_instance[uart_config->uart_num]->BAUD_REG = (short) baud_value; //2 byte values is stored here
}

uint8_t UART_Config( UART_Config_t *uart_config)
{
	//To set up the control register
	uart_instance[uart_config->uart_num]->CTRL = (STOP_BITS(uart_config->stop_bits) | PARITY(uart_config->parity) | UART_TX_RX_LEN(uart_config->char_size));
	//AS these bits need to be set continuously
	uart_instance[uart_config->uart_num]->DELAY_REG = uart_config->delay; //2 bytes
	return SUCCESS;
}



uint8_t UART_RX_Threshold(UART_Config_t *uart_config, unsigned char rxthreshold)
{
	uart_instance[uart_config->uart_num]->RX_THRESHOLD = rxthreshold;
	//8bit value of RX threshold --so unsigned char
	return SUCCESS;
}


uint8_t UART_Write(UART_Config_t *uart_config , uint8_t * string_data, int uart_len)
{
	log_info("\nTransmitting data via uart instance %d",uart_config->uart_num);
	uint32_t i = 0;
	uint8_t temp;

	do
	{
		temp = string_data[i++]; //Data continuous input
	    while(uart_instance[uart_config->uart_num]->STATUS_REG & STS_TX_FULL); //Wait till tx is not full
	    uart_instance[uart_config->uart_num]->TX_REG.data_8 = temp;
	} while(--uart_len!= 0);//getting in data as characters till the length of uart buffer

	while((uart_instance[uart_config->uart_num]->STATUS_REG & STS_TX_EMPTY)==0); //When Tx isnt empty
  return SUCCESS;
}

uint8_t UART_Buffer_Read_Character(UART_Config_t *uart_config, uint8_t *character_data) {
	log_info("\nReading a character from software buffer of instance %d",uart_config->uart_num);	
    if (!buffer_is_empty(&uart_buffers[uart_config->uart_num])) {
		log_debug("\nBuffer is not empty");
		//DEfreferencing the pointer to data
        *character_data = uart_buffers[uart_config->uart_num].buffer[uart_buffers[uart_config->uart_num].tail];  // Read the data from the buffer
		//After selecting the correct buffer CB
		//Tail → points to the index of the oldest unread byte.
        uart_buffers[uart_config->uart_num].tail = (uart_buffers[uart_config->uart_num].tail + 1) % BUFFER_SIZE;  // Move the tail forward
        return SUCCESS;  // Data successfully read
    } else {
		log_debug("\nBuffer is empty");
        return ECBUFEMPTY;  // Buffer is empty
    }
}

uint8_t UART_Buffer_Read_String(UART_Config_t *uart_config, uint8_t *string_data) {
	log_info("\nReading the string from software buffer of instance %d",uart_config->uart_num);	
	int i=0;
	int a;
	while(1)
	{
		char temp;
		a=UART_Buffer_Read_Character(uart_config,&temp);
		if(a==ECBUFEMPTY) //Error circular buffer empty
		{
			if(i!=0)
			{
				log_debug("\nData has been stored successfully");
				string_data[i++] = 0x00;
				
				return SUCCESS;
			}
			else
			{
				// log_error("\nBuffer is empty ");
				return ECBUFEMPTY;
			}
			
		}
		string_data[i++] = temp;
	}
}

uint8_t UART_Flush(UART_Config_t *uart_config)
{
	//The __attribute__((unused)) suppresses compiler warnings since temp is not actually used.
	__attribute__((unused)) char temp;
	while ((uart_instance[uart_config->uart_num]->STATUS_REG & STS_RX_NOT_EMPTY) != 0)
	//Checks if any data is pending in the Rx Fifo
	{
		temp = uart_instance[uart_config->uart_num]->RX_REG.data_8 ; //defining the type of data we want to use
	}
	return SUCCESS;
}
uint8_t UART_Read_Character(UART_Config_t *uart_config , uint8_t * character_data)
/*Polls the UART hardware STATUS_REG until data is ready (STS_RX_NOT_EMPTY).
Reads one byte directly from the hardware RX register into *character_data.
Logs success and returns SUCCESS.*/
{
	log_info("\nReading a character from Rx buffer of instance %d",uart_config->uart_num);	
	//Used to have subdivinsunons like INFO or as DEBUG messages
    uint8_t temp = 0;
	while ((uart_instance[uart_config->uart_num]->STATUS_REG & STS_RX_NOT_EMPTY) ==0 );
	//To check if it is empty
	*character_data = uart_instance[uart_config->uart_num]->RX_REG.data_8; //assigning the value to the passed argument pointer
	log_debug("\nSuccessfully read a character from software buffer of instance %d",uart_config->uart_num);	 //uart number is the instance number
	return SUCCESS;
}

uint8_t UART_Read_String(UART_Config_t *uart_config , char * string_data)
{
	log_info("\nReading the string from software buffer of instance %d",uart_config->uart_num);	
	uint32_t i = 0;
	uint8_t temp = 0;
	while(1)
	{

		temp = uart_instance[uart_config->uart_num]->RX_REG.data_8;
		//Holds single byte from uart
		string_data[i++] = temp;
		// putchar(temp);
		//Stores them into the string_data buffer until a carriage return (0x0D) is received.
		//Appends a null terminator (0x00) at the end to make it a valid C string.

		if (temp == 0x0D)
		{
		if(i!=0)
			{
				log_debug("\nData has been stored successfully");
				string_data[i++] = 0x00;
				//Next vaue is null caharacter
				return SUCCESS;
			}
			else
			{
				log_error("\nBuffer is empty ");
				return EBUFEMPTY;
			}
		}
}
}
uint8_t UART_Write_Wait(UART_Config_t *uart_config)
{
	while((uart_instance[uart_config->uart_num]->STATUS_REG &STS_TX_EMPTY) ==0);
	return SUCCESS;
}

//No uart read
#endif