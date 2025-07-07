#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"

/***************************************************************************
 * Project           		: shakti devt board
 * Name of the file	     	: init.c
 * Brief Description of file    : source file for system initialization.
 * Name of Author    	        : Sathya Narayanan N & Abhinav Ramnath
 * Email ID                     : sathya281@gmail.com

 Copyright (C) 2019  IIT Madras. All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

 ****************************************************************************/
/**
@file init.c
@brief Source file for System Initialization
@details This is the beginning part of a application beginning.
Different sections are initialized. Main function is called. 
Trap handler is initialized. 
*/

#include "traps.h"
#include "plic.h"
#include "clint_driver.h"
#include "log.h"
#include "utils.h"
#include "qspi.h"
#include "encoding.h"
#include "memory_functions.h"
#include "defines.h"
#include "uart.h"
#include "secure_iot.h"

extern void _init_(void);
extern void trap_entry(void);
int main();

/** @fn static inline void Trap_Init()
 * @brief Initialize the trap/interrupt callback routines with user defined handler.
 * @details Assign default handler for trap / interrupt that does not have user defined
 *          callback routines"
 */
static inline void Trap_Init(void)
{
	mcause_interrupt_table[USER_SW_INTERRUPT]        = default_handler;
	mcause_interrupt_table[SUPER_SW_INTERRUPT]       = default_handler;
	mcause_interrupt_table[RESERVED_INTERRUPT0]      = default_handler;
	mcause_interrupt_table[MACH_SW_INTERRUPT]        = default_handler;
	mcause_interrupt_table[USER_TIMER_INTERRUPT]     = default_handler;
	mcause_interrupt_table[SUPER_TIMER_INTERRUPT]    = default_handler;
	mcause_interrupt_table[RESERVED_INTERRUPT1]      = default_handler;
	mcause_interrupt_table[MACH_TIMER_INTERRUPT]     = CLINT_Handler;
	mcause_interrupt_table[USER_EXT_INTERRUPT]       = default_handler;
	mcause_interrupt_table[SUPERVISOR_EXT_INTERRUPT] = default_handler;
	mcause_interrupt_table[RESERVED_INTERRUPT2]      = default_handler;
	mcause_interrupt_table[MACH_EXTERNAL_INTERRUPT]  = PLIC_Handler;
	mcause_interrupt_table[RESERVED_INTERRUPT3]      = default_handler;
	mcause_interrupt_table[RESERVED_INTERRUPT4]      = default_handler;
	mcause_interrupt_table[RESERVED_INTERRUPT5]      = default_handler;
	mcause_interrupt_table[RESERVED_INTERRUPT6]      = default_handler;

	mcause_trap_table[INSTRUCTION_ADDRESS_MISALIGNED] =
		Trap_Handler;
	mcause_trap_table[INSTRUCTION_ACCESS_FAULT] =
		Trap_Handler;
	mcause_trap_table[ILLEGAL_INSTRUCTION] =
		Trap_Handler;
	mcause_trap_table[BREAKPOINT] =
		Trap_Handler;
	mcause_trap_table[LOAD_ADDRESS_MISALIGNED] =
		Trap_Handler;
	mcause_trap_table[LOAD_ACCESS_FAULT] =
		Trap_Handler;
	mcause_trap_table[STORE_AMO_ADDRESS_MISALIGNED] =
		Trap_Handler;
	mcause_trap_table[STORE_AMO_ACCESS_FAULT] =
		Trap_Handler;
	mcause_trap_table[ENVIRONMENT_CALL_FROM_U_MODE] =
		Trap_Handler;
	mcause_trap_table[ENVIRONMENT_CALL_FROM_S_MODE] =
		Trap_Handler;
	mcause_trap_table[RESERVED_TRAP1] =
		Trap_Handler;
	mcause_trap_table[ENVIRONMENT_CALL_FROM_M_MODE] =
		Trap_Handler;
	mcause_trap_table[INSTRUCTION_PAGE_FAULT] =
		Trap_Handler;
	mcause_trap_table[LOAD_PAGE_FAULT] =
		Trap_Handler;
	mcause_trap_table[RESERVED_TRAP2] =
		Trap_Handler;
	mcause_trap_table[STORE_AMO_PAGE_FAULT] =
		Trap_Handler;
}

/** @fn void init(void)
 * @brief initialize the necessary variables for system start
 */
volatile uint64_t CLOCK_FREQUENCY_BASE = CLOCK_FREQUENCY_ASIC;
volatile uint8_t log_level = LOG_LEVEL;
void _init_(void)
{
	log_trace("_init_ Entered\n");
		
	UART_Config_t uart_config;
	uart_config.uart_num=0;
	uart_config.buffer_enable=0;
	UART_Init(&uart_config); // Change this after UART driver changes
	Heap_Init();
	Trap_Init();
	PLIC_Init();
	main();

#ifdef SIMULATE
   asm volatile (
        "uart_end: li t1, 0x11300" "\n\t" //The base address of UART config registers
        "lh a0, 12(t1)" "\n\t"
        "andi a0, a0, 0x1" "\n\t"
        "beqz a0, uart_end" "\n\t"
        "li a0,  0x50000" "\n\t"
        "sw a0,  12(a0)" "\n\t"
        :
        :
        :"a0","t1","cc","memory");
#endif

	log_trace("_init_ Exited\n");

  while(1){
	asm volatile("wfi");
  }
}

#pragma GCC diagnostic pop