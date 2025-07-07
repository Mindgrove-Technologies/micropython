/***************************************************************************
* Project                     : Mindgrove Silicon's Secure-IoT SoC
* Name of the file	      	  : clint_driver.c
* Brief Description of file   : source file for CLINT.
* Name of Author    	      : Sathya Narayanan N & Kapiil Shyam. M
* Email ID                    : sathya281@gmail.com & kapil@mindgrovetech.in

 Copyright (C) 2019  IIT Madras. All rights reserved.
 Copyright (C) 2024  Mindgrove Technologies Pvt Ltd. All rights reserved.

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
***************************************************************************/
/**
@file clint_driver.c
@brief source file for clint.
@details This file is a driver file for clint. The file contains the clint 
interrupt handler, configure the counter and support for e and c class clint timers. 
*/

#include "clint_driver.h"
#include "log.h"
#include "secure_iot.h"
#include "defines.h"
#include "io.h"

// Pointer to the MSIP register
// volatile uint64_t* msip = (volatile uint64_t*)CLINT0_BASE;

// // Pointer to the MTIME register
// volatile uint64_t* mtime    =  (volatile uint64_t*)MTIME;

// // Pointer to the MTIMECMP register
// volatile uint64_t* mtimecmp = (volatile uint64_t*)MTIMECMP;
#include "utils.h"
//extern volatile uint64_t* MTIME;
/** @fn static unsigned long mtime_low( )
 * @brief return the lower 32bit of mtime.
 * @details return the lower half of mtime. And this is needed mostly in dealing mtime in 32 bit machines.
 * @return unsigned long
 */
static uint32_t mtime_low(void)
{
  return *(uint32_t *)(MTIME);
}

/** @fn static uint32_t mtime_high(void)
 * @brief return the upper 32 bit of mtime
 * @details return the upper 32 bit of mtime register. This is very useful incase of 32 bit core.
 *          Incase of 64 bit core this has to be appended with lower 32 bits adn sent.
 * @return unsigned 32bit int
 */
static uint32_t mtime_high(void)
{
  return *(volatile uint32_t *)(MTIME + 4);
}

/** @fn uint64_t Get_MTIME()
 * @brief return the mtime value for a 32 bit or 64 bit machine
 * @details return the mtime value based on the __riscv_xlen. Incase of 64 bit, this joins the upper
 *          and lower 32 bits of mtime and return
 * @return unsigned 64bit int
 */
uint64_t Get_MTIME()
{

#if __riscv_xlen == 64
   return ( ((uint64_t)mtime_high() << 32) | mtime_low());
#else
  return mtime_low();
#endif
}

/** @fn void Config_Counter( uint64_t value)
 * @brief Sets up the CLINT timer
 * @details Sets the mtimecmp to current mtime + delta
 * @param unsigned 64bit int (delta value after which interrupt happens)
 */
void Config_Counter( uint64_t value)
{
	log_trace("\nConfig_Counter entered\n");

	*mtimecmp = *mtime + value;

	log_debug("mtimecmp value = %d\n", *mtimecmp);
	log_debug("mtime value = %d\n", *mtime);

	log_trace("\nConfig_Counter exited\n");
}

/**
 * @brief Configures the CLINT timer and triggers a software interrupt.
 * 
 * @details This function performs the following operations:
 * 
 * 1. Triggers a software interrupt by writing to the MSIP (Machine Software Interrupt Pending) register.
 * 2. Disables specific interrupts by clearing bits in the MIE (Machine Interrupt Enable) register.
 * 3. Enables global interrupts by setting the relevant bits in the MSTATUS (Machine Status) register.
 * 4. Configures the timer with the provided value using the `Config_Counter` function.
 * 
 * @param timer_value The value to configure the timer.
 * 
 * @note 
 * - The function assumes that the `msip` pointer is correctly initialized and points to the MSIP register.
 * - The specific bits disabled in the MIE register and the bit set in the MSTATUS register are
 *   hardcoded and might need adjustment based on the system's interrupt configuration and requirements.
 * 
 * @see Config_Counter
 */
void CLINT_Timer(uint64_t timer_value) {
    // Set MSIP register to trigger a software interrupt
    *msip = 0x01;

    // Disable specific interrupts (for demonstration; adjust as needed)
    asm volatile("li      t0, 0xc0\t\n"    // Load immediate value
                 "csrrs   zero, mie, t0\t\n"); // Set bits in mie register

    // Enable global interrupts
    asm volatile("li      t0, 8\t\n"      // Load immediate value
                 "csrrs   zero, mstatus, t0\t\n"); // Set bits in mstatus register

    // Configure the timer
    Config_Counter(timer_value);
}

/** @fn void CLINT_Handler(uintptr_t int_id, uintptr_t epc)
 * @brief Handler for Machine Timer Interrupt
 * @details Handler for Machine Timer Interrupt. This handles the timer interrupt and sets mtimecmp to clear timer interrupt.
 * @param unsigned int ptr int_id
 * @param unsigned int ptr epc
 */
//Made it weak to prevent the error --> Kishore anna changed
// __attribute__ ((weak)) void CLINT_Handler( __attribute__((unused)) uintptr_t int_id,  __attribute__((unused)) uintptr_t epc)
// {
// 	log_trace("\nCLINT_Handler entered\n");

// 	*mtimecmp = -1;

//   printf("\nTimer Handled!");
// 	log_trace("CLINT_Handler exited\n");
// }
