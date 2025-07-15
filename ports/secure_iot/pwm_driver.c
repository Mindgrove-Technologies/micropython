
/***************************************************************************
 * Project               	    : shakti devt board
 * Name of the file	            : pwmv2.h
 * Brief Description of file    : Header file for PWM V2 Driver.
 * Name of Author    	        : Sathya Narayanan, Kotteeswaran
 * Email ID                     : kottee.off@gmail.com

 Copyright (C) 2021  IIT Madras. All rights reserved.

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
 * @file pwmv2.c
 * @project shakti devt board
 * @brief Driver source file for PWM V2 Driver.
 */

#include "pwm.h"
#include "log.h"
#include "gpio.h"
#include "secure_iot.h"
#include "pinmux.h"

// volatile pwm_struct *pwm_instance[PWM_MAX_COUNT];
volatile PWM_Type *pwm_instance[PWM_MAX_COUNT];

void PWM_Init(PWM_Config_t *config, int count, ...)
{
	va_list args;
    int enable_pwm[8];
	
	va_start(args, count);

    for(uint8_t i = 0; i < count; i++)
    {
        enable_pwm[i] = va_arg(args, int);
    }
    va_end(args);
	
	ENABLE_Pwm(count, enable_pwm);
	
	for(int i=0; i< count; i++){
		pwm_instance[enable_pwm[i]] = (PWM_Type*) (PWM0_BASE + (enable_pwm[i] * PWM_OFFSET) );
		// printf("\n pwm_instance[%x]: %x",  i, pwm_instance[i]);
		log_debug("\n Initilization Done");
	}
}


int PWM_Set_Control(PWM_Config_t *config, int count, int pwm[])
{
	for (int i = 0; i < count; i++)
	{
		pwm_instance[pwm[i]]->CTRL |= config->value;
	}
	log_debug("\n Control Register of module number %d set to %x", config->pwm_number, config->value);

	return 1;
}


int PWM_Clr_Control(PWM_Config_t *config)
{
	pwm_instance[config->pwm_number]->CTRL &= (~config->value & 0xffff);

	log_debug("\n Control Register of module number %d clear to %x", config->pwm_number, config->value);
	log_info("\n Control Register of module number %d clear to %x", config->pwm_number, config->value);

	return 1;
}


int PWM_Set_Deadband_Delay(PWM_Config_t *config)
{
	pwm_instance[config->pwm_number]->DEADBAND_DELAY=config->value;

	log_debug("\n DeadBand Delay Register of module number %d set to %x", config->pwm_number, config->value);

	return 1;
}


void PWM_Clear(PWM_Config_t *config)
{
	pwm_instance[config->pwm_number]->CTRL=0;
	pwm_instance[config->pwm_number]->DUTY_CYCLE=0;
	pwm_instance[config->pwm_number]->PERIOD=0;
	pwm_instance[config->pwm_number]->DEADBAND_DELAY=0;
	log_debug("\n All registers of module number %d cleared", config->pwm_number);
	log_debug("\n All registers of module number %d cleared", config->pwm_number);

}


inline int  PWM_Config_Control(PWM_Config_t *config)
{
	int value = 0x0;

	if(config->update==1)
	{
		value |=PWM_UPDATE_ENABLE;
	}

	if(config->interrupt_mode==0 || config->interrupt_mode==3 || config->interrupt_mode==5 || config->interrupt_mode==6)
	{
		value |= PWM_RISE_INTERRUPT_ENABLE;
	}

	if(config->interrupt_mode==1 || config->interrupt_mode==3 || config->interrupt_mode==4 || config->interrupt_mode==6)
	{
		value |= PWM_FALL_INTERRUPT_ENABLE;
	}

	if(config->interrupt_mode==2 || config->interrupt_mode==4 || config->interrupt_mode==5 || config->interrupt_mode==6)
	{
		value |= PWM_HALFPERIOD_INTERRUPT_ENABLE;
	}

	if(config->change_output_polarity)
	{
		value |= PWM_OUTPUT_POLARITY;
	}

	return value;
}


void PWM_Set_Duty_Cycle(PWM_Config_t *config)
{
	pwm_instance[config->pwm_number]->DUTY_CYCLE=config->duty;

	log_debug("\n Duty Register of module number %d set to %x", config->pwm_number, config->duty);

	// return 1;
}


void PWM_Set_Periodic_Cycle(PWM_Config_t *config)
{
	pwm_instance[config->pwm_number]->PERIOD=config->period;

	log_debug("\n Period Register of module number %d set to %x", config->pwm_number, config->period);

	// return 1;
}


void PWM_Set_Prescalar_Value(PWM_Config_t *config, int count, int pwm[])
{
	if( 32768 < config->prescalar_value )
	{
		log_error("Prescaler value should be less than 32768");
		return;
	}

	for( int i=0; i<count; i++){
		pwm_instance[pwm[i]]->CLOCK_CTRL = (config->prescalar_value << 1);
	}
}


void PWM_Reset_All(PWM_Config_t *config)
{
	config->pwm_number = PWM_MAX_COUNT  - 1 ;
	for(;config->pwm_number>0;config->pwm_number--)
	{
		PWM_Clear(config);
	}

	pwm_instance[config->pwm_number]->CLOCK_CTRL = 0;
	log_debug("\n All registers cleared");
}


void PWM_Configure(PWM_Config_t *config, int count, int pwm[])
{
	for (int i = 0; i < count; i++)
	{
		pwm_instance[pwm[i]]->DUTY_CYCLE=config->duty;
		pwm_instance[pwm[i]]->PERIOD=config->period;
		pwm_instance[pwm[i]]->DEADBAND_DELAY = config->deadband_delay;
	}

	int control =  PWM_Config_Control(config);

	for (int i = 0; i < count; i++)
	{
		pwm_instance[pwm[i]]->CTRL=control;
	}
	log_debug("PWM %d succesfully configured with %x",config->pwm_number, pwm_instance[config->pwm_number]->CTRL);
}


void PWM_Start(PWM_Config_t *config, int count, ...)
{	
	va_list args;

	int pwm[count];

	va_start(args, count);
	for(uint8_t i = 0; i < count; i++)
    {
        pwm[i] = va_arg(args, int);
    }
    va_end(args);

	PWM_Set_Prescalar_Value(config, count, pwm);

	PWM_Configure(config, count, pwm);

	int value= 0x0;
	value = pwm_instance[config->pwm_number]->CTRL ;

	value |= (PWM_UPDATE_ENABLE | PWM_ENABLE | PWM_START | PWM_OUTPUT_ENABLE);
	
	for (int i = 0; i < count; i++)
	{
		pwm_instance[pwm[i]]->CTRL = value;
	}

	PWM_Set_Control(config, count, pwm);
}


void PWM_Update(PWM_Config_t *config)
{
	pwm_instance[config->pwm_number]->DUTY_CYCLE=config->duty;
	pwm_instance[config->pwm_number]->PERIOD=config->period;

	int control =  PWM_Config_Control(config);

	pwm_instance[config->pwm_number]->CTRL = control;
	
    log_debug("PWM %d succesfully updated",config->pwm_number);
}


void PWM_Stop(PWM_Config_t *config,int count, ...)
{
	va_list args;

	int pwm[count];

	va_start(args, count);
	for(uint8_t i = 0; i < count; i++)
    {
        pwm[i] = va_arg(args, int);
    }
    va_end(args);

	int value = 0xfff8;  //it will set pwm_enable,pwm_start,pwm_output_enable  to zero

	for (int i = 0; i < count; i++)
	{
		pwm_instance[pwm[i]]->CTRL &= value;
		log_debug("\n PWM module number %d has been stopped", pwm[i]);
		log_debug("\n PWM module number %d has been stopped", pwm[i]);
	}
}


void PWM_Show_Frequency(PWM_Config_t *config)
{
	int frequency;
	frequency =  CLOCK_FREQUENCY_BASE/((config->period*(config->prescalar_value+1)));
	log_debug("\n PWM %d The expected Frequency %d", config->pwm_number, frequency);
	printf("\n PWM %d The expected Frequency %d", config->pwm_number, frequency);
	//So there is no definite field for frequency
}

void PWM_Show_Values(PWM_Config_t *config)
{	
	log_debug("\n MODULE SPECIFIC REGISTERS");
	log_debug("\n PWM %d Clock Control Register %x",config->pwm_number, pwm_instance[config->pwm_number]->CLOCK_CTRL);
	log_debug("\n PWM %d Period Register %x" ,config->pwm_number, pwm_instance[config->pwm_number]->PERIOD);
	log_debug("\n PWM %d Control Register %x" ,config->pwm_number, pwm_instance[config->pwm_number]->CTRL);
	log_info("\n PWM %d Duty Register %x" ,config->pwm_number, pwm_instance[config->pwm_number]->DUTY_CYCLE);
	log_debug("\n PWM %d DeadBand Delay Register %x" ,config->pwm_number, pwm_instance[config->pwm_number]->DEADBAND_DELAY);
}

//This is not there in the new production version

void PWM_Isr_Handler0(){ 
	
	if(pwm_instance[0]->CTRL & PWM_RISE_INTERRUPT)
		log_debug("Rising Edge Interrupt Triggered");
		
	if(pwm_instance[0]->CTRL & PWM_FALL_INTERRUPT)
		log_debug("Falling Edge Interrupt Triggered");

	if(pwm_instance[0]->CTRL & PWM_HALFPERIOD_INTERRUPT)
		log_debug("HalfPeriod Interrupt Triggered");		

	log_debug("\n PWM 0 Interrupt");
}


void PWM_Isr_Handler1(){ 
 
	if(pwm_instance[1]->CTRL & PWM_RISE_INTERRUPT)
		log_debug("Rising Edge Interrupt Triggered");
		
	if(pwm_instance[1]->CTRL & PWM_FALL_INTERRUPT)
		log_debug("Falling Edge Interrupt Triggered");

	if(pwm_instance[1]->CTRL & PWM_HALFPERIOD_INTERRUPT)
		log_debug("HalfPeriod Interrupt Triggered");		

	log_debug("\n PWM 1 Interrupt");
 
}


void PWM_Isr_Handler2(){ 
	
	if(pwm_instance[2]->CTRL & PWM_RISE_INTERRUPT)
		log_debug("Rising Edge Interrupt Triggered");
		
	if(pwm_instance[2]->CTRL & PWM_FALL_INTERRUPT)
		log_debug("Falling Edge Interrupt Triggered");

	if(pwm_instance[2]->CTRL & PWM_HALFPERIOD_INTERRUPT)
		log_debug("HalfPeriod Interrupt Triggered");		

	log_debug("\n PWM 2 Interrupt"); 
}


void PWM_Isr_Handler3(){ 
	
	if(pwm_instance[3]->CTRL & PWM_RISE_INTERRUPT)
		log_debug("Rising Edge Interrupt Triggered");
		
	if(pwm_instance[3]->CTRL & PWM_FALL_INTERRUPT)
		log_debug("Falling Edge Interrupt Triggered");

	if(pwm_instance[3]->CTRL & PWM_HALFPERIOD_INTERRUPT)
		log_debug("HalfPeriod Interrupt Triggered");		

	log_debug("\n PWM 3 Interrupt"); 
}


void PWM_Isr_Handler4(){ 
	
	if(pwm_instance[4]->CTRL & PWM_RISE_INTERRUPT)
		log_debug("Rising Edge Interrupt Triggered");
		
	if(pwm_instance[4]->CTRL & PWM_FALL_INTERRUPT)
		log_debug("Falling Edge Interrupt Triggered");

	if(pwm_instance[4]->CTRL & PWM_HALFPERIOD_INTERRUPT)
		log_debug("HalfPeriod Interrupt Triggered");		

	log_debug("\n PWM 4 Interrupt"); 
}


void PWM_Isr_Handler5(){ 
	
	if(pwm_instance[5]->CTRL & PWM_RISE_INTERRUPT)
		log_debug("Rising Edge Interrupt Triggered");
		
	if(pwm_instance[5]->CTRL & PWM_FALL_INTERRUPT)
		log_debug("Falling Edge Interrupt Triggered");

	if(pwm_instance[5]->CTRL & PWM_HALFPERIOD_INTERRUPT)
		log_debug("HalfPeriod Interrupt Triggered");		

	log_debug("\n PWM 5 Interrupt"); 
}
//--------------------------------------------------------------------------

