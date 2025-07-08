
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/virtpin.h"
//for making of the virtual pin object
#include "pin.h"
#include "extint.h"
#include "pin_defs_mind.h"

#define MAX_PIN_NUMBER 45
static bool pin_class_debug;
//A static boolean to control debug output for pin mapping.
//only for this particular file uhh


//Resets pin mapper and mapping dictionary to mp_const_none on boot.
void pin_init0(void) {
    //In pin_init0(), always reset:
    //during startup
    MP_STATE_PORT(pin_class_mapper) = mp_const_none;
    //defined in obj.h
    MP_STATE_PORT(pin_class_map_dict) = mp_const_none;
    pin_class_debug = false;
}

//fuck this shit
/*
const machine_pin_obj_t *pin_find(mp_obj_t user_obj) {
    const mp_print_t *print = &mp_plat_print; //ohh that struct
    //data is a pointer to a UART, a file, or a string buffer.
    //print_strn() is the actual function that prints len bytes from str to the desired output.
    const machine_pin_obj_t *pin_obj; //similar to the uart object that was created
    //mp_plat_print object is created in mpprint.h 
    //THe mp_plat_print object was that data + printfunction seen before --> reference to the cook function
    

    // If a pin was provided, then use it
    if (mp_obj_is_type(user_obj, &pin_type)

        ) {
        pin_obj = MP_OBJ_TO_PTR(user_obj);
        //assigning the value for the pin object
        if (pin_class_debug) {
            mp_printf(print, "Pin map passed pin ");
            mp_obj_print(MP_OBJ_FROM_PTR(pin_obj), PRINT_STR); //defined in obj.c
            //getting the ptr to that particular pin object structure
            mp_printf(print, "\n");
        }
        return pin_obj;
    }

    if (MP_STATE_PORT(pin_class_mapper) != mp_const_none) {
        mp_obj_t o = mp_call_function_1(MP_STATE_PORT(pin_class_mapper), user_obj);
        //for the particular mp_call_function_1 -> We are passing the function pointer and the arguments as object
        if (o != mp_const_none) {
            if (!mp_obj_is_type(o, &pin_type)) {
                mp_raise_ValueError(MP_ERROR_TEXT("Pin.mapper didn't return a Pin object"));
            }
            if (pin_class_debug) {
                mp_printf(print, "Pin.mapper maps ");
                //That struct with data and the function for the printing of that particular data
                mp_obj_print(user_obj, PRINT_REPR);
                mp_printf(print, " to ");
                mp_obj_print(o, PRINT_STR);
                mp_printf(print, "\n");
            }
            return MP_OBJ_TO_PTR(o);
        }
        // The pin mapping function returned mp_const_none, fall through to
        // other lookup methods.
    }

    if (MP_STATE_PORT(pin_class_map_dict) != mp_const_none) {
        mp_map_t *pin_map_map = mp_obj_dict_get_map(MP_STATE_PORT(pin_class_map_dict));
        //but there isnt any pointer for pin_class_map_dict right?
        mp_map_elem_t *elem = mp_map_lookup(pin_map_map, user_obj, MP_MAP_LOOKUP);
        //the kind is defined in obj.h
        //mp_map_elem_t *mp_map_lookup(mp_map_t *map, mp_obj_t index, mp_map_lookup_kind_t lookup_kind);
        //defined in obj.h as key and value
        if (elem != NULL && elem->value != MP_OBJ_NULL) {
            mp_obj_t o = elem->value;
            //getting the value of the key value pair
            if (pin_class_debug) {
                mp_printf(print, "Pin.map_dict maps ");
                mp_obj_print(user_obj, PRINT_REPR);//type 1 
                mp_printf(print, " to ");
                mp_obj_print(o, PRINT_STR); //type 0
                mp_printf(print, "\n");
            }
            return MP_OBJ_TO_PTR(o);
        }
    }
    //------------------------------------------------------------------------------------------
    // See if the pin name matches a board pin
    pin_obj = pin_find_named_pin(&machine_pin_board_pins_locals_dict, user_obj);
    //defined this already
    if (pin_obj) {
        if (pin_class_debug) {
            mp_printf(print, "Pin.board maps ");
            mp_obj_print(user_obj, PRINT_REPR); //object input and the kind of the printing required by thst object
            //of type 1 in obj.h file 
            mp_printf(print, " to ");
            mp_obj_print(MP_OBJ_FROM_PTR(pin_obj), PRINT_STR);
            //of type 0 in the obj.h
            mp_printf(print, "\n");
        }
        return pin_obj;
    }
    //WE ONLY HAVE ONE NAMING CONVENTION

    // See if the pin name matches a cpu pin
    // pin_obj = pin_find_named_pin(&machine_pin_cpu_pins_locals_dict, user_obj);
    // //called using the extern keyword ,but they are not defined anywhere
    // if (pin_obj) {
    //     if (pin_class_debug) {
    //         mp_printf(print, "Pin.cpu maps ");
    //         mp_obj_print(user_obj, PRINT_REPR);
    //         mp_printf(print, " to ");
    //         mp_obj_print(MP_OBJ_FROM_PTR(pin_obj), PRINT_STR);
    //         mp_printf(print, "\n");
    //     }
    //     return pin_obj;
    // }
    //---------------------------------------------------------------------------------------------
    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%s) doesn't exist"), mp_obj_str_get_str(user_obj));
}*/

/// \method __str__()
/// Return a string describing the pin object.

extern const machine_pin_obj_t machine_pins[];

// Minimal pin_find: only support integer pin numbers 0-44
const machine_pin_obj_t *pin_find(mp_obj_t user_obj) {
    if (!mp_obj_is_int(user_obj)) {
        mp_raise_TypeError(MP_ERROR_TEXT("Pin ID must be an integer"));
    }

    mp_int_t pin_num = mp_obj_get_int(user_obj);

    if (pin_num < 0 || pin_num > MAX_PIN_NUMBER) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid pin number (0–44 allowed)"));
    }

    return &machine_pins[pin_num];
}

static void pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // pin name
    mp_printf(print, "Pin(Pin.board.%q, mode=Pin.", self->name);

    uint32_t mode = pin_get_mode(self);

    if (mode <2 ) {
        // IO mode
        bool af = false;
        qstr mode_qst;
        if (mode == 1) {
            mode_qst = MP_QSTR_IN;}
            //why just why?
        else{
            mode_qst =MP_QSTR_OUT;
        }
        //to return string from a qstring object
        mp_print_str(print, qstr_str(mode_qst));

        // AF mode
        if (af) {
            mp_uint_t af_idx = pin_get_af(self);
            //en function uhhhhh
            const pin_af_obj_t *af_obj = pin_find_af_by_index(self, af_idx);
            if (af_obj == NULL) {
                mp_printf(print, ", alt=%d)", af_idx);
            } else {
                mp_printf(print, ", alt=Pin.%q)", af_obj->name);
                //printing the name of the alternate function
            }
        } else {
            mp_print_str(print, ")");
        }
    }
}

static mp_obj_t pin_obj_init_helper(const machine_pin_obj_t *pin, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);

/// \classmethod \constructor(id, ...)
/// Create a new Pin object associated with the id.  If additional arguments are given,
/// they are used to initialise the pin.  See `init`.
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Run an argument through the mapper and return the result.
    const machine_pin_obj_t *pin = pin_find(args[0]);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args); //definedcin map.c 
        //for allocating the required space for the created mapping 
        //The defined address and the size of the passed arguments
        pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(pin);
    //return the pin object
}
//---------------------------------------------------------------------------------------
// fast method for getting/setting pin value
static mp_obj_t pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (n_args == 0) {
        // get pin
        //uint32_t GPIO_Read_Data(uint32_t gpio_pin)
        uint32_t pin_read =GPIO_Read_Data(uint32_t self->pin);
        return MP_OBJ_NEW_SMALL_INT(pin_read);
        //return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
    } else {
        // set pin
        //mp_hal_pin_write(self, mp_obj_is_true(args[0]));
        //uint8_t GPIO_Pin_Set(uint8_t no_of_pins, ...);
        if(mp_obj_is_true(args[0])){
            uint8_t success =GPIO_Pin_Set(1,self->pin);
        }
        else{
            uint8_t success =GPIO_Pin_Clear(1,self->pin);
        }
        //to set the pins one by one
        return mp_const_none;
    }
} 
//0 arguments get pin ,or else set pin
//----------------------------------------------------------------------------------------------
#if MICROPY_PY_MACHINE_PIN_LEGACY

/// \classmethod mapper([fun])
/// Get or set the pin mapper function.
// static  mp_obj_t pin_mapper(size_t n_args, const mp_obj_t *args) {
//     if (n_args > 1) {
//         MP_STATE_PORT(pin_class_mapper) = args[1];
//         return mp_const_none;
//     }
//     return MP_STATE_PORT(pin_class_mapper);
// }
//pin_mapper: Set custom mapping function
//pin_map_dict: Use dictionary for pin aliases

// static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_mapper_fun_obj, 1, 2, pin_mapper);
// static MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_mapper_obj, MP_ROM_PTR(&pin_mapper_fun_obj));

/// \classmethod dict([dict])
/// Get or set the pin mapper dictionary.
static mp_obj_t pin_map_dict(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        MP_STATE_PORT(pin_class_map_dict) = args[1];
        return mp_const_none;
    }
    return MP_STATE_PORT(pin_class_map_dict);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_map_dict_fun_obj, 1, 2, pin_map_dict);
static MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_map_dict_obj, MP_ROM_PTR(&pin_map_dict_fun_obj));

/// \classmethod debug([state])
/// Get or set the debugging state (`True` or `False` for on or off).
static mp_obj_t pin_debug(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        pin_class_debug = mp_obj_is_true(args[1]);
        return mp_const_none;
    }
    return mp_obj_new_bool(pin_class_debug);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_debug_fun_obj, 1, 2, pin_debug);
static MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_debug_obj, MP_ROM_PTR(&pin_debug_fun_obj));

#endif // MICROPY_PY_MACHINE_PIN_LEGACY
//0------------------enum definition for getting the input value
// init(mode, pull=None, af=-1, *, value, alt)
static mp_obj_t pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_mode,
        #if MICROPY_PY_MACHINE_PIN_ALT_SUPPORT
        ARG_af,
        #endif
        ARG_value,
        #if MICROPY_PY_MACHINE_PIN_ALT_SUPPORT
        ARG_alt,
        #endif
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        //but in the actua python implementation it was as ip/op objects
        // #if MICROPY_PY_MACHINE_PIN_ALT_SUPPORT
        // { MP_QSTR_af, MP_ARG_INT, {.u_int = -1}}, // legacy
        // #endif
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        #if MICROPY_PY_MACHINE_PIN_ALT_SUPPORT
        { MP_QSTR_alt, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        //default value of -1
        #endif
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get io mode --> As an integer value
    uint mode = args[ARG_mode].u_int;
    if (!IS_GPIO_MODE(mode)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin mode: %d"), mode);
        //defined in runtime.c as an error that must be raised --> as an exception.
    }


    mp_int_t af = -1;
    #if MICROPY_PY_MACHINE_PIN_ALT_SUPPORT
    // get af (alternate function); alt-arg overrides af-arg
    af = args[ARG_alt].u_int;
    if (af == -1) {
        af = args[ARG_af].u_int;
    }
    if ((mode == GPIO_MODE_AF) && !IS_GPIO_AF(af)) {
        //To check if it is an alternate function or not
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin af: %d"), af);
    }
    #endif

    // if given, set the pin value before initialising to prevent glitches
    if (args[ARG_value].u_obj != MP_OBJ_NULL) {
        //write high or low value to the pin
        //mp_hal_pin_write(self, mp_obj_is_true(args[ARG_value].u_obj));
        //To check if its a valid micropython object to work with
        if ((args[ARG_value].u_obj)==0){
            //clear
            uint8_t ret=GPIO_Pin_Clear(1,self->pin);
            //gives us the pin number :)
        }
        if ((args[ARG_value].u_obj)==1){
            //clear
            uint8_t ret=GPIO_Pin_Set(1,self->pin);
            //gives us the pin number :)
        }
    }

    return mp_const_none;
}
//calling of the in itialization object using the helper functions
static mp_obj_t pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pin_obj_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pin_init_obj, 1, pin_obj_init);

/// \method value([value])
/// Get or set the digital logic level of the pin:
///
///   - With no argument, return 0 or 1 depending on the logic level of the pin.
///   - With `value` given, set the logic level of the pin.  `value` can be
///   anything that converts to a boolean.  If it converts to `True`, the pin
///   is set high, otherwise it is set low.
static mp_obj_t pin_value(size_t n_args, const mp_obj_t *args) {
    return pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_value_obj, 1, 2, pin_value);
//defined in obj.h

static mp_obj_t pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    //mp_hal_pin_low(self);
    uint8_t ret=GPIO_Pin_Clear(1,self->pin);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(pin_off_obj, pin_off);

static mp_obj_t pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    //mp_hal_pin_high(self);
    uint8_t ret=GPIO_Pin_Set(1,self->pin);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(pin_on_obj, pin_on);

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING, hard=False)
// Configure interrupts with triggers
//----------------------------------------------------------------------------------------------
static mp_obj_t pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    //Allows Python to attach an interrupt handler to a pin.
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = GPIO_MODE_IT_RISING | GPIO_MODE_IT_FALLING} },
        //they had addresses for this wth?
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
        //for setting it as hard real time?
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (n_args > 1 || kw_args->used != 0) {
        // configure irq
        extint_register_pin(self, args[ARG_trigger].u_int,
            args[ARG_hard].u_bool, args[ARG_handler].u_obj);
    }

    // TODO should return an IRQ object
    return mp_const_none;
}
//--------------------------------------------------------------------------------------
static MP_DEFINE_CONST_FUN_OBJ_KW(pin_irq_obj, 1, pin_irq);

// #if MICROPY_PY_MACHINE_PIN_LEGACY

// /// \method name()
// /// Get the pin name.
// static mp_obj_t pin_name(mp_obj_t self_in) {
//     machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
//     return MP_OBJ_NEW_QSTR(self->name);
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(pin_name_obj, pin_name);

// /// \method names()
// /// Returns the cpu and board names for this pin.
// static mp_obj_t pin_names(mp_obj_t self_in) {
//     machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
//     mp_obj_t result = mp_obj_new_list(0, NULL);
//     mp_obj_list_append(result, MP_OBJ_NEW_QSTR(self->name));

//     const mp_map_t *map = &machine_pin_board_pins_locals_dict.map;
//     mp_map_elem_t *elem = map->table;

//     for (mp_uint_t i = 0; i < map->used; i++, elem++) {
//         if (elem->value == self_in) {
//             mp_obj_list_append(result, elem->key);
//         }
//     }
//     return result;
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(pin_names_obj, pin_names);

// /// \classmethod af_list()
// /// Returns an array of alternate functions available for this pin.
// static mp_obj_t pin_af_list(mp_obj_t self_in) {
//     machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
//     mp_obj_t result = mp_obj_new_list(0, NULL);

//     const pin_af_obj_t *af = self->af;
//     for (mp_uint_t i = 0; i < self->num_af; i++, af++) {
//         mp_obj_list_append(result, MP_OBJ_FROM_PTR(af));
//     }
//     return result;
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(pin_af_list_obj, pin_af_list);

// /// \method port()
// /// Get the pin port.
// static mp_obj_t pin_port(mp_obj_t self_in) {
//     machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
//     return MP_OBJ_NEW_SMALL_INT(self->port);
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(pin_port_obj, pin_port);

// /// \method pin()
// /// Get the pin number.
// static mp_obj_t pin_pin(mp_obj_t self_in) {
//     machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
//     return MP_OBJ_NEW_SMALL_INT(self->pin);
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(pin_pin_obj, pin_pin);

// /// \method gpio()
// /// Returns the base address of the GPIO block associated with this pin.
// static mp_obj_t pin_gpio(mp_obj_t self_in) {
//     machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
//     return MP_OBJ_NEW_SMALL_INT((intptr_t)self->gpio);
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(pin_gpio_obj, pin_gpio);

// /// \method mode()
// /// Returns the currently configured mode of the pin. The integer returned
// /// will match one of the allowed constants for the mode argument to the init
// /// function.
// static mp_obj_t pin_mode(mp_obj_t self_in) {
//     return MP_OBJ_NEW_SMALL_INT(pin_get_mode(MP_OBJ_TO_PTR(self_in)));
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(pin_mode_obj, pin_mode);

// /// \method pull()
// /// Returns the currently configured pull of the pin. The integer returned
// /// will match one of the allowed constants for the pull argument to the init
// /// function.
// static mp_obj_t pin_pull(mp_obj_t self_in) {
//     return MP_OBJ_NEW_SMALL_INT(pin_get_pull(MP_OBJ_TO_PTR(self_in)));
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(pin_pull_obj, pin_pull);

// /// \method af()
// /// Returns the currently configured alternate-function of the pin. The
// /// integer returned will match one of the allowed constants for the af
// /// argument to the init function.
// static mp_obj_t pin_af(mp_obj_t self_in) {
//     return MP_OBJ_NEW_SMALL_INT(pin_get_af(MP_OBJ_TO_PTR(self_in)));
// }
// static MP_DEFINE_CONST_FUN_OBJ_1(pin_af_obj, pin_af);

// #endif // MICROPY_PY_MACHINE_PIN_LEGACY

static const mp_rom_map_elem_t pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),     MP_ROM_PTR(&pin_irq_obj) },

    // Legacy names as used by pyb.Pin
    { MP_ROM_QSTR(MP_QSTR_low),     MP_ROM_PTR(&pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),    MP_ROM_PTR(&pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_name),    MP_ROM_PTR(&pin_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_names),   MP_ROM_PTR(&pin_names_obj) },
    { MP_ROM_QSTR(MP_QSTR_af_list), MP_ROM_PTR(&pin_af_list_obj) },
    { MP_ROM_QSTR(MP_QSTR_port),    MP_ROM_PTR(&pin_port_obj) },
    { MP_ROM_QSTR(MP_QSTR_pin),     MP_ROM_PTR(&pin_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_gpio),    MP_ROM_PTR(&pin_gpio_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode),    MP_ROM_PTR(&pin_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_af),      MP_ROM_PTR(&pin_af_obj) },
    { MP_ROM_QSTR(MP_QSTR_debug),   MP_ROM_PTR(&pin_debug_obj) },

    // class attributes
    #if MICROPY_PY_MACHINE_PIN_BOARD_NUM_ENTRIES > 0
    { MP_ROM_QSTR(MP_QSTR_board),   MP_ROM_PTR(&pin_board_pins_obj_type) },
    #endif
    #if MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES > 0
    { MP_ROM_QSTR(MP_QSTR_cpu),     MP_ROM_PTR(&pin_cpu_pins_obj_type) },
    #endif

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN),        MP_ROM_INT(GPIO_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT),       MP_ROM_INT(GPIO_MODE_OUTPUT) },
    #if MICROPY_PY_MACHINE_PIN_ALT_SUPPORT
    { MP_ROM_QSTR(MP_QSTR_ALT),       MP_ROM_INT(GPIO_MODE_AF_PP) },
    { MP_ROM_QSTR(MP_QSTR_ALT_OPEN_DRAIN), MP_ROM_INT(GPIO_MODE_AF_OD) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_ANALOG),    MP_ROM_INT(GPIO_MODE_ANALOG) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP),   MP_ROM_INT(GPIO_PULLUP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULLDOWN) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(GPIO_MODE_IT_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(GPIO_MODE_IT_FALLING) },

};

static MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    switch (request) {
    //where is input/output defined for getting value?
        case MP_PIN_READ: {
            return mp_hal_pin_read(self);
        }
        case MP_PIN_WRITE: {
            mp_hal_pin_write(self, arg);
            return 0;
        }
    }
    return -1;
}

static const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
    print, pin_print,
    call, pin_call,
    protocol, &pin_pin_p,
    locals_dict, &pin_locals_dict
    );


/******************************************************************************/
// PinAF class

/// \moduleref pyb
/// \class PinAF - Pin Alternate Functions
///
/// A Pin represents a physical pin on the microcprocessor. Each pin
/// can have a variety of functions (GPIO, I2C SDA, etc). Each PinAF
/// object represents a particular function for a pin.
///
/// Usage Model:
///
///     x3 = pyb.Pin.board.X3
///     x3_af = x3.af_list()
///
/// x3_af will now contain an array of PinAF objects which are available on
/// pin X3.
///
/// For the pyboard, x3_af would contain:
///     [Pin.AF1_TIM2, Pin.AF2_TIM5, Pin.AF3_TIM9, Pin.AF7_USART2]
///
/// Normally, each peripheral would configure the af automatically, but sometimes
/// the same function is available on multiple pins, and having more control
/// is desired.
///
/// To configure X3 to expose TIM2_CH3, you could use:
///    pin = pyb.Pin(pyb.Pin.board.X3, mode=pyb.Pin.AF_PP, alt=pyb.Pin.AF1_TIM2)
/// or:
///    pin = pyb.Pin(pyb.Pin.board.X3, mode=pyb.Pin.AF_PP, alt=1)

/// \method __str__()
/// Return a string describing the alternate function.
static void pin_af_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_af_obj_t *self = MP_OBJ_TO_PTR(self_in);
    //To get the name of the pin 
    //but what exactly is the kind of print?
    mp_printf(print, "Pin.%q", self->name);
}

/// \method index()
/// Return the alternate function index.
static mp_obj_t pin_af_index(mp_obj_t self_in) {
    pin_af_obj_t *af = MP_OBJ_TO_PTR(self_in);
    //from the stuct that was already defined
    return MP_OBJ_NEW_SMALL_INT(af->idx); //get the index of the a;ternate function
}
static MP_DEFINE_CONST_FUN_OBJ_1(pin_af_index_obj, pin_af_index);

/// \method name()
/// Return the name of the alternate function.
static mp_obj_t pin_af_name(mp_obj_t self_in) {
    pin_af_obj_t *af = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_QSTR(af->name);
}
static MP_DEFINE_CONST_FUN_OBJ_1(pin_af_name_obj, pin_af_name);

/// \method reg()
/// Return the base register associated with the peripheral assigned to this
/// alternate function. For example, if the alternate function were TIM2_CH3
/// this would return stm.TIM2
static mp_obj_t pin_af_reg(mp_obj_t self_in) {
    pin_af_obj_t *af = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT((uintptr_t)af->reg);
}
static MP_DEFINE_CONST_FUN_OBJ_1(pin_af_reg_obj, pin_af_reg);

static const mp_rom_map_elem_t pin_af_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_index), MP_ROM_PTR(&pin_af_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_name), MP_ROM_PTR(&pin_af_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_reg), MP_ROM_PTR(&pin_af_reg_obj) },
};
static MP_DEFINE_CONST_DICT(pin_af_locals_dict, pin_af_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    pin_af_type,
    MP_QSTR_PinAF,
    MP_TYPE_FLAG_NONE,
    print, pin_af_obj_print,
    locals_dict, &pin_af_locals_dict
    );

MP_REGISTER_ROOT_POINTER(mp_obj_t pin_class_mapper);
//This a weird variable declaration
MP_REGISTER_ROOT_POINTER(mp_obj_t pin_class_map_dict);
//Thsi is also a weird variable declaration
//These don’t define actual variables — they register global/static pointers with the MicroPython garbage collector (GC).
// Global pointers to mp_obj_t ->To avoid the freeing of these objects.
//creates a struct as  _mp_state_port_t
//It gets included to  struct _mp_state_ctx_t
//MicroPython's GC works by scanning a fixed list of root pointers during collection. If an object isn’t reachable from this list, it's freed.
//in runtime . h these structs are added at runtime (that .vm, .port)
//When the garbage collector runs, it will scan MP_STATE_PORT(...) fields for mp_obj_t references.