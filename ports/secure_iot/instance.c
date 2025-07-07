#include"uart.h"
#include"io.h"
#include"secure_iot.h"
#include"gptimer.h"
#include"plic.h"
#include "log.h"
size_t my_strlen(const char *str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}
void main()
{
    UART_Config_t uart; /*Structure variable which hold the UART's specifications */
    UART_Config_t uart_config=&uart; /*Creating a pointer to the above stucture variable/
    uart_config->uart_num=UART_1; /Specifies the UART instance/
    uart_config->baudrate=115200; /Specifies the baudrate/
    uart_config->char_size=8; /Specifies the Char size/ //5,6,7,8 -->
    uart_config->delay=0;/Specifies the delay/
    uart_config->parity=0;/Specifies the parity/
    uart_config->stop_bits=STOP_BIT_1;/Specifies the stopbits/
    uart_config->buffer_enable=UART_BUILTIN_BUFF;/Disable the circular buffer/
    UART_Init(uart_config);/* Initializing the particular uart instance */
    printf("\ncode started !\n");
    char str[10] = "MINDGROVE";/*Declared a string and which is going to transmitted */
    // Calling strlen with Str as argument
    int len = my_strlen(str);/Finding the length of the string/
    char new[100];/*Declared a string variable to receive */
    UART_Write(uart_config,str,len);/Transmitting the data via the specified UART instance/
    UART_Write_Wait(uart_config);//Wait till the uart transmission.
    UART_Read_String(uart_config,new);/Read all the data in default built-in  buffer/
    printf("\nstring1 :%s\n", new);
    
}