#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "gpio.h"
#include "secure_iot.h"
//for interrupt:
#include "io.h"
#include "log.h"
#include "plic.h"
#include "gptimer.h"
//#include "pin.h"
//#include "pin_defs_mind.h"
// Define the number of GPIO pins available on your MCU
#define NUM_PINS 16
#define MAX_GPIO_PINS 44
//As the count goes from 0-44
extern const mp_obj_type_t machine_pin_type;

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t pin; // Physical pin number
} machine_pin_obj_t;

// Forward declarations
static mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);

// Pin object array (adjust NUM_PINS as needed)
//probably as possible alternate function and if true or false can be added to the pin object?
const machine_pin_obj_t machine_pin_obj[NUM_PINS] = {
    {{&machine_pin_type}, 0}, {{&machine_pin_type}, 1}, {{&machine_pin_type}, 2}, {{&machine_pin_type}, 3},
    {{&machine_pin_type}, 4}, {{&machine_pin_type}, 5}, {{&machine_pin_type}, 6}, {{&machine_pin_type}, 7},
    {{&machine_pin_type}, 8}, {{&machine_pin_type}, 9}, {{&machine_pin_type},10}, {{&machine_pin_type},11},
    {{&machine_pin_type},12}, {{&machine_pin_type},13}, {{&machine_pin_type},14}, {{&machine_pin_type},15},
};

void handle_button_press(void*arg)
{
    int num = (int)((int*)(arg));
	printf("Button pressed is %d\n",num);
    printf("ehhh");
	GPT_Delay_Millisecs_H(100); // delay to adjust debounce time for button interrupt. Comment this if not needed.

}

void handle_button_release(void*arg)
{
    int num = (int)((int*)(arg));
	printf("Button released is %d\n",num);
	GPT_Delay_Millisecs_H(100); // delay to adjust debounce time for button interrupt. Comment this if not needed.

}
// Helper to get pin object from Python argument
static machine_pin_obj_t *mp_obj_get_pin_obj(mp_obj_t pin_in) {
    if (mp_obj_is_type(pin_in, &machine_pin_type)) {
        return MP_OBJ_TO_PTR(pin_in);
    }
    if (mp_obj_is_small_int(pin_in)) {
        int pin = mp_obj_get_int(pin_in);
        if (pin >= 0 && pin < NUM_PINS) {
            return (machine_pin_obj_t *)&machine_pin_obj[pin];
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
}

// Pin print
static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Pin(%u)", self->pin);
}

// Pin init: pin.init(mode, *, value)
static mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_value };
    //p is the created object so the pin number should take a valid number.
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int mode = args[ARG_mode].u_int;
    int value = (args[ARG_value].u_obj == MP_OBJ_NULL) ? -1 : mp_obj_is_true(args[ARG_value].u_obj);
    if (self == NULL || self->pin >= MAX_GPIO_PINS) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin object"));
    }

    // Set mode (replace with your hardware function)
    if (mode == 0) { // Input
        //mp_hal_pin_input(self->pin_num);
        GPIO_Config(0,1,self->pin);
    } else if (mode == 1) { // Output
        //mp_hal_pin_output(self->pin_num);
        GPIO_Config(1,1,self->pin);
        if (value != -1) {
            //mp_hal_pin_write(self->pin_num, value);
            if((value)){
                uint8_t success =GPIO_Pin_Set(1,self->pin);
            }
            else{
                uint8_t success =GPIO_Pin_Clear(1,self->pin);
            }
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
    }

    return mp_const_none;
}

// Pin constructor: Pin(id, ...)
static mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    machine_pin_obj_t *pin = mp_obj_get_pin_obj(args[0]);
    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }
    return MP_OBJ_FROM_PTR(pin);
}

// Pin call: p() or p(x)
static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (n_args == 0) {
        // get value
        //return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self->pin_num));
        //uint32_t pin_read =GPIO_Read_Data((uint32_t) self->pin);
        uint32_t pin_read =GPIO_Read_Pin_Status((uint32_t) self->pin);
        return MP_OBJ_NEW_SMALL_INT(pin_read);
    } else {
        // set value
        //mp_hal_pin_write(self->pin_num, mp_obj_is_true(args[0]));
        if(mp_obj_is_true(args[0])){
            uint8_t success =GPIO_Pin_Set(1,self->pin);
        }
        else{
            uint8_t success =GPIO_Pin_Clear(1,self->pin);
        }
        return mp_const_none;
    }
}
/*For the interrupt logic:
def callback(pin):
    print("Interrupt on", pin)
p.irq(handler=callback, trigger=Pin.IRQ_RISING)
If the handler is not provided we have to assign the default handler to the interrupt
*/
static mp_obj_t machine_pin_interrupt_helper(machine_pin_obj_t *self,size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){//const mp_obj_t *all_args) {

    //mp_arg_check_num(n_args, n_kw, 1, 2, false);
    //machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    //machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    enum { ARG_trigger, ARG_handler };
    //p is the created object so the pin number should take a valid number.
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_trigger, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_handler, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        //2nd one is a maybe
    };
    mp_arg_val_t args_parsed[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args_parsed);
    //--------------------------------------------------------------------------------------------------------
    //     printf("Positional args (%d):\n", (int)n_args);
    // for (size_t i = 0; i < n_args; ++i) {
    //     mp_obj_print_helper(&mp_plat_print, args_parsed[i], PRINT_REPR);
    //     printf("\n");
    // }
    /*
    for (size_t i = 0; i < MP_ARRAY_SIZE(allowed_args); ++i) {
    if (allowed_args[i].flags & MP_ARG_INT) {
        printf("arg[%d] = %d\n", (int)i, args_parsed[i].u_int);
    } else if (allowed_args[i].flags & MP_ARG_OBJ) {
        printf("arg[%d] = ", (int)i);
        mp_obj_print_helper(&mp_plat_print, args_parsed[i].u_obj, PRINT_REPR);
        printf("\n");
    }
    }


    if (kw_args != NULL && kw_args->used > 0) {
        printf("Keyword args:\n");
    for (size_t i = 0; i < kw_args->alloc; ++i) {
        if (mp_map_slot_is_filled(kw_args, i)) {
            mp_map_elem_t *elem = &kw_args->table[i];
            mp_obj_print_helper(&mp_plat_print, elem->key, PRINT_REPR);
            printf(" : ");
            mp_obj_print_helper(&mp_plat_print, elem->value, PRINT_REPR);
            printf("\n");
        }
    }
}*/
//-----------------------------------------------------------------------
    //mp_map_t kw_args;
    //mp_map_init_fixed_table(&kw_args,n_kw,all_args+n_args);
    //mp_arg_parse_all(n_args, all_args, &kw_args,MP_ARRAY_SIZE(allowed_args), allowed_args, args_parsed);
    int trigger = args_parsed[ARG_trigger].u_int;
    printf("trigger ,%d",trigger);
    uint8_t s =PLIC_Init();
    if(s==0){
        printf("Plic initialized");
    }
    //int  = (args[ARG_value].u_obj == MP_OBJ_NULL) ? -1 : mp_obj_is_true(args[ARG_value].u_obj);
    if (self == NULL || self->pin >= MAX_GPIO_PINS) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin object"));
    }
    //if (n_args == 2 | n_args==1) {
        // use default handler
        //if (trigger ==0 | trigger==1){
        if(1){
            int num=self->pin;
            printf("pin number ,%d",num);
            num=1; //could be corrupted
            //the pin number
            GPIO_Interrupt_Config(num,0);
            PLIC_Config_t plic_config;
            //plic config structure
            
            plic_config.interrupt_id=GPIO0_IRQn+num;
            if(trigger ==0){
                plic_config.fptr=handle_button_press;
                //handler same for button press and button release now
            }
            if(trigger==1){
                plic_config.fptr=handle_button_press;
                //handler same for button press and button release now
            }
            plic_config.fptr=handle_button_press; //the issue could be with handler
            interrupt_arg[GPIO0_IRQn+num]=(void *)num;/*To pass argument to ISR function (comment this if not required)*/
            plic_config.priority_value=PLIC_PRIORITY_3;/*Priority for this ISR function*/
            IRQ_Connect(&plic_config);//set the corresponding isr for interrupt id 6
            printf("Completed IRQ Connect\n\r");
        }
    else {
        mp_const_obj_t handler;
        // Use a custom handler
        if (args_parsed[ARG_handler].u_obj != mp_const_none) {
                handler = args_parsed[ARG_handler].u_obj;
        }
        else{
            if(trigger ==0){
                handler=handle_button_press;
                //handler same for button press and button release now
            }
            if(trigger==1){
                handler=handle_button_press;
                //handler same for button press and button release now
            }
        }
        if (trigger ==0 || trigger==1){
            int num=self->pin;
            //the pin number
            GPIO_Interrupt_Config(num,trigger);
            PLIC_Config_t plic_config;
            //plic config structure
            plic_config.interrupt_id=GPIO0_IRQn+num;
            plic_config.fptr=handler;
            interrupt_arg[GPIO0_IRQn+num]=(void *)num;/*To pass argument to ISR function (comment this if not required)*/
            plic_config.priority_value=PLIC_PRIORITY_3;/*Priority for this ISR function*/
            IRQ_Connect(&plic_config);//set the corresponding isr for interrupt id 6
            printf("Completed IRQ Connect\n\r");
    }
    }
    return mp_const_none;
}
/*
static mp_obj_t machine_pin_interrupt(size_t n_args, size_t n_kw, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    enum { ARG_trigger, ARG_handler };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_trigger, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_handler, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args_parsed[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args_parsed);

    int trigger = args_parsed[ARG_trigger].u_int;
    mp_obj_t handler = args_parsed[ARG_handler].u_obj;

    // Debug print
    printf("Pin: %d, Trigger: %d, Handler: %p\n", self->pin, trigger, handler);

    if (trigger != 0 && trigger != 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid trigger value"));
    }

    int num = self->pin;
    GPIO_Interrupt_Config(num, trigger);

    PLIC_Config_t plic_config;
    plic_config.interrupt_id = GPIO0_IRQn + num;
    plic_config.priority_value = PLIC_PRIORITY_3;
    interrupt_arg[GPIO0_IRQn + num] = (void *)num;

    // Handler logic
    // if (handler != MP_OBJ_NULL && handler != mp_const_none) {
    //     // If you want to call a Python handler, you need a C trampoline!
    //     plic_config.fptr = c_trampoline_for_python_handler;
    //     saved_python_handler[num] = handler; // Save handler for later
    // } else {
    {
        plic_config.fptr = handle_button_press;
    }

    IRQ_Connect(&plic_config);
    printf("Completed IRQ Connect\n\r");

    return mp_const_none;
}*/

//static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_interrupt_obj, 2, 3, machine_pin_interrupt);
//This supports only positional arguments

static mp_obj_t machine_pin_interrupt(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_interrupt_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}

static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_interrupt_obj, 1, machine_pin_interrupt);

// Pin.value([value])
static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// Pin.on()
static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    //mp_hal_pin_write(self->pin_num, 1);
    uint8_t success =GPIO_Pin_Set(1,self->pin);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

// Pin.off()
static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    //mp_hal_pin_write(self->pin_num, 0);
    uint8_t success =GPIO_Pin_Clear(1,self->pin);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

// Pin.init(mode, *, value)
static mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

// Locals dict table
static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_pin_interrupt_obj) },
    // Class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_LOW), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_HIGH), MP_ROM_INT(1) },
    // So low and high has to point to the corresponding values of 0 and 1 
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

// Pin type
MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_make_new,
    print, machine_pin_print,
    call, machine_pin_call,
    locals_dict, &machine_pin_locals_dict
    );
