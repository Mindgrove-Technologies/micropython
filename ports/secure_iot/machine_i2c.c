#include "py/runtime.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "modmachine.h"
#include "i2c.h"
#include "secure_iot.h"
//for interrupt:
#include "io.h"
#include "log.h"
#include "plic.h"


#define NUM_I2C 2
#define clk_freq 80000000
extern const mp_obj_type_t machine_i2c_type;

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    uint8_t instance;
} machine_i2c_obj_t;

// Forward declarations
static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
static mp_obj_t machine_i2c_obj_init_helper(machine_i2c_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

const machine_i2c_obj_t machine_i2c_obj[NUM_I2C] = {
    {{&machine_i2c_type}, 0}, {{&machine_i2c_type}, 1},
};

static machine_i2c_obj_t *mp_obj_get_i2c_obj(mp_obj_t i2c_in) {
    if (mp_obj_is_type(i2c_in, &machine_i2c_type)) {
        return MP_OBJ_TO_PTR(i2c_in);
        //iF the direct object was passed as input
    }
    if (mp_obj_is_small_int(i2c_in)) {
        int i2c_inst = mp_obj_get_int(i2c_in);
        //extracting the integer from the micropython object
        if (i2c_inst >= 0 && i2c_inst < NUM_I2C) {
            return (machine_i2c_obj_t *)&machine_i2c_obj[i2c_inst];
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid instance"));
}

// Pin print
static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C instance(%u)", self->instance);
    //the print object
}

static mp_obj_t machine_i2c_obj_init_helper(machine_i2c_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_num};//,ARG_mode};
    //p is the created object so the pin number should take a valid number.
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_num, MP_ARG_REQUIRED | MP_ARG_INT ,{.u_int = 0}},
        //{ MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int num = args[ARG_num].u_int;
    I2C_Init(num,clk_freq);

    return mp_const_none;
}

static mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    //everything should happen with respect to the pin object
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    //atleast one argument
    machine_i2c_obj_t *i2c = mp_obj_get_i2c_obj(args[0]);//just in case
    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_i2c_obj_init_helper(i2c, n_args - 1, args + 1, &kw_args);
    }
    return mp_const_none;
}


static mp_obj_t machine_i2c_transmit_helper(machine_i2c_obj_t *self,size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){//const mp_obj_t *all_args) {
    enum { ARG_mode, ARG_len,ARG_addr,ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = 0} }, //default mode 0
        { MP_QSTR_len, MP_ARG_INT ,{.u_int = 8}}, //default 8
        { MP_QSTR_addr, MP_ARG_KW_ONLY|MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} }, // Accepts decimal or hex
        { MP_QSTR_data, MP_ARG_REQUIRED|MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args_parsed[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args_parsed);
    int mode = args_parsed[ARG_mode].u_int;
    uint8_t len = args_parsed[ARG_len].u_int;
    uint8_t addr=args_parsed[ARG_addr].u_int;
    mp_const_obj_t data =args_parsed[ARG_data].u_obj;
    // Set mode (replace with your hardware function)
    uint8_t mode_bits;
    if (mode == 0) { // Stop
        mode_bits=START_BIT|STOP_BIT;
    } 
    else if (mode == 1) { //Repeated start
        mode_bits=START_BIT|REPEATED_START;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
        mode_bits=START_BIT|STOP_BIT;
        //screw you Iam using the default one
    }
    uint8_t data_ptr=MP_OBJ_TO_PTR(data);
    //uint32_t I2C_Transmit(uint32_t instance_number,uint8_t slave_address,uint8_t *data,uint8_t length,uint8_t mode)
    int s=I2C_Transmit(self->instance,addr,data_ptr,len,mode_bits);
    return mp_const_none;
}

static mp_obj_t machine_i2c_recieve_helper(machine_i2c_obj_t *self,size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){//const mp_obj_t *all_args) {
    enum { ARG_mode, ARG_len,ARG_addr};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = 0} }, //default mode 0
        { MP_QSTR_len, MP_ARG_INT ,{.u_int = 8}}, //default 8
        { MP_QSTR_addr, MP_ARG_KW_ONLY|MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} }, // Accepts decimal or hex
        //{ MP_QSTR_data, MP_ARG_REQUIRED|MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        //fatal flaw
    };
    mp_arg_val_t args_parsed[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args_parsed);
    int mode = args_parsed[ARG_mode].u_int;
    uint8_t len = args_parsed[ARG_len].u_int;
    uint8_t addr=args_parsed[ARG_addr].u_int;
    mp_const_obj_t data=1;
    // Set mode (replace with your hardware function)
    uint8_t mode_bits;
    if (mode == 0) { // Stop
        mode_bits=START_BIT|STOP_BIT;
    } 
    else if (mode == 1) { //Repeated start
        mode_bits=START_BIT|REPEATED_START;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
        mode_bits=START_BIT|STOP_BIT;
        //screw you Iam using the default one
    }

    if(data){
        //if the mp_object is created
        uint8_t data_ptr=MP_OBJ_TO_PTR(data);
        //uint32_t I2C_Transmit(uint32_t instance_number,uint8_t slave_address,uint8_t *data,uint8_t length,uint8_t mode)
        int s=I2C_Recieve(self->instance,addr,data_ptr,len,mode_bits);
        for(int i=0;i<len;i++){
            printf("%c\t",data_ptr[i]);
            //if len not given use default value
        }
    }
    else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid data"));
    }
    return mp_const_none;
}



static mp_obj_t machine_i2c_recieve(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_i2c_recieve_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
    //no need to create an object from the instance number again
}

static MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_recieve_obj, 2, machine_i2c_recieve);

static mp_obj_t machine_i2c_transmit(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_i2c_transmit_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
    //no need to create an object from the instance number again
}

static MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_transmit_obj, 2, machine_i2c_transmit);

// Locals dict table
static const mp_rom_map_elem_t machine_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_transmit), MP_ROM_PTR(&machine_i2c_transmit_obj) },
    { MP_ROM_QSTR(MP_QSTR_recieve), MP_ROM_PTR(&machine_i2c_recieve_obj) },
    // Class constants
    { MP_ROM_QSTR(MP_QSTR_default_mode), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_default_len), MP_ROM_INT(8) },

};
static MP_DEFINE_CONST_DICT(machine_i2c_locals_dict, machine_i2c_locals_dict_table);
// static mp_stream_p_t i2c_stream_p = {
//     .read = machine_i2c_recieve,
//     .write = machine_i2c_transmit,
//     .//ioctl = mp_machine_uart_ioctl,
//     .is_text = false,
// };
// Pin type
MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    print, machine_i2c_print,
    //protocol, &i2c_stream_p,
    locals_dict, &machine_i2c_locals_dict
    );
