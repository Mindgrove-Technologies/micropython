/*typedef struct 
{
    uint32_t gpt_num;
    uint32_t mode;
    uint32_t interrupt_en;
    uint32_t period;
    uint32_t prescalar;
    uint32_t dutycycle;
    uint32_t cnt_en;
    // The parameter capture_val is an unsigned integer that sets the input value to be captured
    uint32_t capture_val;
    // The parameter output_en is an unsigned integer that sets the output_enable high in the control and status register
    uint32_t output_en;
}GPTIMER_Config_t;*/
#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "gpio.h"
#include "secure_iot.h"
//for interrupt: -> Have to setup timer based interrupt.
#include "io.h"
#include "log.h"
#include "plic.h"
#include "gptimer.h"
#define STATIC static
#define TIMER_NUM 3
#define clk_freq 30000000
#define prescale_initial 30
// GPTIMER_Config_t gptimer_b;
// GPTIMER_Config_t *GPTIMER_Config_1 = &gptimer_b;
//GPTIMER_Config_1->gpt_num = 0;  // This is not valid
//GPTIMER_Config_t* gptimer_array[TIMER_NUM];

extern const mp_obj_type_t machine_timer_type;

GPTIMER_Config_t gptimer_inst0={
    .gpt_num=0,
    .mode=0,
    .interrupt_en=0,
    .period=1000,
    .prescalar=prescale_initial,
    .dutycycle=128,
    .cnt_en=0,
    .capture_val=0,
    .output_en=1
};

GPTIMER_Config_t gptimer_inst1={
    .gpt_num=1,
    .mode=0,
    .interrupt_en=0,
    .period=1000,
    .prescalar=prescale_initial,
    .dutycycle=128,
    .cnt_en=0,
    .capture_val=0,
    .output_en=1
};

GPTIMER_Config_t gptimer_inst2={
    .gpt_num=2,
    .mode=0,
    .interrupt_en=0,
    .period=1000,
    .prescalar=prescale_initial,
    .dutycycle=128,
    .cnt_en=0,
    .capture_val=0,
    .output_en=1
};
GPTIMER_Config_t* gptimer_array[TIMER_NUM]={
    &gptimer_inst0,
    &gptimer_inst1,
    &gptimer_inst2,
};

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    bool periodic;
    mp_obj_t callback; //call back defined here itself
    uint32_t prescalar;
    uint32_t duty;
    uint32_t period;
    uint32_t capture;
} machine_timer_obj_t;

const machine_timer_obj_t machine_timer_obj[TIMER_NUM] = {
    {{&machine_timer_type}, 0,0,mp_const_none,prescale_initial,128,256,0}, {{&machine_timer_type}, 1,0,mp_const_none,prescale_initial,128,256,0}, {{&machine_timer_type}, 2,0,mp_const_none,prescale_initial,128,256,0}
};//No need to create separate handlers array.


// Timer info print
static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Timer(%u), prescalar: %u, duty %u, period %u", self->id,self->prescalar,self->duty,self->period);
}

// Timer.init(period=1000, mode=Timer.PERIODIC, callback=cb)
STATIC mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self,size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    //machine_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    //already passed in as input
    enum { ARG_period, ARG_mode,ARG_prescalar,ARG_duty,ARG_cnt_en,ARG_capture,ARG_int_en };
    static const mp_arg_t allowed_args[] = {
        //Instance number of the GPtimer
        { MP_QSTR_period, MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = 1} }, // 00->pwm 01-> up 10->down 11->up down
        //{ MP_QSTR_callback, MP_ARG_OBJ, {.u_obj = mp_const_none} }, --> Not here 
        {MP_QSTR_prescalar,MP_ARG_INT,{.u_int=256}},
        {MP_QSTR_duty,MP_ARG_INT,{.u_int=500}},//initialized to 50 percent duty cycle.
        {MP_QSTR_cnt_en,MP_ARG_BOOL,{.u_bool=0}},
        {MP_QSTR_capture,MP_ARG_INT,{.u_int=0x00}},
        {MP_QSTR_int_en,MP_ARG_INT,{.u_int=0}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    //mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed);
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if (self == NULL || self->id >= TIMER_NUM) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid timer object"));
    }
    //adding period
    gptimer_array[self->id]->period =args[ARG_period].u_int;
    self->period=args[ARG_period].u_int;
    //adding duty
    self->duty=args[ARG_duty].u_int;
    gptimer_array[self->id]->dutycycle=self->duty;
    gptimer_array[self->id]->mode = args[ARG_mode].u_int; //default value 1->up counter
    //self->callback = parsed[ARG_callback].u_obj;
    // Here you would start/configure the hardware timer using self->id and parsed[ARG_period].u_int
    switch (args[ARG_int_en].u_int){
        case 1:
            gptimer_array[self->id]->interrupt_en=1;
            break;
        case 0:
            gptimer_array[self->id]->interrupt_en=0;
            break;
        default:
            gptimer_array[self->id]->interrupt_en=0;
            break;
    }
    //adding prescalar value ->with default value
    self->prescalar=args[ARG_prescalar].u_int;
    gptimer_array[self->id]->prescalar=self->prescalar;

    //continuous enable :/
    self->periodic=args[ARG_cnt_en].u_int;
    gptimer_array[self->id]->cnt_en=self->periodic;

    //capture
    self->capture=args[ARG_capture].u_int;
    gptimer_array[self->id]->capture_val=self->capture;

    GPT_Init(gptimer_array[self->id]);
    //sets the period and the prescalar ,dutycycle


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init_helper);

static machine_timer_obj_t *mp_obj_get_timer_obj(mp_obj_t time_in) {

        uint8_t num = mp_obj_get_int(time_in);
        if (num >= 0 && num < TIMER_NUM) {
            return (machine_timer_obj_t *)&machine_timer_obj[num];
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid timer number"));
}

// Timer constructor
STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1,  MP_OBJ_FUN_ARGS_MAX, false);//what does the boolean value signify
    //machine_timer_obj_t *self = m_new_obj(machine_timer_obj_t);
    //This is ok if there is only one object or its very difficult to keep track of that object
    machine_timer_obj_t *timer = mp_obj_get_timer_obj(args[0]);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_timer_init_helper(timer, n_args - 1, args + 1, &kw_args);
    return MP_OBJ_FROM_PTR(timer);
    //Important to return object or it treats it as a null type
}

static mp_obj_t machine_timer_reset(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t success =GPT_Reset(gptimer_array[self->id]);
    printf("Reset the GPTimer");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_reset_obj, machine_timer_reset);
// Timer locals dict
STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_timer_reset_obj) },
    // Add more methods as needed
};
STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

// Timer type definition
// const mp_obj_type_t machine_timer_type = {
//     { &mp_type_type },
//     .name = MP_QSTR_Timer,
//     .make_new = machine_timer_make_new,
//     .locals_dict = (mp_obj_dict_t*)&machine_timer_locals_dict,
// };

MP_DEFINE_CONST_OBJ_TYPE(
    machine_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
    print, machine_pin_print,
    //protocol, &machine_spi_p,
    locals_dict, &machine_timer_locals_dict
    );


