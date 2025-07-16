#include "py/mphal.h"
#include "py/obj.h"
#include "modmachine.h"
#include "py/runtime.h"
#include "spi.h"
#define SPI_NUM 5
#define clk_freq 80000000
#define bit_rate_from_prescale(clk_freq, prescale) (((clk_freq) / (prescale)) - 1)//justin case uhh
#define bit_rate_from_prescale(prescale) (((clk_freq) / (prescale)) - 1)
#define prescale_from_bit_rate(clk_freq,bitrate) ((clk_freq)/((bitrate)-1))
#define prescale_from_bit_rate(bitrate) ((clk_freq)/((bitrate)-1))
#define setup(bitrate) ((1/10)*(1/(bitrate)))
#define hold(bitrate) ((1/10)*(1/(bitrate)))
/*
typedef struct 
{
    int spi_number;
    int pol;
    int pha;
    int prescale;//Sets the prescaler value to adjust the SPI clock speed. A lower value results in a higher speed.
    int setup_time;
    int hold_time;
    uint32_t bit_rate;
    int master_mode;
    int lsb_first;
    int comm_mode;
    int spi_size;
    uint32_t value;
    int bits;
}SPI_Config_t;*/
//SPI_Config_t* spi_array[SPI_NUM];
//Life is hard ,code is messy and I am sad
//SPI_Config_t* spi_b;
SPI_Config_t spi_b_inst0={
    .spi_number=0,
    .pol=0,
    .pha=0,
    .prescale=256,
    .bit_rate=bit_rate_from_prescale(256),
    .setup_time=setup(bit_rate_from_prescale(256)),
    .hold_time=hold(bit_rate_from_prescale(256)),
    .master_mode=0,
    .lsb_first=0,
    .comm_mode=3,
    .spi_size=8,
    .value=0,
    .bits=8,
};
//spi_b =&spi_b_inst0;
//spi_array[0]=spi_b;
SPI_Config_t spi_b_inst1={
    .spi_number=1,
    .pol=0,
    .pha=0,
    .prescale=256,
    .bit_rate=bit_rate_from_prescale(256),
    .setup_time=setup(bit_rate_from_prescale(256)),
    .hold_time=hold(bit_rate_from_prescale(256)),
    .master_mode=0,
    .lsb_first=0,
    .comm_mode=3,
    .spi_size=8,
    .value=0,
    .bits=8,
};
//spi_b =&spi_b_inst1;
//spi_array[1]=spi_b;
SPI_Config_t spi_b_inst2={
    .spi_number=2,
    .pol=0,
    .pha=0,
    .prescale=256,
    .bit_rate=bit_rate_from_prescale(256),
    .setup_time=setup(bit_rate_from_prescale(256)),
    .hold_time=hold(bit_rate_from_prescale(256)),
    .master_mode=0,
    .lsb_first=0,
    .comm_mode=3,
    .spi_size=8,
    .value=0,
    .bits=8,
};
//spi_b =&spi_b_inst2;
//spi_array[2]=spi_b;
SPI_Config_t spi_b_inst3={
    .spi_number=3,
    .pol=0,
    .pha=0,
    .prescale=256,
    .bit_rate=bit_rate_from_prescale(256),
    .setup_time=setup(bit_rate_from_prescale(256)),
    .hold_time=hold(bit_rate_from_prescale(256)),
    .master_mode=0,
    .lsb_first=0,
    .comm_mode=3,
    .spi_size=8,
    .value=0,
    .bits=8,
};
//spi_b =&spi_b_inst3;
//spi_array[3]=spi_b;
SPI_Config_t spi_b_inst4={
    .spi_number=4,
    .pol=0,
    .pha=0,
    .prescale=256,
    .bit_rate=bit_rate_from_prescale(256),
    .setup_time=setup(bit_rate_from_prescale(256)),
    .hold_time=hold(bit_rate_from_prescale(256)),
    .master_mode=0,
    .lsb_first=0,
    .comm_mode=3,
    .spi_size=8,
    .value=0,
    .bits=8,
};
//spi_b =&spi_b_inst4;
//spi_array[4]=spi_b;
SPI_Config_t* spi_array[SPI_NUM] = {
    &spi_b_inst0,
    &spi_b_inst1,
    &spi_b_inst2,
    &spi_b_inst3,
    &spi_b_inst4
};
//my life saver definition
extern const mp_obj_type_t machine_spi_type;
//----------------------------------------------------------------------------------------------------
//there are 4 spis.
//tbh a lot of stuff is redundant here and I might need to remove.
//But I would need a spi_object also
typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    uint8_t spi_id; // 0, 1, 2 3, etc.
    uint32_t prescalar;
    uint32_t bit_rate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t master_mode; //default it is master mode
} machine_spi_obj_t;

const machine_spi_obj_t machine_spi_obj[SPI_NUM] = {
    {{&machine_spi_type}, 0,256,bit_rate_from_prescale(256),0,0,0}, {{&machine_spi_type}, 1,256,bit_rate_from_prescale(256),0,0,0},
    {{&machine_spi_type}, 2,256,bit_rate_from_prescale(256),0,0,0},{{&machine_spi_type}, 3,256,bit_rate_from_prescale(256),0,0,0},
    {{&machine_spi_type}, 4,256,bit_rate_from_prescale(256),0,0,0}
};
//helper to map to the spi object
static machine_spi_obj_t *mp_obj_get_spi_obj(mp_obj_t spi_in) {
    // if (mp_obj_is_type(pin_in, &machine_pin_type)) {
    //     return MP_OBJ_TO_PTR(pin_in);
    // }
    if (mp_obj_is_small_int(spi_in)) {
        int spi_instance = mp_obj_get_int(spi_in);
        if (spi_instance>= 0 && spi_instance< SPI_NUM) {
            return (machine_spi_obj_t *)&machine_spi_obj[spi_instance];
            //for ever mapped to that object ,till the end of time <3
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid spi instance"));
}
//can use that alternative function table I had created :)
//actual transfer function for spi:
static mp_obj_t machine_spi_transmit_helper(machine_spi_obj_t *self,size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){
    enum { ARG_mode, ARG_size,ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = 3} }, //default mode 0
        { MP_QSTR_size, MP_ARG_INT ,{.u_int = 8}}, //default 8
        { MP_QSTR_data,  MP_ARG_REQUIRED|MP_ARG_KW_ONLY |  MP_ARG_INT, {.u_int = 0} },
        //not data pointer like in i2c its the actual data
    };
    mp_arg_val_t args_parsed[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args_parsed);
    int mode = args_parsed[ARG_mode].u_int;
    //ok so we have the mode value
    int spi_id =self->spi_id;
    if(mode <=3 && mode>=0){
        spi_array[spi_id]->comm_mode=mode;
        //else no change
    }
    uint8_t len = args_parsed[ARG_size].u_int;//else takes 8    uint8_t addr=args_parsed[ARG_addr].u_int;
    // Set mode (replace with your hardware function)
    if(len==8){
        uint8_t data =args_parsed[ARG_data].u_int;
        int s =SPI8_Transmit_Data(spi_array[spi_id], data);
    }
    else if(len==16){
        uint16_t data =args_parsed[ARG_data].u_int;
        int s =SPI16_Transmit_Data(spi_array[spi_id], data);
    }
    else if(len==32){
        uint32_t data =args_parsed[ARG_data].u_int;
        int s =SPI32_Transmit_Data(spi_array[spi_id], data);
    }
    else{
        mp_raise_ValueError(MP_ERROR_TEXT("invalid len number"));
    }
    return mp_const_none;
}

static mp_obj_t machine_spi_recieve_helper(machine_spi_obj_t *self,size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){
    enum { ARG_mode, ARG_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = 3} }, //default mode 0
        { MP_QSTR_size, MP_ARG_INT ,{.u_int = 8}}, //default 8
        //not data pointer like in i2c its the actual data
    };
    mp_arg_val_t args_parsed[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args_parsed);
    int mode = args_parsed[ARG_mode].u_int;
    //ok so we have the mode value
    int spi_id =self->spi_id;
    if(mode <=3 && mode>=0){
        spi_array[spi_id]->comm_mode=mode;
        //else no change
    }
    uint8_t len = args_parsed[ARG_size].u_int;//else takes 8    uint8_t addr=args_parsed[ARG_addr].u_int;
    // Set mode (replace with your hardware function)
    if(len==8){
        //uint8_t data =args_parsed[ARG_data].u_int;
        int s =SPI8_Receive_Data(spi_array[spi_id]);
    }
    else if(len==16){
        //uint16_t data =args_parsed[ARG_data].u_int;
        int s =SPI16_Receive_Data(spi_array[spi_id]);
    }
    else if(len==32){
        //uint32_t data =args_parsed[ARG_data].u_int;
        int s =SPI32_Receive_Data(spi_array[spi_id]);
    }
    else{
        mp_raise_ValueError(MP_ERROR_TEXT("invalid len number"));
    }
    return mp_const_none;
}
//print function for SPI
static void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(baudrate=%u, polarity=%u, phase=%u)", self->bit_rate, self->polarity, self->phase);
    //have to ensure all the values are properly added before setup
}

//Init function for SPI:
static void machine_spi_obj_init_helper(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    //separating the kw and positional arguments needs prior knowledge of the argument distribution
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    //setting of default values
    enum { ARG_spi_id,ARG_bit_rate, ARG_polarity, ARG_phase,ARG_prescalar,ARG_master_mode,ARG_bits,ARG_lsb_first,ARG_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi_id,MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bit_rate, MP_ARG_KW_ONLY|MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY|MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY|MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_prescalar, MP_ARG_KW_ONLY|MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_master_mode, MP_ARG_INT, {.u_int = -1} },        
        { MP_QSTR_bits, MP_ARG_KW_ONLY|MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_lsb_first, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_size, MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    uint8_t spi_id;
    uint8_t set_value=0;
    if (args[ARG_spi_id].u_int != -1){
        spi_id =args[ARG_spi_id].u_int;
        self->spi_id= args[ARG_spi_id].u_int;
        spi_array[spi_id]->spi_number=spi_id; //already set just to make sure.
    }
    // else{
    //     uint8_t spi_id=0;
    //     self->spi_id=0;
    //     spi_array[self->spi_id]->spi_number=0;
    //     //default
    // }
    //should be in the input function
    if (args[ARG_bit_rate].u_int != -1){
        //more preference to bit_rate
        uint32_t bit_rate =args[ARG_bit_rate].u_int;
        self->bit_rate=bit_rate;
        self->prescalar =prescale_from_bit_rate(bit_rate);
        spi_array[spi_id]->bit_rate=bit_rate;
        //spi_b->bit_rate=0;
        spi_array[spi_id]->prescale=prescale_from_bit_rate(bit_rate);
        set_value=1;
    }
    if (args[ARG_polarity].u_int != -1){
        self->polarity = args[ARG_polarity].u_int;
        spi_array[spi_id]->pol=args[ARG_polarity].u_int;
    }
    if (args[ARG_phase].u_int != -1) 
        {self->phase = args[ARG_phase].u_int;
        spi_array[spi_id]->pha=args[ARG_phase].u_int;
    }
    if(set_value==0 && args[ARG_prescalar].u_int){
        //use prescalar if the bit_rate is not set as input
        uint32_t prescalar =args[ARG_prescalar].u_int;
        self->bit_rate=bit_rate_from_prescale(prescalar);
        self->prescalar =prescalar;
        spi_array[spi_id]->bit_rate=bit_rate_from_prescale(prescalar);
        spi_array[spi_id]->prescale=prescalar;
    }
    if (args[ARG_master_mode].u_int != -1) {
        self->master_mode=args[ARG_master_mode].u_int;
        spi_array[spi_id]->master_mode=args[ARG_master_mode].u_int;
    }
    if (args[ARG_bits].u_int != -1) {
        spi_array[spi_id]->bits=args[ARG_bits].u_int;
    }
    if (args[ARG_lsb_first].u_int != -1) {
        spi_array[spi_id]->lsb_first=args[ARG_lsb_first].u_int;
    }
    if (args[ARG_size].u_int != -1) {
        spi_array[spi_id]->bits=args[ARG_size].u_int;
    }
    // Hardware-specific configuration here (replace SPIx as needed)
    //spi_init_gpio(SPIx, ...);
    //SPI_Init(spi_array[spi_id]);
    SPI_Configure(spi_array[spi_id],0);
    //for now by default hardware based ncs -->to chnage
    SPI_Configure_Clock_In_Hz(spi_array[spi_id]);
    SPI_Enable(spi_array[spi_id]);
    //spi_clock(SPIx, ...);
    //spi_mode(SPIx, self->phase, self->polarity);
}
//object constructor from python inputs
mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    // machine_spi_obj_t *self = mp_obj_malloc(machine_spi_obj_t, &machine_spi_type);
    //we dont want any object we want the object mapped to that instance
    // self->baudrate = 1000000; // Default 1 MHz
    // self->polarity = 0;
    // self->phase = 0;
    machine_spi_obj_t *self = mp_obj_get_spi_obj(args[0]);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_spi_obj_init_helper((mp_obj_base_t *)self, n_args - 1, args + 1, &kw_args);
    return MP_OBJ_FROM_PTR(self);
}
/*In objtype.c, functions like mp_obj_instance_make_new() or fun_builtin_var_call() (for __new__) extract args from the Python call stack and fill in n_args, n_kw, and args[].*/
static mp_obj_t machine_spi_recieve(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_spi_recieve_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
    //the instance number to the actual pointer
    //no need to create an object from the instance number again
}

static MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_recieve_obj, 2, machine_spi_recieve);

static mp_obj_t machine_spi_transmit(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_spi_transmit_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
    //no need to create an object from the instance number again
}

static MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_transmit_obj, 2, machine_spi_transmit);


//-------------------------------------------------------
// static const mp_machine_spi_p_t machine_spi_p = {
//     .init = machine_spi_init,
//     .transfer = machine_spi_transfer,
// };
//protocol table for spi
//type declaration:
// Locals dict table
static const mp_rom_map_elem_t machine_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_transmit), MP_ROM_PTR(&machine_spi_transmit_obj) },
    { MP_ROM_QSTR(MP_QSTR_recieve), MP_ROM_PTR(&machine_spi_recieve_obj) },
};
static MP_DEFINE_CONST_DICT(machine_spi_locals_dict, machine_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    print, machine_spi_print,
    //protocol, &machine_spi_p,
    locals_dict, &machine_spi_locals_dict
    );
