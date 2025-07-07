#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
//#include "pin.h"
//#include "pin_defs_mind.h"
// Define the number of GPIO pins available on your MCU
#define NUM_PINS 16
extern const mp_obj_type_t machine_pin_type;

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t pin; // Physical pin number
} machine_pin_obj_t;

// Forward declarations
static mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);

// Pin object array (adjust NUM_PINS as needed)
const machine_pin_obj_t machine_pin_obj[NUM_PINS] = {
    {{&machine_pin_type}, 0}, {{&machine_pin_type}, 1}, {{&machine_pin_type}, 2}, {{&machine_pin_type}, 3},
    {{&machine_pin_type}, 4}, {{&machine_pin_type}, 5}, {{&machine_pin_type}, 6}, {{&machine_pin_type}, 7},
    {{&machine_pin_type}, 8}, {{&machine_pin_type}, 9}, {{&machine_pin_type},10}, {{&machine_pin_type},11},
    {{&machine_pin_type},12}, {{&machine_pin_type},13}, {{&machine_pin_type},14}, {{&machine_pin_type},15},
};

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
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int mode = args[ARG_mode].u_int;
    int value = (args[ARG_value].u_obj == MP_OBJ_NULL) ? -1 : mp_obj_is_true(args[ARG_value].u_obj);

    // Set mode (replace with your hardware function)
    if (mode == 0) { // Input
        //mp_hal_pin_input(self->pin_num);
        GPIO_Config(1,1,self->pin);
    } else if (mode == 1) { // Output
        //mp_hal_pin_output(self->pin_num);
        GPIO_Config(0,1,self->pin);
        if (value != -1) {
            //mp_hal_pin_write(self->pin_num, value);
            if(mp_obj_is_true(value)){
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
    // Class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(1) },
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
