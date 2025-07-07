/**
 * Project                           : Secure IoT SoC
 * Name of the file                  : gpio_driver.c
 * Brief Description of file         : This is a Baremetal GPIO Driver file for Mindgrove Silicon's GPIO Peripheral.
 * Name of Author                    : Kapil Shyam. M, Shri Mahaalakshmi S J
 * Email ID                          : kapilshyamm@gmail.com, mahaalakshmi@mindgrovetech.in
 *
 * @file gpio_driver.c
 * @author Kapil Shyam. M (kapilshyamm@gmail.com)
 * @author Shri Mahaalakshmi S J (mahaalakshmi@mindgrovetech.in)
 * @brief This is a Baremetal GPIO Driver file for Mindgrove Silicon's GPIO Peripheral.
 * @version 0.2
 * @date 2024-09-23
 *
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2024. All rights reserved.
 *
 */

#include "gpio.h"
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include "secure_iot.h"
#include "pinmux.h"
#include "log.h"
#include "errors.h"

// control reg macros
#define BUFFER_4_8_DATA_SIZE(x)   (x<<21)
#define BUFFER_8_DATA_SIZE(x)     (x<<19)
#define BUFFER_4_DATA_SIZE(x)     (x<<17)
#define BUFFER_2_DATA_SIZE(x)     (x<<15)
#define BUFFER_8_FLUSH            (1<<14)
#define BUFFER_4_FLUSH    	      (1<<13)
#define BUFFER_2_FLUSH            (1<<12)
#define BUFFER_8_DIR(x)           (x<<11)
#define BUFFER_4_DIR(x)           (x<<10)
#define BUFFER_2_DIR(x)           (x<<9)
#define BUFFER_8_CLK_EDGE_SEL(x)  (x<<8)
#define BUFFER_4_CLK_EDGE_SEL(x)  (x<<7)
#define BUFFER_2_CLK_EDGE_SEL(x)  (x<<6)
#define BUFFER_8_CLK_SEL(x)       (x<<5)
#define BUFFER_4_CLK_SEL(x)       (x<<4)
#define BUFFER_2_CLK_SEL(x)       (x<<3)
#define BUFFER_8_ENABLE           (1<<2)
#define BUFFER_4_ENABLE           (1<<1)
#define BUFFER_2_ENABLE           (1<<0)

// status reg macros
#define BUFFER_4_8_OUTP_READY     (1<<13)
#define BUFFER_4_8_CAN_TAKE_INP   (1<<12)
#define BUFFER_8_OUTP_READY       (1<<11)
#define BUFFER_8_CAN_TAKE_INP     (1<<10)
#define BUFFER_4_OUTP_READY       (1<<9)
#define BUFFER_4_CAN_TAKE_INP     (1<<8)
#define BUFFER_2_OUTP_READY       (1<<7)
#define BUFFER_2_CAN_TAKE_INP     (1<<6)
#define BUFFER_8_NOT_EMPTY	      (1<<5)
#define BUFFER_8_NOT_FULL         (1<<4)
#define BUFFER_4_NOT_EMPTY	      (1<<3)
#define BUFFER_4_NOT_FULL         (1<<2)
#define BUFFER_2_NOT_EMPTY	      (1<<1)
#define BUFFER_2_NOT_FULL         (1<<0)

#define GPIO_REG ((GPIO_Type*)(GPIO_BASE))
#define GPIO_PINMUX_REG ((GPIO_PINMUX_Type*)(GPIO_PINMUX_BASE))

uint8_t GPIO_Config(bool direction, uint8_t no_of_pins, ...)
{
    uint8_t gpio_pins[45];

    va_list valist;
    va_start(valist, no_of_pins);
    for (uint8_t i = 0; i < no_of_pins; i++)
    {
        gpio_pins[i] = va_arg(valist, int);
    }
    va_end(valist);

    for (uint8_t i = 0; i < no_of_pins; i++)
    {
        if (gpio_pins[i] < 32)
        {
            if ((gpio_pins[i] >= 0 && gpio_pins[i] <= 7) || (gpio_pins[i] >= 17 && gpio_pins[i] <= 22))
            {
                if (((unsigned int *)(PINMUX0_BASE)) + gpio_pins[i] == 1)
                {
                    log_error("These pins %d are configured for PWM", gpio_pins[i]);
                    return EPERM;
                }
            }
            else if(gpio_pins[i] == 8 || gpio_pins[i] == 9 || gpio_pins[i] == 11 || gpio_pins[i] == 15)
            {
                if (((unsigned int *)(PINMUX0_BASE)) + gpio_pins[i] == 0)
                {
                    log_error("These pins %d are configured for UART", gpio_pins[i]);
                    return EPERM;
                }
            }
            GPIO_REG->GPIO_DIRECTION &= ~(1 << (gpio_pins[i]));
            GPIO_REG->GPIO_DIRECTION |= ((uint8_t)(direction) << gpio_pins[i]);
        }

        else if(gpio_pins[i] >= 32 && gpio_pins[i] <= 44)
        {
            if(gpio_pins[i] >= 32 && gpio_pins[i] <=37)
            {
                if (((unsigned int *)(PINMUX0_BASE)) + gpio_pins[i] == 1)
                {
                    log_error("These pins %d are configured for SPI", gpio_pins[i]);
                    return EPERM;
                }
            }
            else if(gpio_pins[i] >= 38 && gpio_pins[i] <= 41)
            {
                if (((unsigned int *)(PINMUX0_BASE)) + gpio_pins[i] == 0)
                {
                    log_error("These pins %d are configured for GPTIMER", gpio_pins[i]);
                    return EPERM;
                }
            }
            else if(gpio_pins[i] >= 42 && gpio_pins[i] <= 44)
            {
                if (((unsigned int *)(PINMUX0_BASE)) + gpio_pins[i] == 0)
                {
                    log_error("These pins %d are configured for JTAG", gpio_pins[i]);
                    return EPERM;
                }
            }
            GPIO_PINMUX_REG->GPIO_DIRECTION &= ~(1 << (gpio_pins[i]));
            GPIO_PINMUX_REG->GPIO_DIRECTION |= ((uint8_t)(direction) << gpio_pins[i]);
        }

        else
        {
            log_error("Invalid pins, Enter between 0 to 44\n");
            return ENODEV;
        }
    }
    
    return SUCCESS;
}

uint8_t GPIO_Pin_Set(uint8_t no_of_pins, ...)
{
    uint8_t gpio_pins[42];

    va_list valist;
    va_start(valist, no_of_pins);
    for (uint8_t i = 0; i < no_of_pins; i++)
    {
        gpio_pins[i] = va_arg(valist, int);
    }
    va_end(valist);

    for (uint8_t i = 0; i < no_of_pins; i++)
    {
        if (gpio_pins[i] < 32)
        {
            GPIO_REG->GPIO_SET = (1 << gpio_pins[i]);
        }
      
        else if (gpio_pins[i] >= 32 && gpio_pins[i] <= 41)
        {
            GPIO_PINMUX_REG->GPIO_SET = (1 << gpio_pins[i]);
        }
      
        else
        {
            log_error("Invalid pins, Enter between 0 to 41\n");
            return ENODEV;
        }
    }
    
    return SUCCESS;
}

uint8_t GPIO_Pin_Clear(uint8_t no_of_pins, ...)
{
    uint8_t gpio_pins[45];

    va_list valist;
    va_start(valist, no_of_pins);
    for (uint8_t i = 0; i < no_of_pins; i++)
    {
        gpio_pins[i] = va_arg(valist, int);
    }
    va_end(valist);

    for (uint8_t i = 0; i < no_of_pins; i++)
    {
        if (gpio_pins[i] < 32)
        {
            GPIO_REG->GPIO_CLEAR = (1 << gpio_pins[i]);
        }
      
        else if (gpio_pins[i] >= 32 && gpio_pins[i] <= 44)
        {
            GPIO_PINMUX_REG->GPIO_CLEAR = (1 << gpio_pins[i]);
        }
      
        else
        {
            log_error("Invalid pins, Enter between 0 to 41\n");
            return ENODEV;
        }
    }
    
    return SUCCESS;
}

uint8_t GPIO_Pin_Toggle(uint8_t no_of_pins, ...)
{
    uint8_t gpio_pins[45];

    va_list valist;
    va_start(valist, no_of_pins);
    for (uint8_t i = 0; i < no_of_pins; i++)
    {
        gpio_pins[i] = va_arg(valist, int);
    }
    va_end(valist);

    for (uint8_t i = 0; i < no_of_pins; i++)
    {
        if (gpio_pins[i] < 32)
        {
            GPIO_REG->GPIO_TOGGLE = (1 << gpio_pins[i]);
        }
      
        else if (gpio_pins[i] >= 32 && gpio_pins[i] <= 44)
        {
            GPIO_PINMUX_REG->GPIO_TOGGLE = (1 << gpio_pins[i]);
        }
      
        else
        {
            log_error("Invalid pins, Enter between 0 to 42\n");
            return ENODEV;
        }
    }
    
    return SUCCESS;
}

uint8_t GPIO_Interrupt_Config(uint8_t gpio_pin, uint8_t low_ena)
{
    if (gpio_pin < 32)
    {
        if (low_ena)
            GPIO_REG->GPIO_INTR |= (0 << gpio_pin);
        else
            GPIO_REG->GPIO_INTR |= (1 << gpio_pin);
    }
    else if (gpio_pin >= 32 && gpio_pin <= 44)
    {
        if (low_ena)
            GPIO_PINMUX_REG->GPIO_INTR |= (0 << gpio_pin);
        else
            GPIO_PINMUX_REG->GPIO_INTR |= (1 << gpio_pin);
    }
    else
    {
        log_error("Invalid pins, Enter between 0 to 44\n");
        return ENODEV;
    }

    return SUCCESS;
}

uint32_t GPIO_Read_Data(uint32_t gpio_pin)
{
    uint32_t read_data;
    
    if (gpio_pin < 32)
    {
        log_debug("Data register : %x\n", (GPIO_REG->GPIO_DATA));
        read_data = GPIO_REG->GPIO_DATA;
    }

    else if(gpio_pin >= 32 && gpio_pin <= 44)
    {
        log_debug("Data register : %x\n", (GPIO_PINMUX_REG->GPIO_DATA));
        read_data = GPIO_PINMUX_REG->GPIO_DATA;
    }
    
    else
    {
        log_debug("Invalid pins, Enter between 0 to 44\n");
    }

    return read_data;
}

uint32_t GPIO_Read_Pin_Status(uint32_t gpio_pin)
{
    uint32_t pin_status;

    if (gpio_pin < 32)
    {
        pin_status = (GPIO_REG->GPIO_DATA) & (1 << gpio_pin);
    }
    
    else if (gpio_pin >= 32 && gpio_pin <= 44)
    {
        pin_status = (GPIO_PINMUX_REG->GPIO_DATA) & (1 << gpio_pin);
    }
    
    else
    {
        log_debug("Invalid pins, Enter between 0 to 44\n");
    }

    return (pin_status != 0) ? 1 : 0;

}

uint32_t GPIO_Write_Data(uint32_t gpio_pin, uint32_t data_word)
{
    if (gpio_pin < 32)
    {
        GPIO_REG->GPIO_DATA = data_word;
    }
    
    else if (gpio_pin >= 32 && gpio_pin <= 44)
    {
        GPIO_PINMUX_REG->GPIO_DATA = data_word;
    }
    
    else
    {
        log_debug("Invalid pin, Enter between 0 to 44\n");
    }

    return SUCCESS;
}

// buffer functions
void buffer_2_enable() {

    GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_2_ENABLE;
}

void buffer_4_enable() {

    GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_4_ENABLE;
}

void buffer_8_enable() {

    GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_8_ENABLE;
}

void buffer_4_8_enable() {

    GPIO_REG->GPIO_BUFFER_CONTROL |= (BUFFER_4_ENABLE | BUFFER_8_ENABLE);
}

void buffer_enable(GPIO_Struct_t *gpio_buf_config)
{
    switch (gpio_buf_config->buffer_num)
    {
    case 2:
        GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_2_ENABLE;
        break;
    case 4:
        GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_4_ENABLE;
        break;
    case 8:
        GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_8_ENABLE;
        break;
    case 12:
        GPIO_REG->GPIO_BUFFER_CONTROL |= (BUFFER_4_ENABLE | BUFFER_8_ENABLE);
        break;
    default:
        log_error("Invalid buffer number, please enter the following number: 2, 4, 8, 12.\n");
        break;
    }
}

void wr_buffer_2_clk_prescaler(uint32_t prescale) {
    GPIO_REG->GPIO_BUFFER_2_CLOCK_PRESCALAR = prescale;
}

void wr_buffer_4_clk_prescaler(uint32_t prescale) {
    GPIO_REG->GPIO_BUFFER_4_CLOCK_PRESCALAR = prescale;
}

void wr_buffer_8_clk_prescaler(uint32_t prescale) {
    GPIO_REG->GPIO_BUFFER_8_CLOCK_PRESCALAR = prescale;
}

void wr_buffer_4_8_clk_prescaler(uint32_t prescale) {
    GPIO_REG->GPIO_BUFFER_4_8_CLOCK_PRESCALAR = prescale;
}

void wr_buffer_clk_prescaler(GPIO_Struct_t *gpio_buf_config)
{
    switch (gpio_buf_config->buffer_num)
    {
    case 2:
        GPIO_REG->GPIO_BUFFER_2_CLOCK_PRESCALAR = gpio_buf_config->prescale;
        break;
    case 4:
        GPIO_REG->GPIO_BUFFER_4_CLOCK_PRESCALAR = gpio_buf_config->prescale;
        break;
    case 8:
        GPIO_REG->GPIO_BUFFER_8_CLOCK_PRESCALAR = gpio_buf_config->prescale;
        break;
    case 12:
        GPIO_REG->GPIO_BUFFER_4_8_CLOCK_PRESCALAR = gpio_buf_config->prescale;
        break;
    default:
        log_error("Invalid buffer number, please enter the following numbers: 2, 4, 8, 12.");
        break;
    }
}

void GPIO_config_buffer_2(GPIO_Struct_t *gpio_buf_config) 
{
    uint32_t buff_ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t val = (BUFFER_2_DATA_SIZE((gpio_buf_config->data_size & 0x03)) | BUFFER_2_DIR((gpio_buf_config->direction & 1)) | \
		    BUFFER_2_CLK_EDGE_SEL((gpio_buf_config->clk_edge_sel & 1)) | BUFFER_2_CLK_SEL((gpio_buf_config->clk_sel & 1)));

    uint32_t mask = ~(BUFFER_2_DATA_SIZE((0x03)) | BUFFER_2_DIR(1) | \
		    BUFFER_2_CLK_EDGE_SEL((1)) | BUFFER_2_CLK_SEL((1)));

    if (gpio_buf_config->direction == 1 && gpio_buf_config->clk_sel == 1)
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | val;
    else
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | (val | BUFFER_2_ENABLE);
}


void GPIO_config_buffer_4(GPIO_Struct_t *gpio_buf_config) 
{
    uint32_t buff_ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t val = (BUFFER_4_DATA_SIZE((gpio_buf_config->data_size & 0x03)) | BUFFER_4_DIR((gpio_buf_config->direction & 1)) | \
		    BUFFER_4_CLK_EDGE_SEL((gpio_buf_config->clk_edge_sel & 1)) | BUFFER_4_CLK_SEL((gpio_buf_config->clk_sel & 1)));

    uint32_t mask = ~(BUFFER_4_DATA_SIZE((0x03)) | BUFFER_4_DIR(1) | \
		    BUFFER_4_CLK_EDGE_SEL((1)) | BUFFER_4_CLK_SEL((1)));

    if (gpio_buf_config->direction == 1 && gpio_buf_config->clk_sel == 1)
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | val;
    else
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | (val | BUFFER_4_ENABLE);
}


void GPIO_config_buffer_8(GPIO_Struct_t *gpio_buf_config) {
    uint32_t buff_ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t val = (BUFFER_8_DATA_SIZE((gpio_buf_config->data_size & 0x03)) | BUFFER_8_DIR((gpio_buf_config->direction & 1)) | \
		    BUFFER_8_CLK_EDGE_SEL((gpio_buf_config->clk_edge_sel & 1)) | BUFFER_8_CLK_SEL((gpio_buf_config->clk_sel & 1)));

    uint32_t mask = ~(BUFFER_8_DATA_SIZE((0x03)) | BUFFER_8_DIR(1) | \
		    BUFFER_8_CLK_EDGE_SEL((1)) | BUFFER_8_CLK_SEL((1)));

    if (gpio_buf_config->direction == 1 && gpio_buf_config->clk_sel == 1)
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | val;
    else
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | (val | BUFFER_8_ENABLE);
}


void GPIO_config_buffer_4_8(GPIO_Struct_t *gpio_buf_config) {
    uint32_t buff_ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t val = (BUFFER_4_8_DATA_SIZE((gpio_buf_config->data_size & 0x03)) | BUFFER_4_DIR((gpio_buf_config->direction & 1)) | BUFFER_8_DIR((gpio_buf_config->direction & 1)) | \
		    BUFFER_4_CLK_EDGE_SEL((gpio_buf_config->clk_edge_sel & 1)) | BUFFER_8_CLK_EDGE_SEL((gpio_buf_config->clk_edge_sel & 1)) | \
		    BUFFER_4_CLK_SEL((gpio_buf_config->clk_sel & 1)) |  BUFFER_8_CLK_SEL((gpio_buf_config->clk_sel & 1)));

    uint32_t mask = ~(BUFFER_4_8_DATA_SIZE((0x03)) | BUFFER_4_DIR(1) | BUFFER_8_DIR(1) | \
		    BUFFER_4_CLK_EDGE_SEL((1)) | BUFFER_8_CLK_EDGE_SEL((1)) | \
            BUFFER_4_CLK_SEL((1)) |  BUFFER_8_CLK_SEL((1)));

    if (gpio_buf_config->direction == 1 && gpio_buf_config->clk_sel == 1)
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | val;
    else
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | (val | BUFFER_4_ENABLE | BUFFER_8_ENABLE);
}

void config_buffer_2(uint8_t wr_rd_data_size, uint8_t dir, uint8_t clk_edge_sel, uint8_t clk_sel) {
    uint32_t buff_ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t val = (BUFFER_2_DATA_SIZE((wr_rd_data_size & 0x03)) | BUFFER_2_DIR((dir & 1)) | \
		    BUFFER_2_CLK_EDGE_SEL((clk_edge_sel & 1)) | BUFFER_2_CLK_SEL((clk_sel & 1)));

    uint32_t mask = ~(BUFFER_2_DATA_SIZE((0x03)) | BUFFER_2_DIR(1) | \
		    BUFFER_2_CLK_EDGE_SEL((1)) | BUFFER_2_CLK_SEL((1)));

    if (dir == 1 && clk_sel == 1)
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | val;
    else
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | (val | BUFFER_2_ENABLE);
}


void config_buffer_4(uint8_t wr_rd_data_size, uint8_t dir, uint8_t clk_edge_sel, uint8_t clk_sel) {
    uint32_t buff_ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t val = (BUFFER_4_DATA_SIZE((wr_rd_data_size & 0x03)) | BUFFER_4_DIR((dir & 1)) | \
		    BUFFER_4_CLK_EDGE_SEL((clk_edge_sel & 1)) | BUFFER_4_CLK_SEL((clk_sel & 1)));

    uint32_t mask = ~(BUFFER_4_DATA_SIZE((0x03)) | BUFFER_4_DIR(1) | \
		    BUFFER_4_CLK_EDGE_SEL((1)) | BUFFER_4_CLK_SEL((1)));

    if (dir == 1 && clk_sel == 1)
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | val;
    else
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | (val | BUFFER_4_ENABLE);
}


void config_buffer_8(uint8_t wr_rd_data_size, uint8_t dir, uint8_t clk_edge_sel, uint8_t clk_sel) {
    uint32_t buff_ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t val = (BUFFER_8_DATA_SIZE((wr_rd_data_size & 0x03)) | BUFFER_8_DIR((dir & 1)) | \
		    BUFFER_8_CLK_EDGE_SEL((clk_edge_sel & 1)) | BUFFER_8_CLK_SEL((clk_sel & 1)));

    uint32_t mask = ~(BUFFER_8_DATA_SIZE((0x03)) | BUFFER_8_DIR(1) | \
		    BUFFER_8_CLK_EDGE_SEL((1)) | BUFFER_8_CLK_SEL((1)));

    if (dir == 1 && clk_sel == 1)
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | val;
    else
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | (val | BUFFER_8_ENABLE);
}


void config_buffer_4_8(uint8_t wr_rd_data_size, uint8_t dir, uint8_t clk_edge_sel, uint8_t clk_sel) {
    uint32_t buff_ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t val = (BUFFER_4_8_DATA_SIZE((wr_rd_data_size & 0x03)) | BUFFER_4_DIR((dir & 1)) | BUFFER_8_DIR((dir & 1)) | \
		    BUFFER_4_CLK_EDGE_SEL((clk_edge_sel & 1)) | BUFFER_8_CLK_EDGE_SEL((clk_edge_sel & 1)) | \
		    BUFFER_4_CLK_SEL((clk_sel & 1)) |  BUFFER_8_CLK_SEL((clk_sel & 1)));

    uint32_t mask = ~(BUFFER_4_8_DATA_SIZE((0x03)) | BUFFER_4_DIR(1) | BUFFER_8_DIR(1) | \
		    BUFFER_4_CLK_EDGE_SEL((1)) | BUFFER_8_CLK_EDGE_SEL((1)) | \
            BUFFER_4_CLK_SEL((1)) |  BUFFER_8_CLK_SEL((1)));

    if (dir == 1 && clk_sel == 1)
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | val;
    else
        GPIO_REG->GPIO_BUFFER_CONTROL = (buff_ctrl & mask) | (val | BUFFER_4_ENABLE | BUFFER_8_ENABLE);
}


// Buffer 2
void wr_8bits_into_buffer_2(uint8_t data) {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_2_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_2_DATA.data_8 = data;
}

void wr_16bits_into_buffer_2(uint16_t data) {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_2_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_2_DATA.data_16 = data;
}

void wr_32bits_into_buffer_2(uint32_t data) {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_2_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_2_DATA.data_32 = data;
}

void wr_into_buffer_2(GPIO_Struct_t *gpio_buf_config, uint8_t data)
{
    while(!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_2_CAN_TAKE_INP));
    switch (gpio_buf_config->data_size)
    {
    case 1:
        GPIO_REG->GPIO_BUFFER_2_DATA.data_8 = data;
        break;
    case 2:
        GPIO_REG->GPIO_BUFFER_2_DATA.data_16 = data;
        break;
    case 3:
        GPIO_REG->GPIO_BUFFER_2_DATA.data_32 = data;
        break;
    default:
        log_error("Invalid bit selection, please enter the following numbers: 8, 16, 32\n");
        break;
    }
}

uint8_t rd_8bits_from_buffer_2() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_2_OUTP_READY));
    uint8_t rx_data = GPIO_REG->GPIO_BUFFER_2_DATA.data_8;

    return rx_data;
}

uint16_t rd_16bits_from_buffer_2() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_2_OUTP_READY));
    uint16_t rx_data = GPIO_REG->GPIO_BUFFER_2_DATA.data_16;

    return rx_data;
}

uint32_t rd_32bits_from_buffer_2() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_2_OUTP_READY));
    uint32_t rx_data = GPIO_REG->GPIO_BUFFER_2_DATA.data_32;

    return rx_data;
}

uint32_t rd_from_buffer_2(GPIO_Struct_t *gpio_buf_config)
{
    uint32_t rx_data;
    while(!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_2_OUTP_READY));
    switch (gpio_buf_config->data_size)
    {
    case 1:
        rx_data = GPIO_REG->GPIO_BUFFER_2_DATA.data_8;
        break;
    case 2:
        rx_data = GPIO_REG->GPIO_BUFFER_2_DATA.data_16;
        break;
    case 3:
        rx_data = GPIO_REG->GPIO_BUFFER_2_DATA.data_32;
        break;
    default:
        log_error("Invalid bit selection, please enter the following numbers: 8, 16, 32\n");
        break;
    }
    return rx_data;
}

// Buffer 4
void wr_8bits_into_buffer_4(uint8_t data) {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_4_DATA.data_8 = data;
}

void wr_16bits_into_buffer_4(uint16_t data) {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_4_DATA.data_16 = data;
}

void wr_32bits_into_buffer_4(uint32_t data) {
    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_4_DATA.data_32 = data;
}

void wr_into_buffer_4(GPIO_Struct_t *gpio_buf_config, uint8_t data)
{
    while(!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_CAN_TAKE_INP));
    switch (gpio_buf_config->data_size)
    {
    case 1:
        GPIO_REG->GPIO_BUFFER_4_DATA.data_8 = data;
        break;
    case 2:
        GPIO_REG->GPIO_BUFFER_4_DATA.data_16 = data;
        break;
    case 3:
        GPIO_REG->GPIO_BUFFER_4_DATA.data_32 = data;
        break;
    default:
        log_error("Invalid bit selection. please enter the following numbers: 8, 16, 32.\n");
        break;
    }
}

uint8_t rd_8bits_from_buffer_4() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_OUTP_READY));
    uint8_t rx_data = GPIO_REG->GPIO_BUFFER_4_DATA.data_8;

    return rx_data;
}

uint16_t rd_16bits_from_buffer_4() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_OUTP_READY));
    uint16_t rx_data = GPIO_REG->GPIO_BUFFER_4_DATA.data_16;

    return rx_data;
}

uint32_t rd_32bits_from_buffer_4() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_OUTP_READY));
    uint32_t rx_data = GPIO_REG->GPIO_BUFFER_4_DATA.data_32;

    return rx_data;
}

uint32_t rd_from_buffer_4(GPIO_Struct_t *gpio_buf_config)
{
    uint32_t rx_data;
    while(!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_OUTP_READY));
    switch (gpio_buf_config->data_size)
    {
    case 1:
        rx_data = GPIO_REG->GPIO_BUFFER_4_DATA.data_8;
        break;
    case 2:
        rx_data = GPIO_REG->GPIO_BUFFER_4_DATA.data_16;
        break;
    case 3:
        rx_data = GPIO_REG->GPIO_BUFFER_4_DATA.data_32;
        break;
    default:
        log_error("Invalid bit selection, please enter the following numbers: 8, 16, 32.\n");
        break;
    }
    return rx_data;
}

// Buffer 8
void wr_8bits_into_buffer_8(uint8_t data) {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_8_DATA.data_8 = data;
}

void wr_16bits_into_buffer_8(uint16_t data) {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_8_DATA.data_16 = data;
}

void wr_32bits_into_buffer_8(uint32_t data) {
    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_8_DATA.data_32 = data;
}

void wr_into_buffer_8(GPIO_Struct_t *gpio_buf_config, uint32_t data)
{
    while(!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_CAN_TAKE_INP));
    switch (gpio_buf_config->data_size)
    {
    case 1:
        GPIO_REG->GPIO_BUFFER_8_DATA.data_8 = data;
        break;
    case 2:
        GPIO_REG->GPIO_BUFFER_8_DATA.data_16 = data;
        break;
    case 3:
        GPIO_REG->GPIO_BUFFER_8_DATA.data_32 = data;
        break;
    default:
        log_error("Invalid bit selection, please enter the following numbers: 8, 16, 32.\n");
        break;
    }
}

uint8_t rd_8bits_from_buffer_8() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_OUTP_READY));
    uint8_t rx_data = GPIO_REG->GPIO_BUFFER_8_DATA.data_8;

    return rx_data;
}

uint16_t rd_16bits_from_buffer_8() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_OUTP_READY));
    uint16_t rx_data = GPIO_REG->GPIO_BUFFER_8_DATA.data_16;

    return rx_data;
}

uint32_t rd_32bits_from_buffer_8() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_OUTP_READY));
    uint32_t rx_data = GPIO_REG->GPIO_BUFFER_8_DATA.data_32;

    return rx_data;
}

uint32_t rd_from_buffer_8(GPIO_Struct_t *gpio_buf_config)
{
    uint32_t rx_data;
    while(!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_OUTP_READY));
    switch (gpio_buf_config->data_size)
    {
    case 1:
        rx_data = GPIO_REG->GPIO_BUFFER_8_DATA.data_8;
        break;
    case 2:
        rx_data = GPIO_REG->GPIO_BUFFER_8_DATA.data_16;
        break;
    case 3:
        rx_data = GPIO_REG->GPIO_BUFFER_8_DATA.data_32;
        break;
    default:
        log_error("Invalid bit selection, please enter the following numbers: 8, 16, 32.\n");
        break;
    }
}

// buffer 4_8
void wr_16bits_into_buffer_4_8(uint16_t data) {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_8_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_4_8_DATA.data_16 = data;
}

void wr_32bits_into_buffer_4_8(uint32_t data) {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_8_CAN_TAKE_INP));
    GPIO_REG->GPIO_BUFFER_4_8_DATA.data_32 = data;
}

void wr_into_buffer_4_8(GPIO_Struct_t *gpio_buf_config, uint32_t data)
{
    while(!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_8_CAN_TAKE_INP));
    switch (gpio_buf_config->data_size)
    {
    case 2:
        GPIO_REG->GPIO_BUFFER_4_8_DATA.data_16 = data;
        break;
    case 3:
        GPIO_REG->GPIO_BUFFER_4_8_DATA.data_32 = data;
        break;
    default:
        log_error("Invalid bit selection, please enter the following numbers: 16, 32.\n");
        break;
    }
}

uint16_t rd_16bits_from_buffer_4_8() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_8_OUTP_READY));
    uint16_t rx_data = GPIO_REG->GPIO_BUFFER_4_8_DATA.data_16;

    return rx_data;
}

uint32_t rd_32bits_from_buffer_4_8() {

    while (!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_8_OUTP_READY));
    uint32_t rx_data = GPIO_REG->GPIO_BUFFER_4_8_DATA.data_32;

    return rx_data;
}

uint32_t rd_from_buffer_4_8(GPIO_Struct_t *gpio_buf_config)
{
    uint32_t rx_data;
    while(!(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_8_OUTP_READY));
    switch (gpio_buf_config->data_size)
    {
    case 2:
        rx_data = GPIO_REG->GPIO_BUFFER_4_8_DATA.data_16;
        break;
    case 3:
        rx_data = GPIO_REG->GPIO_BUFFER_4_8_DATA.data_32;
        break;
    default:
        log_error("Invalid bit selection, please enter the following numbers: 16, 32.\n");
        break;
    }
}

void buffer_2_wait_till_tx_completes() {

    while (GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_2_NOT_EMPTY);
}

void buffer_4_wait_till_tx_completes() {

    while (GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_NOT_EMPTY);
}

void buffer_8_wait_till_tx_completes() {

    while (GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_NOT_EMPTY);
}

void buffer_4_8_wait_till_tx_completes() {

    while (GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_NOT_EMPTY);
    while (GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_NOT_EMPTY);
}

void buffer_wait_till_tx_completes(GPIO_Struct_t *gpio_buf_config)
{
    switch (gpio_buf_config->buffer_num)
    {
    case 2:
        while(GPIO_REG-> GPIO_BUFFER_STATUS & BUFFER_2_NOT_EMPTY);
        break;
    case 4:
        while(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_4_NOT_EMPTY);
        break;
    case 8:
        while(GPIO_REG->GPIO_BUFFER_STATUS & BUFFER_8_NOT_EMPTY);
        break;
    case 12:
        while(GPIO_REG->GPIO_BUFFER_STATUS & (BUFFER_4_NOT_EMPTY | BUFFER_8_NOT_EMPTY));
        break;
    default:
        log_error("Invalid buffer number, please enter the following numbers: 2, 4, 8, 12.\n");
        break;
    }
}

void buffer_2_flush() {
    GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_2_FLUSH;
}

void buffer_4_flush() {
    GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_4_FLUSH;
}

void buffer_8_flush() {
    GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_8_FLUSH;
}

void buffer_4_8_flush() {
    GPIO_REG->GPIO_BUFFER_CONTROL |= (BUFFER_4_FLUSH | BUFFER_8_FLUSH);
}

void buffer_flush(GPIO_Struct_t *gpio_buf_config)
{
    switch (gpio_buf_config->buffer_num)
    {
    case 2:
        GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_2_FLUSH;
        break;
    case 4:
        GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_4_FLUSH;
        break;
    case 8:
        GPIO_REG->GPIO_BUFFER_CONTROL |= BUFFER_8_FLUSH;
        break;
    case 12:
        GPIO_REG->GPIO_BUFFER_CONTROL |= (BUFFER_4_FLUSH | BUFFER_8_FLUSH);
        break;
    default:
        log_error("Invalid buffer number, please enter the following numbers: 2, 4, 8, 12.\n");
        break;
    }
}

void buffer_2_disable() {

    uint32_t ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t temp = ~BUFFER_2_ENABLE;
    GPIO_REG->GPIO_BUFFER_CONTROL = ctrl & temp;
}

void buffer_4_disable() {

    uint32_t ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t temp = ~BUFFER_4_ENABLE;
    GPIO_REG->GPIO_BUFFER_CONTROL = ctrl & temp;
}

void buffer_8_disable() {

    uint32_t ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t temp = ~BUFFER_8_ENABLE;
    GPIO_REG->GPIO_BUFFER_CONTROL = ctrl & temp;
}

void buffer_4_8_disable() {

    uint32_t ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    uint32_t temp = ~(BUFFER_4_ENABLE | BUFFER_8_ENABLE);
    GPIO_REG->GPIO_BUFFER_CONTROL = ctrl & temp;
}

void buffer_disable(GPIO_Struct_t *gpio_buf_config)
{
    uint32_t temp;
    uint32_t ctrl = GPIO_REG->GPIO_BUFFER_CONTROL;
    
    switch (gpio_buf_config->buffer_num)
    {
    case 2:
        temp = ~BUFFER_2_ENABLE;
        GPIO_REG->GPIO_BUFFER_CONTROL = ctrl & temp;
        break;
    case 4:
        temp = ~BUFFER_4_ENABLE;
        GPIO_REG->GPIO_BUFFER_CONTROL = ctrl & temp;
        break;
    case 8:
        temp = ~BUFFER_8_ENABLE;
        GPIO_REG->GPIO_BUFFER_CONTROL = ctrl & temp;
        break;
    case 12:
        temp = ~(BUFFER_4_ENABLE | BUFFER_8_ENABLE);
        GPIO_REG->GPIO_BUFFER_CONTROL = ctrl & temp;
        break;
    default:
        log_error("Invalid buffer number, please enter the following numbers: 2, 4, 8, 12.\n");
        break;
    }
}