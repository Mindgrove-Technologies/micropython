/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// This file is never compiled standalone, it's included directly from
// extmod/machine_pwm.c via MICROPY_PY_MACHINE_PWM_INCLUDEFILE.

#include "py/mphal.h"
#include "machine_pwm.h"
#include "py/obj.h"
#include "modmachine.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "pwm.h"

#define clk_freq 30000000
uint32_t prescalar=256;
uint32_t period=256;
//defaults
uint32_t duty;
uint32_t duty_per;
//used throughout the code
#define PWM_NUM 14

#define deadband_delay(period_1) ((1/10)*(period_1))
/*typedef struct
{
  int pwm_number;
  uint32_t duty;
  uint32_t period;
  uint32_t value;
  int pin_number;
  int val;
  bool update;
  pwm_interrupt_modes interrupt_mode;
  bool change_output_polarity;
  uint16_t prescalar_value;
  uint32_t deadband_delay;
}PWM_Config_t;*/

//THis will work only for single instance
//PWM_Config_t pwm_config_b;
//PWM_Config_t *PWM_Config_1 =&pwm_config_b;
// initialize some random values
// PWM_Config_1->pwm_number = 1;
// PWM_Config_1->duty = 50; // should be the divided prescalar value
//THese are statements that should only be inside a function :)

PWM_Config_t pwm_b_inst0={
    .pwm_number=0,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=0,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};

PWM_Config_t pwm_b_inst1={
    .pwm_number=1,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=1,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};
PWM_Config_t pwm_b_inst2={
    .pwm_number=2,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=2,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};

PWM_Config_t pwm_b_inst3={
    .pwm_number=3,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=3,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};

PWM_Config_t pwm_b_inst4={
    .pwm_number=4,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=4,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};
PWM_Config_t pwm_b_inst5={
    .pwm_number=5,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=5,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};
PWM_Config_t pwm_b_inst6={
    .pwm_number=6,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=6,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};
PWM_Config_t pwm_b_inst7={
    .pwm_number=7,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=7,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};
PWM_Config_t pwm_b_inst8={
    .pwm_number=8,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=17,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};
PWM_Config_t pwm_b_inst9={
    .pwm_number=9,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=18,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};
PWM_Config_t pwm_b_inst10={
    .pwm_number=10,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=19,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};
PWM_Config_t pwm_b_inst11={
    .pwm_number=11,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=20,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};
PWM_Config_t pwm_b_inst12={
    .pwm_number=12,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=21,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};

PWM_Config_t pwm_b_inst13={
    .pwm_number=13,
    .duty=0,
    .period=256,
    .value=256,
    .pin_number=22,
    .val=0,
    .update=0,
    .interrupt_mode=7,
    .change_output_polarity=0,
    .prescalar_value=256,
    .deadband_delay=deadband_delay(256),
};


PWM_Config_t* pwm_array[PWM_NUM] = {
    &pwm_b_inst0,
    &pwm_b_inst1,
    &pwm_b_inst2,
    &pwm_b_inst3,
    &pwm_b_inst4,
    &pwm_b_inst5,
    &pwm_b_inst6,   
    &pwm_b_inst7, 
    &pwm_b_inst8,
    &pwm_b_inst9,
    &pwm_b_inst10,
    &pwm_b_inst11,
    &pwm_b_inst12,
    &pwm_b_inst13,
};

typedef struct _machine_pwm_obj_t
{
    mp_obj_base_t base;
    uint8_t num;
    //pyb_pin_obj_t *pin; // create this specifically for pwm pins?
    uint8_t active;
    // uint8_t channel; -->We dont use different clock source for pwm
    uint32_t duty;
    uint32_t freq;
    uint32_t period; // can take 32 bit values 0-(2^32 )-1;
    uint32_t value;  // dummy
    uint8_t pin_number;
    uint16_t prescalar_value; // minimum value of 2 --> So it takes the value as 1
    uint32_t deadband_delay;  // calculated on the fly
} machine_pwm_obj_t;

static bool pwm_inited = false;
extern const mp_obj_type_t machine_pwm_type;


//uint32_t freq =((clk_freq)/((prescalar-1)*period));
//uint32_t freq= 1225;
//added the freq initial value directly as there is a compulsion for const

const machine_pwm_obj_t machine_pwm_obj[PWM_NUM] = {
    {{&machine_pwm_type}, 0,0,0,1225,256,0,0,256,25}, 
    {{&machine_pwm_type}, 1,0,0,1225,256,0,1,256,25},
    {{&machine_pwm_type}, 2,0,0,1225,256,0,2,256,25},
    {{&machine_pwm_type}, 3,0,0,1225,256,0,3,256,25},
    {{&machine_pwm_type}, 4,0,0,1225,256,0,4,256,25},
    {{&machine_pwm_type}, 5,0,0,1225,256,0,5,256,25},
    {{&machine_pwm_type}, 6,0,0,1225,256,0,6,256,25},
    {{&machine_pwm_type}, 7,0,0,1225,256,0,7,256,25},
    {{&machine_pwm_type}, 8,0,0,1225,256,0,17,256,25},
    {{&machine_pwm_type}, 9,0,0,1225,256,0,18,256,25},
    {{&machine_pwm_type}, 10,0,0,1225,256,0,19,256,25},
    {{&machine_pwm_type}, 11,0,0,1225,256,0,20,256,25},
    {{&machine_pwm_type}, 12,0,0,1225,256,0,21,256,25},
    {{&machine_pwm_type}, 13,0,0,1225,256,0,22,256,25},
};

static machine_pwm_obj_t *mp_obj_get_pwm_obj(mp_obj_t pwm_in) {
    if (mp_obj_is_small_int(pwm_in)) {
        int pwm_inst = mp_obj_get_int(pwm_in);
        //extracting the integer from the micropython object
        if (pwm_inst >= 0 && pwm_inst < PWM_NUM) {
            return (machine_pwm_obj_t *)&machine_pwm_obj[pwm_inst];
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid instance"));
}

/******************************************************************************/
// MicroPython bindings for PWM

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // the object created just now
    mp_printf(print, "PWM(%u", self->num);
    if (self->active)
    {
        mp_printf(print, ", freq=%u, duty=%u",
                  self->freq,self->duty);
    }
    mp_printf(print, ")");
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum
    {
        ARG_pwm_num,
        ARG_prescalar,
        ARG_freq,
        ARG_duty,
        ARG_period,
        ARG_update,
        ARG_polarity,
        ARG_mode,
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_pwm_num, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = -1}},
        {MP_QSTR_prescalar, MP_ARG_KW_ONLY|MP_ARG_INT, {.u_int = -1}},
        {MP_QSTR_freq, MP_ARG_KW_ONLY|MP_ARG_INT, {.u_int = -1}},
        {MP_QSTR_duty, MP_ARG_KW_ONLY|MP_ARG_INT, {.u_int = -1}},
        {MP_QSTR_period, MP_ARG_KW_ONLY|MP_ARG_INT, {.u_int = -1}},
        {MP_QSTR_update ,MP_ARG_KW_ONLY|MP_ARG_BOOL,{.u_bool=0}},
        {MP_QSTR_polarity ,MP_ARG_KW_ONLY|MP_ARG_BOOL,{.u_bool=0}},
        {MP_QSTR_mode, MP_ARG_KW_ONLY|MP_ARG_INT,{.u_int=7}},
        //default no interrupt
        //(pwm_interrupt_modes)3 -> get the number and type cast it for the enum
        // { MP_QSTR_duty_u16, MP_ARG_INT, {.u_int = -1} },
        // { MP_QSTR_duty_ns, MP_ARG_INT, {.u_int = -1} },
    };
    // make the pwm configuration object here
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    //2 arguments are only required in prescalar,period,freq
    //uint32_t count=0;
    int set=0;
    //uint32_t prescalar;
    if (args[ARG_prescalar].u_int != -1)
    {
        if (args[ARG_prescalar].u_int > 2 | args[ARG_prescalar].u_int <= 32768)
        {
            self->prescalar_value = args[ARG_prescalar].u_int;
            prescalar=self->prescalar_value;
            //pwm_array[self->num]->prescalar_value=prescalar;
            //count++; //I changed my mind 
        }
        else
        {
            self->prescalar_value = 256;
            prescalar=256;
            // like a base reference
        }
    }
    else
    {
        self->prescalar_value = 256;
        prescalar=256;
        // Random value assigned
    }

    pwm_array[self->num]->prescalar_value=prescalar;
    if (args[ARG_freq].u_int != -1)
    {
        // pwm_set_freq(args[ARG_freq].u_int, self->channel);
        // configuring the set frequency value
        // ok frequency is given
        self->freq = args[ARG_freq].u_int;
        uint32_t period = (clk_freq) / ((prescalar-1)*self->freq);
        self->period = period;
        pwm_array[self->num]->period=period;
        self->deadband_delay=deadband_delay(period);
        pwm_array[self->num]->deadband_delay=self->deadband_delay;
        set=1;
    }
    if(set !=1 && args[ARG_period].u_int !=-1){
        //THe value of period is provided insted of frequency value
        uint32_t period=args[ARG_period].u_int;
        self->period=period;
        self->freq= (clk_freq)/((prescalar-1)*period);
        pwm_array[self->num]->period=period;
        self->deadband_delay=deadband_delay(period);
        pwm_array[self->num]->deadband_delay=self->deadband_delay;

    }
    if (args[ARG_duty].u_int != -1)
    {
        // pwm_set_duty(args[ARG_duty].u_int, self->channel);
        //I am getting them as percent
        uint32_t period=self->period;
        duty_per=args[ARG_duty].u_int;
        duty= period*(args[ARG_duty].u_int)/100;
        self->duty=duty;
        pwm_array[self->num]->duty=duty;
        //with respect to the period value
    }
    //specific to the config structure
    if (args[ARG_update].u_int==0){
        pwm_array[self->num]->update=0;
        //just in case if there was a previous instance with the same number?
    }
    else{
        pwm_array[self->num]->update=1;
    }

    if (args[ARG_polarity].u_int==0){
        pwm_array[self->num]->change_output_polarity=0;
    }
    else{
        pwm_array[self->num]->change_output_polarity=1;
    }

    // pwm_start();
    if (args[ARG_mode].u_int >=0 && args[ARG_mode].u_int <=7){
        uint8_t mode=args[ARG_mode].u_int;
        pwm_array[self->num]->interrupt_mode =(pwm_interrupt_modes)mode;
        //converts the obtained integer to enum values
    }
    PWM_Init(pwm_array[self->num],1, args[ARG_pwm_num].u_int);
    //should call pwm configure also?
    self->active=1;
    PWM_Start(pwm_array[self->num],1, args[ARG_pwm_num].u_int);
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    //pyb_pin_obj_t *pin = mp_obj_get_pin_obj(args[0]);
    //I am using a different type for pin

    // create PWM object from the given pin
    machine_pwm_obj_t *self = mp_obj_get_pwm_obj(args[0]);
    self->active = 0; // setting it as low?
    // start the PWM running for this channel
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self)
{
    self->active = 0;
    // pwm_start();
    //PWM_Stop(PWM_Config_1,pwm_num);//made for single pwm at a time
    PWM_Stop(pwm_array[self->num],1,self->num);
    PWM_Clear(pwm_array[self->num]);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pwm_deinit_obj, mp_machine_pwm_deinit);

static void mp_machine_pwm_freq_get(machine_pwm_obj_t *self)
{
    //return MP_OBJ_NEW_SMALL_INT(pwm_get_freq(0));
    //from the object we can obtain the pwm number
    PWM_Show_Frequency(pwm_array[self->num]);
    //passing the corresponding config struct
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pwm_freq_get_obj, mp_machine_pwm_freq_get);

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq)
{
    // pwm_set_freq(freq, 0);
    // we should set the prescalar value ig
    if(freq >0){
    self->freq=freq;
    uint32_t period = (clk_freq) / ((prescalar)*self->freq);
    //should have defined a macro instead :/
    self->period =period;
    self->duty =(period)*duty_per;
    //getting the new duty value
    duty=self->duty;
    uint32_t dead_band=deadband_delay(period);
    self->deadband_delay=dead_band;
    //PWM_Configure(pwm_array[self->num],1,{self->num,});
    int num[]={self->num,};
    PWM_Configure(pwm_array[self->num],1,num);
    //should send the pwm list as an array
    }
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_pwm_freq_set_obj, mp_machine_pwm_freq_set);

static void set_active(machine_pwm_obj_t *self)
{
    if (!self->active)
    {
        // something like calling the init function
        // pwm_add(self->pin->phys_port, self->pin->periph, self->pin->func);
        self->active = 1;
        PWM_Init(pwm_array[self->num],1,self->num);
        //ensuring that the pwm was initialized first
        //Internally enables pwm inside
        PWM_Start(pwm_array[self->num],1,self->num);
    }
}
//static MP_DEFINE_CONST_FUN_OBJ_1(machine_pwm_duty_get_obj, mp_machine_pwm_duty_get);

static mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self)
{
    set_active(pwm_array[self->num]);
    //return MP_OBJ_NEW_SMALL_INT(pwm_get_duty(self->channel));
    //gets the duty value wrt to the period value
    //if not accurate -> read from registers
    printf("%d",self->duty);
    return self->duty; //type cast the obj to uint and you have it :)
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pwm_duty_get_obj, mp_machine_pwm_duty_get);

static void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty)
{
    // set_active(self, false);
    // self->duty_ns = -1;
    // pwm_set_duty(duty, self->channel);
    PWM_Set_Duty_Cycle(pwm_array[self->num]);
    //whatever value is the config goes here
    PWM_Start(pwm_array[self->num],1, self->num);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_pwm_duty_set_obj, mp_machine_pwm_duty_set);

static const mp_rom_map_elem_t machine_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_pwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq_get), MP_ROM_PTR(&machine_pwm_freq_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq_set), MP_ROM_PTR(&machine_pwm_freq_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty_get), MP_ROM_PTR(&machine_pwm_duty_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty_set), MP_ROM_PTR(&machine_pwm_duty_set_obj) },
};
static MP_DEFINE_CONST_DICT(machine_pwm_locals_dict, machine_pwm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pwm_type,
    MP_QSTR_PWM,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_pwm_make_new,
    print, mp_machine_pwm_print,
    //protocol, &machine_spi_p,
    locals_dict, &machine_pwm_locals_dict
    );
