/**
 * Project                               : Secure IoT SoC
 * Name of the file                      : plic_driver.c
 * Brief Description of file             : This is a Baremetal UART Driver file for Mindgrove Silicon's UART Peripheral.
 * Name of Author                        : Sathya Narayanan N, Vignesh Kumar. J 
 * Email ID                              : <sathya281@gmail.com>  <vigneshkumar@mindgrovetech.in>
 * 
 * @file plic_driver.c
 * @brief source file for plic driver
 * @details This file contains the driver code for plic device. The functions to
 * setup each plic registers, isr routine and plic interrupt handler are here.
 * @version 1.0
 * @date 2024-10-08
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2024. All rights reserved.
 * 
 */

#include "plic.h"
//#include "ehh.h"

/* Macros */

/* Offsets for different registers in plic */

#define PLIC_PRIORITY_OFFSET            0x0000UL
#define PLIC_PENDING_OFFSET             0x1000UL
#define PLIC_ENABLE_OFFSET              0x2000UL

//#if defined(SOS) 
#define PLIC_THRESHOLD_OFFSET           0x200000UL
#define PLIC_CLAIM_OFFSET               0x200004UL
/*#else
//Only context 0 supported*/
//#define PLIC_THRESHOLD_OFFSET           0x10000UL
//#define PLIC_CLAIM_OFFSET               0x10010UL
//#endif

/* The priority value for each int src can be found at addresses 4 bytes apart
   starting from base address + priority offset */

#define PLIC_PRIORITY_SHIFT_PER_INT  2


/*
   Global interrupt data maintenance structure
 */
plic_fptr_t isr_table[PLIC_MAX_INTERRUPT_SRC];
interrupt_data_t hart0_interrupt_matrix[PLIC_MAX_INTERRUPT_SRC];
void *interrupt_arg[PLIC_MAX_INTERRUPT_SRC];

inline static void INTERRUPT_Complete(uint32_t interrupt_id)
{
	//log_emit(TRACE,"INTERRUPT_Complete entered\n\r");

	
	volatile uint32_t *claim_addr =  (uint32_t *) (PLIC_BASE +
					      PLIC_CLAIM_OFFSET);

	*claim_addr = interrupt_id;
	hart0_interrupt_matrix[interrupt_id].state = SERVICED;
	hart0_interrupt_matrix[interrupt_id].count++;

	//log_emit(DEBUG,"Interrupt ID %d, state changed to %d\n\r", interrupt_id,
		  //hart0_interrupt_matrix[interrupt_id].state);

	/*//log_emit(DEBUG,"Interrupt ID = %x \n\r Reset to default values state = %x \
		  \n\r Priority = %x\n\r Count = %x\n\r", \
		  hart0_interrupt_matrix[interrupt_id].id, \
		  hart0_interrupt_matrix[interrupt_id].state, \
		  hart0_interrupt_matrix[interrupt_id].priority, \
		  hart0_interrupt_matrix[interrupt_id].count);*/

	//log_emit(TRACE,"INTERRUPT_Complete exited\n\r");
}

/** @fn uint32_t INTERRUPT_Claim_Request()
 * @brief know the id of the interrupt
 * @details read the interrupt claim register to know the interrupt id
 *           of the highest priority pending interrupt
 * @return uint32_t
 */
inline static uint32_t INTERRUPT_Claim_Request()
{
	volatile uint32_t *interrupt_claim_address = NULL;
	// uint32_t interrupt_id;

	//log_emit(TRACE,"INTERRUPT_Claim_Request entered\n\r");

	/*
	   return the interrupt id. This will be used to index into the plic isr table.
	   From the isr table, the exact isr will be called

	   refer https://gitlab.com/shaktiproject/uncore/devices/blob/master/plic/plic.bsv as on 26/8/2019
	 */

	
	interrupt_claim_address = (uint32_t *)(PLIC_BASE +
					       PLIC_CLAIM_OFFSET);

	int  interrupt_id = *interrupt_claim_address;  //My uncommenting because I didnt get the head and tail ,just the body

	//log_emit(DEBUG,"interrupt id [%08x] claimed  at address %08x\n\r", interrupt_id,
		  //interrupt_claim_address );

	//log_emit(TRACE,"INTERRUPT_Claim_Request exited\n\r");

	return interrupt_id;
}


void PLIC_Handler(uintptr_t interrupt_id)
{

	//log_emit(TRACE,"PLIC_Handler entered\n\r");

	interrupt_id = INTERRUPT_Claim_Request();

	//log_emit(DEBUG,"Interrupt ID claimed = %x\n\r", interrupt_id);

	if (interrupt_id <= 0 || interrupt_id > PLIC_MAX_INTERRUPT_SRC)
	{
		//log_emit(FATAL,"Fatal error, interrupt id [%08x] claimed is wrong\n\r", interrupt_id);
	}

	/*change state to active*/
	hart0_interrupt_matrix[interrupt_id].state = ACTIVE;

	//log_emit(DEBUG," Interrupt ID %d, state changed to %d\n\r",
		  //interrupt_id,hart0_interrupt_matrix[interrupt_id].state);

	/*call relevant interrupt service routine*/
	isr_table[interrupt_id]((uint64_t)interrupt_arg[interrupt_id]);

	INTERRUPT_Complete(interrupt_id);

	//log_emit(DEBUG,"Interrupt ID %d completed. \n\r", interrupt_id);

	//log_emit(TRACE,"PLIC_Handler exited\n\r");
}

/** @fn uint32_t ISR_Default(uint32_t interrupt_id) 
 * @brief default interrupt service routine
 * @details Default isr. Use it when you dont know what to do with interrupts
 * @param uint32_t interrupt_id
 * @return uint32_t
 */
static inline void ISR_Default(uint64_t interrupt_id)
{
	//log_emit(TRACE,"ISR_Default entered\n\r");

	if( interrupt_id > 0 && interrupt_id < 7 )  //PWM Interrupts
	{
		/*
		   Assuming 6 pwm's are there
		 */
/**#ifndef SOS
		if(pwm_check_continuous_mode((6-interrupt_id)) == 0)
		{
			set_pwm_control_register((6-interrupt_id),0x80);
		}
#endif
*/
	}

	//log_emit(INFO,"Interrupt [%d] serviced\n\r",interrupt_id);

	//log_emit(TRACE,"ISR_Default exited\n\r");
}


void INTERRUPT_Enable(uint32_t interrupt_id)
{
	uint32_t *interrupt_enable_addr;
	uint32_t current_value = 0x00, new_value;

	//log_emit(TRACE,"INTERRUPT_Enable entered \n\r");

	//log_emit(INFO,"Interrupt ID = %04x\n\r", interrupt_id);

	//log_emit(DEBUG,"PLIC BASE ADDRESS = %08x, PLIC ENABLE OFFSET = %08x\n\r" \
			,PLIC_BASE, PLIC_ENABLE_OFFSET);
	
	interrupt_enable_addr = (uint32_t *) (PLIC_BASE +
			PLIC_ENABLE_OFFSET +
			((interrupt_id / 32)*sizeof(uint32_t)));

	current_value = *interrupt_enable_addr;

	//log_emit(INFO,"interrupt_enable_addr = %08x current_value = %08x \n\r", \
			interrupt_enable_addr, current_value);

	/*set the bit corresponding to the interrupt src*/
	new_value = current_value | (0x1 << (interrupt_id % 32)); 

	*((uint32_t*)interrupt_enable_addr) = new_value;

	//log_emit(DEBUG,"Interrupt Value = %x\n\r", new_value);

	//log_emit(TRACE,"INTERRUPT_Enable exited \n\r");

}

uint8_t INTERRUPT_Disable(uint32_t interrupt_id)
{
	volatile uint32_t *interrupt_disable_addr = 0;
	uint32_t current_value = 0x00, new_value;

	//log_emit(TRACE,"INTERRUPT_Disable entered \n\r");

	//log_emit(DEBUG,"Interrupt ID = %04x\n\r", interrupt_id);

	//log_emit(DEBUG,"PLIC BASE ADDRESS = %08x, PLIC ENABLE OFFSET = %08x interrupt_id = %08x\n\r",
				  //PLIC_BASE, PLIC_ENABLE_OFFSET,interrupt_id);

	interrupt_disable_addr = (uint32_t *) (PLIC_BASE +
					      PLIC_ENABLE_OFFSET +
					      (interrupt_id / 32)*sizeof(uint32_t));

	current_value = *interrupt_disable_addr;

	//log_emit(DEBUG,"interrupt_disable_addr = %08x, current_value = %08x \n\r",
		  //interrupt_disable_addr, current_value);

	/*unset the bit corresponding to the interrupt src*/
	new_value = current_value & (~(0x1 << (interrupt_id % 32)));

	*interrupt_disable_addr = new_value;

	hart0_interrupt_matrix[interrupt_id].state = INACTIVE;

	//log_emit(DEBUG,"Interrupt ID %d, state changed to %d\n\r",
		  //interrupt_id,hart0_interrupt_matrix[interrupt_id].state);

	//log_emit(TRACE,"INTERRUPT_Disable exited\n\r");

	return SUCCESS;
}


uint8_t INTERRUPT_Threshold(uint32_t priority_value)
{
	//log_emit(TRACE,"INTERRUPT_Threshold entered\n\r");

	volatile uint32_t *interrupt_threshold_priority = NULL;

	interrupt_threshold_priority = (uint32_t *) (PLIC_BASE +
						     PLIC_THRESHOLD_OFFSET);

	*interrupt_threshold_priority = priority_value;

	//log_emit(INFO,"PLIC Threshold set to %d\n\r", *interrupt_threshold_priority);

	//log_emit(TRACE,"INTERRUPT_Threshold exited\n\r");

	return SUCCESS;
}

uint32_t SET_Interrupt_Priority(uint32_t priority_value, uint32_t int_id)
{
	//log_emit(TRACE,"SET_Interrupt_Priority entered %x\n\r", priority_value);

	volatile uint32_t * interrupt_priority_address;

	/*
	   base address + priority offset + 4*interruptId
	 */

	
	interrupt_priority_address = (uint32_t *) (PLIC_BASE +
						   PLIC_PRIORITY_OFFSET +
						   (int_id <<
						    PLIC_PRIORITY_SHIFT_PER_INT));

	//log_emit(DEBUG,"Interrupt_priority_address = %x\n\r", interrupt_priority_address);

	//log_emit(DEBUG,"Current data at interrupt_priority_address = %x\n\r", *interrupt_priority_address);

	*interrupt_priority_address = priority_value;

	//log_emit(DEBUG,"New data at interrupt_priority_address = %x\n\r", *interrupt_priority_address);

	//log_emit(TRACE,"SET_Interrupt_Priority exited\n\r");

	return SUCCESS;
}

static uint8_t Global_interrupt_enable()
{
	//log_emit(TRACE, "Reading MSTATUS and MIE");

    asm volatile("li      t0, 8\t\n"
             "csrrs   zero, mstatus, t0\t\n"
            );
    
    //log_emit(TRACE, "Completed reading MSTATUS.");

    asm volatile("li      t0, 0x800\t\n"
             "csrrs   zero, mie, t0\t\n"
            );

    //log_emit(TRACE, "Completed reading MIE.");

	return SUCCESS;
}

uint8_t PLIC_Init()
{
	uint32_t int_id = 0;

	//log_emit(TRACE,"PLIC_Init entered.\n\r");

	/*Assign service routine for external interrupt in machine mode*/
	//mcause_interrupt_table[MACH_EXTERNAL_INTERRUPT] = PLIC_Handler;

	//log_emit(DEBUG,"Assigned PLIC_Handler to trap id : %d.\n\r", MACH_EXTERNAL_INTERRUPT);
	
	hart0_interrupt_matrix[0].state = INACTIVE;
	hart0_interrupt_matrix[0].id = 0;
	hart0_interrupt_matrix[0].priority = 0;
	hart0_interrupt_matrix[0].count = 0;

	for(int_id = 1; int_id <= PLIC_MAX_INTERRUPT_SRC; int_id++)
	{
		hart0_interrupt_matrix[int_id].state = INACTIVE;
		hart0_interrupt_matrix[int_id].id = int_id;
		hart0_interrupt_matrix[int_id].priority = PLIC_PRIORITY_3;
		hart0_interrupt_matrix[int_id].count = 0;

		//log_emit(DEBUG,"************************************************");

		/*Disable all interrupts at the beginning*/
		INTERRUPT_Disable(int_id);

		/*assign a default isr for all interrupts*/
		isr_table[int_id] = ISR_Default;

		/*set priority for all interrupts*/

		SET_Interrupt_Priority(PLIC_PRIORITY_3, int_id);


		/*log_emit(DEBUG,"Interrupt ID = %x \n\r Reset to default values state = %x \
			  \n\r Priority = %x\n\r Count = %x\n\r ",
			  hart0_interrupt_matrix[int_id].id, \
			  hart0_interrupt_matrix[int_id].state, \
			  hart0_interrupt_matrix[int_id].priority, \
			  hart0_interrupt_matrix[int_id].count);

		log_emit(DEBUG, "*************************************************\n\r");*/
	}

	INTERRUPT_Threshold(PLIC_PRIORITY_1);
	
	Global_interrupt_enable();

	//log_emit(TRACE,"PLIC_Init exited. \n\r");

  	return SUCCESS;
}


uint8_t IRQ_Connect(PLIC_Config_t *plic_config)
{
	//log_emit(TRACE,"IRQ_Connect entered. \n\r");

	INTERRUPT_Enable(plic_config->interrupt_id);

	//log_emit(DEBUG, "Interrupt ID: %d, Interrupt Handler Address: %08x, ISR Table Address: %08x", plic_config->interrupt_id, plic_config->fptr, isr_table);

    isr_table[plic_config->interrupt_id] = plic_config->fptr;

	//log_emit(DEBUG, "ISR Address: %08x, ISR Table [%d]: %08x", &isr_table[plic_config->interrupt_id], plic_config->interrupt_id, isr_table[plic_config->interrupt_id]);

    //log_emit(DEBUG, "Interrupt ID: %d, Interrupt Priority Value: %08x", plic_config->interrupt_id, plic_config->priority_value);
	
	SET_Interrupt_Priority(plic_config->priority_value,plic_config->interrupt_id);

	//log_emit(TRACE,"IRQ_Connect exited. \n\r");

	return SUCCESS;
}