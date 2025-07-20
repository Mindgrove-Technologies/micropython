/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
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
// extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.

#include "secure_iot.h"
//gives access to SDK-level system functions 
#include "py/obj.h"
#include "wdtimer.h"
#include "io.h"
#include "py/runtime.h"
//#include "dynruntime.h"

#define STATIC static


typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    //The base is how micropython tracks the different object types
    //MicroPython object type for the Watchdog Timer.
} machine_wdt_obj_t;

extern const mp_obj_type_t machine_wdt_type;
//just a forward declaration as micropython is wack
static machine_wdt_obj_t wdt_default = {{&machine_wdt_type}};

//function definitions:
static void mp_machine_wdt_feed(machine_wdt_obj_t *self);
static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms);
STATIC mp_obj_t mp_machine_wdt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_machine_wdt_make_new_instance_obj, 1, mp_machine_wdt_make_new_instance);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_wdt_feed_obj, mp_machine_wdt_feed);

//wdt_default is a static instance of this type.
//Returning &wdt_default ensures that this singleton instance is used and can be referenced from Python.

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms) { //enough to pass only timeout
    // The timeout on ESP8266 is fixed, so raise an exception if the argument is not the default.
    if (timeout_ms < 5000) {
        //timeout_ms: desired timeout (but ESP8266 uses a fixed 5s WDT).
        mp_raise_ValueError(NULL);
    }

    switch (id) {
        case 0:
            //ets_loop_dont_feed_sw_wdt = 1;
            wdtimer_disable();
            //disables automatic WDT feeding by the system task loop.
            //system_soft_wdt_feed();//actual SDK/API call to reset the watchdog.
            wdtimer_reset(); //should call start internally
            mp_printf(&mp_plat_print, "Watch dog timer instance created and fed\n");
            //immediately feeds the WDT (resets the counter).
            //Then returns the singleton WDT object.
            volatile uint16_t *control_reg1 = (uint16_t*) (CONTROL_REG_ADDR);
            volatile uint32_t *wd_cycles1 = (uint32_t*) (WD_CYCLES_REG_ADDR);
            // Number of cycles before triggering the watchdog interrupt/event
            volatile uint16_t *reset_cycles1 = (uint16_t*) (RESET_CYCLES_REG_ADDR);
            //uint32_t *new_reset=(uint32_t*)&timeout_ms;
            uint32_t new_reset=(uint32_t)(timeout_ms);
            wdtimer_start(*control_reg1,new_reset,*wd_cycles1);
            return &wdt_default; //similar to write in uart
        default:
            mp_raise_ValueError(NULL);
    }
}/*ets_loop_dont_feed_sw_wdt is checked inside the ESP8266 event/task loop.
If it is set to 0 (default), the system automatically feeds the software watchdog timer on every loop iteration.
If it is set to 1, the system stops feeding the WDT — meaning you must manually call system_soft_wdt_feed() to prevent a reset.*/
//Calls system_soft_wdt_feed() which tells the ESP8266 to reset the watchdog timer.

STATIC mp_obj_t mp_machine_wdt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);

    mp_int_t id = mp_obj_get_int(args[0]);
    mp_int_t timeout_ms = mp_obj_get_int(args[1]);

    return MP_OBJ_FROM_PTR(mp_machine_wdt_make_new_instance(id, timeout_ms));
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    (void)self;
    //system_soft_wdt_feed();
    wdtimer_reset();
    mp_printf(&mp_plat_print, "watch dog timer fed\n"); //for debug
    //wdtimer_disable();
    //for testing disable the wdt and do not enable it
    //cant test it in FPGA
}

    STATIC const mp_rom_map_elem_t machine_wdt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_machine_wdt_make_new_instance_obj) },
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&mp_machine_wdt_feed_obj) }
    };

    STATIC MP_DEFINE_CONST_DICT(mp_machine_wdt_locals_dict, machine_wdt_locals_dict_table);

    MP_DEFINE_CONST_OBJ_TYPE(
    machine_wdt_type,
    MP_QSTR_WDT,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_wdt_make_new,
    locals_dict, &mp_machine_wdt_locals_dict
    );

//static machine_wdt_obj_t wdt_default = {{&machine_wdt_type}};
