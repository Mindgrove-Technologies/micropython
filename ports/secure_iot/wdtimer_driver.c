#include "wdtimer.h"
#include "io.h"

volatile uint16_t *control_reg = (uint16_t*) (CONTROL_REG_ADDR);
volatile uint32_t *wd_cycles = (uint32_t*) (WD_CYCLES_REG_ADDR);
// Number of cycles before triggering the watchdog interrupt/event
volatile uint16_t *reset_cycles = (uint16_t*) (RESET_CYCLES_REG_ADDR);
//No. of cycles passed before triggering a reset

/*void wdt_init()
{
	 volatile uint16_t *control_reg = (uint16_t*) (CONTROL_REG_ADDR);
	 volatile uint32_t *wd_cycles = (uint32_t*) (WD_CYCLES_REG_ADDR);
	 volatile uint16_t *reset_cycles = (uint16_t*) (RESET_CYCLES_REG_ADDR);
}*/


void wdtimer_start(uint16_t mode,uint16_t rcycles,uint32_t wcycles)
{
	*control_reg=mode;
	*reset_cycles=rcycles;
	*wd_cycles=wcycles;
}
void wdtimer_disable()
{
	//*control_reg=*control_reg && 0xFFFE;  -->shouldnt it have been a single and ?
	*control_reg=*control_reg & 0xFFFE; //to clear the watch dog timer
}


void wdtimer_reset()
{
	wdtimer_disable();
	wdtimer_start(*control_reg,*reset_cycles,*wd_cycles);
	// Resets the watchdog timer — it disables and then reinitializes it using previous values.
}


