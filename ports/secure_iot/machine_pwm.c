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
#define clk_freq 30000000
uint32_t prescalar=2;
uint32_t duty;
uint32_t duty_per;
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

extern PWM_Config_t *PWM_Config_1;
// initialize some random values
PWM_Config_1->pwm_number = 1;
PWM_Config_1->duty = 50; // should be the divided prescalar value

typedef struct _machine_pwm_obj_t
{
    mp_obj_base_t base;
    uint8_t num;
    pyb_pin_obj_t *pin; // create this specifically for pwm pins?
    uint8_t active;
    // uint8_t channel; -->We dont use different clock source for pwm
    // int32_t duty_ns; --> Kandipa thevaya?
    uint32_t duty;
    uint32_t freq;
    uint32_t period; // can take 32 bit values 0-(2^32 )-1;
    uint32_t value;  // dummy
    uint8_t pin_number;
    uint16_t prescalar_value; // minimum value of 2 --> So it takes the value as 1
    uint32_t deadband_delay;  // calculated on the fly
} machine_pwm_obj_t;

static bool pwm_inited = false;

/******************************************************************************/
// MicroPython bindings for PWM

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // the object created just now
    mp_printf(print, "PWM(%u", self->pin->phys_port);
    if (self->active)
    {
        mp_printf(print, ", freq=%u, duty=%u",
                  pwm_get_freq(self->channel), pwm_get_duty(self->channel));
    }
    mp_printf(print, ")");
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum
    {
        ARG_pwm_num,
        ARG_freq,
        ARG_duty,
        ARG_period
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_pwm_num, MP_ARG_INT, {.u_int = -1}},
        {MP_QSTR_prescalar, MP_ARG_INT, {.u_int = -1}},
        {MP_QSTR_freq, MP_ARG_INT, {.u_int = -1}},
        {MP_QSTR_duty, MP_ARG_INT, {.u_int = -1}},
        {MP_QSTR_period, MP_ARG_INT, {.u_int = -1}},
        // { MP_QSTR_duty_u16, MP_ARG_INT, {.u_int = -1} },
        // { MP_QSTR_duty_ns, MP_ARG_INT, {.u_int = -1} },
    };
    // make the pwm configuration object here
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_prescalar].u_int != -1)
    {
        // pwm_set_freq(args[ARG_freq].u_int, self->channel);
        // configuring the set frequency value
        // ok frequency is given
        if (args[ARG_prescalar].u_int > 2 | args[ARG_prescalar].u_int <= 32768)
        {
            self->prescalar_value = args[ARG_prescalar].u_int;
            prescalar=self->prescalar_value;
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
    if (args[ARG_freq].u_int != -1)
    {
        // pwm_set_freq(args[ARG_freq].u_int, self->channel);
        // configuring the set frequency value
        // ok frequency is given
        self->freq = args[ARG_freq].u_int;
        uint32_t period = (clk_freq) / ((prescalar)*self->freq);
        self->period = period;
    }
    if (args[ARG_duty].u_int != -1)
    {
        // pwm_set_duty(args[ARG_duty].u_int, self->channel);
        //I am getting them as percent
        duty_per=args[ARG_duty].u_int;
        duty= period*(args[ARG_duty].u_int)/100;
        self->duty=duty;
        //with respect to the period value
    }

    // pwm_start();
    PWM_Start(PWM_Config_t * config, args[ARG_pwm_num].u_int)
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    pyb_pin_obj_t *pin = mp_obj_get_pin_obj(args[0]);

    // create PWM object from the given pin
    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->pin = pin;
    self->active = 0; // setting it as low?

    // start the PWM subsystem if it's not already running
    if (!pwm_inited)
    {
      //pwm_init();
        PWM_Init(PWM_Config_1,1);
        //count of the pwm configuration
        pwm_inited = true;
        self->active = 1;
    }

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
    PWM_Stop(PWM_Config_1,pwm_num);
    PWM_Clear(PWM_Config_1);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self)
{
    //return MP_OBJ_NEW_SMALL_INT(pwm_get_freq(0));
    PWM_Show_Frequency(PWM_Config_1);

}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq)
{
    // pwm_set_freq(freq, 0);
    // we should set the prescalar value ig
    self->freq=freq;
    uint32_t period = (clk_freq) / ((prescalar)*self->freq);
    self->period =period;
    self->duty =(period)*duty_per;
    //getting the new duty value
    duty=self->duty;

}

static void set_active(machine_pwm_obj_t *self, bool set_pin)
{
    if (!self->active)
    {
        // something like calling the init function
        // pwm_add(self->pin->phys_port, self->pin->periph, self->pin->func);
        self->active = 1;
    }
}

static mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self)
{
    set_active(self, true);
    //return MP_OBJ_NEW_SMALL_INT(pwm_get_duty(self->channel));
    return self->duty;
    //gets the duty value wrt to the period value
    //if not accurate -> read from registers
}

static void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty)
{
    // set_active(self, false);
    // self->duty_ns = -1;
    // pwm_set_duty(duty, self->channel);
    PWM_Set_Duty_Cycle(PWM_Config_1);
    //whatever value is the config goes here
    PWM_Start(PWM_Config_1, args[ARG_pwm_num].u_int);
}
