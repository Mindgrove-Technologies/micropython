/**
 * Project                           : Secure IoT SoC
 * Name of the file                  : gptimer_driver.c
 * Brief Description of file         : Driver to Standard gp_timer driver
 * Name of Author                    : Suneeth D, Shri Mahaalakshmi S J
 * Email ID                          : suneethdamodharan@gmail.com, mahaalakshmi@mindgrovetech.in
 * 
 * @file gptimer.h
 * @author Suneeth D (suneethdamodharan@gmail.com)
 * @author Shri Mahaalakshmi S J (mahaalakshmi@mindgrovetech.in)
 * @brief This is a Baremetal GP_TIMER Driver's header file for Mindgrove Silicon's General Purpose Timer module.
 * @version 0.2
 * @date 2024-09-20
 * 
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2024. All rights reserved.
 * 
 */

#include "gptimer.h"

#define GPT_MAX_COUNT 4
#define CHECK_UPDATE_EN 0x20000
#define CONTROL_REG(gpt_num) gpt_instance[gpt_num] -> CTRL 
#define CLOCK_CNTRL(gpt_num) gpt_instance[gpt_num] -> CLOCK_CTRL
#define COUNTER_VAL(gpt_num) gpt_instance[gpt_num] -> COUNT
#define REPEATD_COUNT(gpt_num) gpt_instance[gpt_num] -> RPTD_COUNT
#define DUTY_CYCL(gpt_num) gpt_instance[gpt_num] -> DUTY_CYCLE
#define PERIOD_REG(gpt_num) gpt_instance[gpt_num]-> PERIOD
#define CAPTR_VAL(gpt_num) gpt_instance[gpt_num] -> CAPTURE_INP

/* Bit Specifications of each GP_TIMER's register*/
/* Control Register */
#define GPT_EN              (1<<0)
#define GPT_MODE(x)         (x<<2)
#define GPT_OUTPUT_EN       (1<<4)
#define COUNT_RESET         (1<<5)
#define CONTIN_CNT_EN       (1<<6)
#define PWM_FALL_INTR_EN    (1<<7) 
#define PWM_RISE_INTR_EN    (1<<8)
#define CNTR_OFLOW_INTR_EN  (1<<9)
#define CNTR_UFLOW_INTR_EN  (1<<10)
#define CAPTURE_IP(x)       (x<<15) 

/* Clock Control Reg*/
#define clk_src (0<<0)
#define clk_prescalar(x)    (x<<1)
#define clk_update_en(x)    (x<<17)

/* Duty Cycle Reg*/
#define DUTY_CYC_VAL(x)     (x<<0)

/* Period Reg*/
#define PRD_VAL(x)          (x<<0)

GPTIMER_Type *gpt_instance[GPT_MAX_COUNT];

uint32_t GPT_Set_Period(GPTIMER_Config_t *gptimer)
{
    PERIOD_REG(gptimer->gpt_num) = (PERIOD_REG(gptimer->gpt_num) & 0x00000000) | gptimer->period;
    log_debug("Period register : %x\n", PERIOD_REG(gptimer->gpt_num));
    return SUCCESS;
}

uint32_t GPT_Set_Dutycycle(GPTIMER_Config_t *gptimer)
{
    int dut_cyc;
    dut_cyc = ((int)(gptimer->dutycycle * gptimer->period))/100;
    DUTY_CYCL(gptimer->gpt_num) = (DUTY_CYCL(gptimer->gpt_num) & 0x00000000) | dut_cyc;
    log_debug("Dutycycle value: %x\n", DUTY_CYCL(gptimer->gpt_num));
    return SUCCESS;
}

uint32_t GPT_Set_Prescalar(GPTIMER_Config_t *gptimer)
{
    if(gptimer->prescalar != 0)
    {
        CLOCK_CNTRL(gptimer->gpt_num) |= (clk_prescalar(gptimer->prescalar) & 0xFFFFFFFE);
        log_debug("Clock register : %x\n", CLOCK_CNTRL(gptimer->gpt_num));
    }

    else
    {        
        log_error("Invalid Prescalar Value! Expected Prescalar Value: > 0\n");    
    }
    return SUCCESS;
}

uint32_t GPT_Init(GPTIMER_Config_t *gptimer)
{
    switch (gptimer->gpt_num)
    {
    case GPTIMER_0:
        gpt_instance[gptimer->gpt_num] = GPTIMER0;
        break;
    case GPTIMER_1:
        gpt_instance[gptimer->gpt_num] = GPTIMER1;
        break;
    case GPTIMER_2:
        gpt_instance[gptimer->gpt_num] = GPTIMER2;
        break;
    case GPTIMER_3:
        gpt_instance[gptimer->gpt_num] = GPTIMER3;
        break;
    default:
        log_error("Invalid access attempt! Try access number <4\n");
        return EPERM;
    }
    
    GPT_Set_Period(gptimer);
    GPT_Set_Prescalar(gptimer);
    
    if(gptimer->cnt_en == GPT_CONTIN_COUNT_EN)
    {        
        CONTROL_REG(gptimer->gpt_num) |= CONTIN_CNT_EN;            
    }

    if(gptimer->output_en == GPT_OUT_EN)
    {
        CONTROL_REG(gptimer->gpt_num) |= GPT_OUTPUT_EN;
    }

    if(gptimer->mode == GPT_PWM_MODE)
    {
        CONTROL_REG(gptimer->gpt_num) |= PWM_FALL_INTR_EN | PWM_RISE_INTR_EN;
        if ((gptimer->dutycycle <= 100) && (gptimer->dutycycle >= 0))
        {
            GPT_Set_Dutycycle(gptimer);        
        }

        else
        {
            log_error("Invalid DutyCycle value! Expected value: <= 100\n");
        }
    }

    else if(gptimer->mode == GPT_UP_COUNT)
    {
        if(gptimer->interrupt_en == GPT_INTR_EN)
            CONTROL_REG(gptimer->gpt_num) |= CNTR_OFLOW_INTR_EN;
        else
            CONTROL_REG(gptimer->gpt_num) &= ~CNTR_OFLOW_INTR_EN;
    }

    else if(gptimer->mode == GPT_DOWM_COUNT /*&& interrupt_en == 1*/)
    {
        if(gptimer->interrupt_en == GPT_INTR_EN)
            CONTROL_REG(gptimer->gpt_num) |= CNTR_UFLOW_INTR_EN;
        else
            CONTROL_REG(gptimer->gpt_num) &= ~CNTR_UFLOW_INTR_EN;
    }

    else if(gptimer->mode == GPT_UPDOWN_COUNT /*&& interrupt_en == 1*/)
    {
        if(gptimer->interrupt_en == GPT_INTR_EN)
            CONTROL_REG(gptimer->gpt_num) |= CNTR_OFLOW_INTR_EN | CNTR_UFLOW_INTR_EN;
        else
            CONTROL_REG(gptimer->gpt_num) &= ~(CNTR_OFLOW_INTR_EN | CNTR_UFLOW_INTR_EN);
    }

    else 
    {
        log_error("Invalid Mode value! Expected value < 4\n");
    }

    CONTROL_REG(gptimer->gpt_num) |= GPT_EN | GPT_MODE(gptimer->mode) | CAPTURE_IP(gptimer->capture_val) | COUNT_RESET;  //Starting the Counter/PWM wrt its mode of operation
    return SUCCESS;
}

uint32_t GPT_Update_Enable(GPTIMER_Config_t *gptimer)
{
    if(CLOCK_CNTRL(gptimer->gpt_num) & CHECK_UPDATE_EN)
    {
        CLOCK_CNTRL(gptimer->gpt_num) = (0xFFFDFFFF & CLOCK_CNTRL(gptimer->gpt_num));
        log_debug("Clock register : %x\n", CLOCK_CNTRL(gptimer->gpt_num));
        log_debug("Clock register : %x\n", (0xFFFDFFFF & CLOCK_CNTRL(gptimer->gpt_num)));
    }
    return SUCCESS;
}

uint32_t GPT_Read_Counter_Val(GPTIMER_Config_t *gptimer)
{
    log_debug("Counter value : %x\n", COUNTER_VAL(gptimer->gpt_num));
    return COUNTER_VAL(gptimer->gpt_num);
}

uint32_t GPT_Read_ReptdCount(GPTIMER_Config_t *gptimer)
{
    log_debug("Repeated count value : %x\n", REPEATD_COUNT(gptimer->gpt_num));
    return REPEATD_COUNT(gptimer->gpt_num);
}

uint32_t GPT_Read_Captured_Val(GPTIMER_Config_t *gptimer)
{
    log_debug("Captured value : %x\n", CAPTR_VAL(gptimer->gpt_num));
    return CAPTR_VAL(gptimer->gpt_num);
}

uint32_t GPT_Reset(GPTIMER_Config_t *gptimer)
{
    CONTROL_REG(gptimer->gpt_num) |= (1<<5);
    log_debug("Control register: %x\n", CONTROL_REG(gptimer->gpt_num));
    return SUCCESS;
}

uint32_t GPT_Delay_Milliseconds(GPTIMER_Config_t *gptimer, uint32_t delay) 
{
    uint32_t prescalar = CLOCK_FREQUENCY_BASE/1000000;
    uint32_t period = delay * 1000;
    gptimer->period = period;
    gptimer->prescalar = prescalar;
    
    GPT_Init(gptimer);
    volatile int counter_val = COUNTER_VAL(gptimer->gpt_num);
    while (1)
    {
        if (counter_val == 0)
            break;
        else
            counter_val = COUNTER_VAL(gptimer->gpt_num);
    }
    return SUCCESS;
}

uint32_t GPT_Delay_Millisecs_H(uint32_t delay) 
{
    GPTIMER_Config_t gptimer2; // Using gptimer instance 2
    GPTIMER_Config_t *gptimer = &gptimer2;
    uint32_t prescalar = CLOCK_FREQUENCY_BASE/1000000;
    uint32_t period = delay * 1000;
    gptimer->gpt_num = 2;
    gptimer->mode = 2;
    gptimer->interrupt_en = 0;
    gptimer->period = period;
    gptimer->prescalar = prescalar;
    gptimer->dutycycle = 0;
    gptimer->cnt_en = 0;
    gptimer->capture_val = 0;
    gptimer->output_en = 0;
    
    GPT_Init(gptimer);
    volatile int counter_val = COUNTER_VAL(gptimer->gpt_num);
    while (1)
    {
        if (counter_val == 0)
            break;
        else
            counter_val = COUNTER_VAL(gptimer->gpt_num);
    }
    return SUCCESS;
}

uint32_t GPT_Delay_Microseconds(GPTIMER_Config_t *gptimer, uint32_t delay) 
{
    uint32_t prescalar = CLOCK_FREQUENCY_BASE/1000000;
    uint32_t period = delay * 1;
    gptimer->period = period;
    gptimer->prescalar = prescalar;
    
    GPT_Init(gptimer);
    volatile int counter_val = COUNTER_VAL(gptimer->gpt_num);
    while (1)
    {
        if (counter_val == 0)
            break;
        else
            counter_val = COUNTER_VAL(gptimer->gpt_num);
    }
    return SUCCESS;
}

uint32_t GPT_Delay_Microsecs_H(uint32_t delay) 
{
    GPTIMER_Config_t gptimer2; // Using gptimer instance 2
    GPTIMER_Config_t *gptimer = &gptimer2;
    uint32_t prescalar = CLOCK_FREQUENCY_BASE/1000000;
    uint32_t period = delay * 1;
    gptimer->gpt_num = 2;
    gptimer->mode = 2;
    gptimer->interrupt_en = 0;
    gptimer->period = period;
    gptimer->prescalar = prescalar;
    gptimer->dutycycle = 0;
    gptimer->cnt_en = 0;
    gptimer->capture_val = 0;
    gptimer->output_en = 0;
    
    GPT_Init(gptimer);
    volatile int counter_val = COUNTER_VAL(gptimer->gpt_num);
    while (1)
    {
        if (counter_val == 0)
            break;
        else
            counter_val = COUNTER_VAL(gptimer->gpt_num);
    }
    return SUCCESS;
}