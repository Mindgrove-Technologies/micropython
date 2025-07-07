#include "utils.h"
#include "string_functions.h"
#include <stdio.h>

#define MILLIS 1500000 //Defines the number of system clock cycles per millisecond tick. Calculated using the formula: (CLK_FREQ (700 MHz) × 10⁻³) / PRESCALAR (256).
#define MSIP       0x02000000
#define MTIME      0x0200BFF8
#define MTIMECMP   0x02004000
#define CLINT_DIVISOR   256

volatile uint64_t* msip     = (volatile uint64_t*)MSIP;
volatile uint64_t* mtime    = (volatile uint64_t*)MTIME;
volatile uint64_t* mtimecmp = (volatile uint64_t*)MTIMECMP;
volatile uint64_t milliscount;
static uint64_t last_count;

// Pointer to the MSIP register
//extern volatile uint64_t* msip;

// Pointer to the MTIME register
//extern volatile uint64_t* mtime;

// Pointer to the MTIMECMP register
//extern volatile uint64_t* mtimecmp;

unsigned long ReadData(size_t *addr)
{
	// log_emit(DEBUG,"addr = %x data = %x\n\r", addr, *addr);
	return *addr;
}

void WriteData(size_t *addr, unsigned long val)
{
	*addr = val;
	// log_emit(DEBUG,"addr = %x data = %x\n\r", addr, *addr);
}

void millis_init() {
    // Set MSIP register to trigger a software interrupt
    *msip = 0x01;
	//printf("msip: %p, mtime: %p, mtimecmp: %p\n", msip, mtime, mtimecmp);
	//uint64_t val;
    // Try reading
    // val = *msip;
    // printf("msip value: %llx\n", val);
    // val = *mtime;
    // printf("mtime value: %llx\n", val);
    // val = *mtimecmp;
    // printf("mtimecmp value: %llx\n", val);

    // Disable specific interrupts (for demonstration; adjust as needed)
    asm volatile("li      t0, 0xc0\t\n\r"    // Load immediate value
                 "csrrs   zero, mie, t0\t\n\r"); // Set bits in mie register
    // Enable global interrupts
    asm volatile("li      t0, 8\t\n\r"      // Load immediate value
                 "csrrs   zero, mstatus, t0\t\n\r"); // Set bits in mstatus register

    //last_count = * mtime; //changes made
    *mtimecmp = *mtime+MILLIS;
}

__attribute__((weak)) void CLINT_Handler( __attribute__((unused)) uintptr_t int_id,  __attribute__((unused)) uintptr_t epc)
{
	// log_emit(DEBUG,"\nCLINT_Handler entered\n\r");
  	milliscount=milliscount+1;
  	uint64_t now = *mtime;
  	uint64_t dcycles = now - last_count;
  	last_count += dcycles;
  	uint64_t next = last_count + MILLIS;
  	*mtimecmp = -1;
  	*mtimecmp = next;
	//printf("CLint handler called\n\r");
  	// log_emit(DEBUG,"Millicount:%lld\n\r",milliscount);
	// log_emit(DEBUG,"CLINT_Handler exited\n\r");
}


uint64_t millis(){
  	// log_emit(DEBUG,"MILLIS:%llu\n\r",milliscount);
  	return milliscount;
}