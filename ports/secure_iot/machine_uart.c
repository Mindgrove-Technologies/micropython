
#include "py/mperrno.h"
//This file is part of the MicroPython core.
//return mp_raise_OSError(MP_EINVAL); --> Error invalid val 
#include "py/mphal.h" //provides cross platform macros
// Protocol and dict
#include "py/obj.h" //for objects created 
#include "py/runtime.h"
#include "py/objtype.h"
#include "py/objtuple.h"
//#include "py/ringbuf.h" //for the ring buffer data type
#include "py/stream.h"
#include "uart.h"
#include "utils.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/mpirq.h"
#include "extmod/modmachine.h"
#include <stdint.h>
#include "secure_iot.h"
#include "uart_core.h"
// #include "modmachine.h"

#ifdef MP_DEFINE_CONST_OBJ_TYPE
#define MICRO_PY_MACRO_OK 1
#else
#error "MP_DEFINE_CONST_OBJ_TYPE is not defined!"
#endif



//#include "uart_driver.c"
#define timeout_char 60
#define STATIC static
#define UART_NUM 5
extern const mp_obj_type_t machine_uart_type;
extern volatile UART_Type *uart_instance[MAX_UART_COUNT] ;
typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint8_t uart_id;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop; //stop bit uhh
    uint32_t baudrate;
    uint8_t rx_thresh;

} machine_uart_obj_t;

const machine_uart_obj_t machine_uart_obj[UART_NUM] = {
    {{&machine_uart_type}, 0,8,0,0,115200,10},
    {{&machine_uart_type}, 1,8,0,0,115200,10},
    {{&machine_uart_type}, 2,8,0,0,115200,10},
    {{&machine_uart_type}, 3,8,0,0,115200,10},
    {{&machine_uart_type}, 4,8,0,0,115200,10},

};

UART_Config_t uart_b_inst0={
    .uart_num=0,
    .baudrate=115200, //I think this the baud reg value
    .stop_bits=0,
    .parity=0,
    .char_size=8,
    .delay=100, //check if the delay is fine
    .rxthreshold=10,
    .transfer_mode=0,
    .receive_mode=0,
    .pullup=0,
    //pull up initially disabled.
};
UART_Config_t uart_b_inst1={
    .uart_num=1,
    .baudrate=115200, //I think this the baud reg value
    .stop_bits=0,
    .parity=0,
    .char_size=8,
    .delay=100, //check if the delay is fine
    .rxthreshold=10,
    .transfer_mode=0,
    .receive_mode=0,
    .pullup=0,
    //pull up initially disabled.
};
UART_Config_t uart_b_inst2={
    .uart_num=2,
    .baudrate=115200, //I think this the baud reg value
    .stop_bits=0,
    .parity=0,
    .char_size=8,
    .delay=100, //check if the delay is fine
    .rxthreshold=10,
    .transfer_mode=0,
    .receive_mode=0,
    .pullup=0,
    //pull up initially disabled.
};
UART_Config_t uart_b_inst3={
    .uart_num=3,
    .baudrate=115200, //I think this the baud reg value
    .stop_bits=0,
    .parity=0,
    .char_size=8,
    .delay=100, //check if the delay is fine
    .rxthreshold=10,
    .transfer_mode=0,
    .receive_mode=0,
    .pullup=0,
    //pull up initially disabled.
};
UART_Config_t uart_b_inst4={
    .uart_num=4,
    .baudrate=115200, //I think this the baud reg value
    .stop_bits=0,
    .parity=0,
    .char_size=8,
    .delay=100, //check if the delay is fine
    .rxthreshold=10,
    .transfer_mode=0,
    .receive_mode=0,
    .pullup=0,
    //pull up initially disabled.
};

UART_Config_t* uart_array[UART_NUM] = {
    &uart_b_inst0,
    &uart_b_inst1,
    &uart_b_inst2,
    &uart_b_inst3,
    &uart_b_inst4
};


//function definitions are defined here -> should add it in a .h file
uint8_t UART_Tx_empty(UART_Config_t *uart_config);
static const char *_parity_name[] = {"NO_PARITY" ,"ODD_PARITY" ,"EVEN_PARITY"}; // a lookup tabel for reference -> Where data is passed on to print
//----------------------------------------------------------------------------------------------------------
//All the functions are defined separately
// static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
// //static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
// static mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args); 
// static mp_uint_t mp_machine_uart_read(machine_uart_obj_t *self,mp_uint_t size);
// static mp_uint_t mp_machine_uart_write(machine_uart_obj_t *self, const void *buf_in) ;
// //static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) ;
// static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self);
// static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
// uint8_t UART_Deinit(machine_uart_obj_t *self);

//------------------------------------------------------------------------------------------------------------
//getting the object of uart from the instance number passed as input to the function
static machine_uart_obj_t *mp_obj_get_uart_obj(mp_obj_t uart_in) {
    if (mp_obj_is_small_int(uart_in)) {
        int uart_instance = mp_obj_get_int(uart_in);
        if (uart_instance>= 0 && uart_instance< UART_NUM) {
            return (machine_uart_obj_t *)&machine_uart_obj[uart_instance];
            //for ever mapped to that object ,till the end of time <3
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid uart instance"));
}
//c
static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {

    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    //print – the printer object from MicroPython (where to send the text).
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, rxbuf=%u)",
    self->uart_id,
    self->baudrate,
    self->bits,
    _parity_name[self->parity],
    self->stop,
    self->rx_thresh );
    //defined in py/stream.h
}
//STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_uart_print_obj, mp_machine_uart_print);

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_num,ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_timeout, ARG_rx_thresh };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 0} }, //char size
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 0} },
        {MP_QSTR_timeout,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
        { MP_QSTR_rx_thresh, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} }
    };//.u vachu we are defining initial values
    //key word only arguments -> and object when we are not sure about the datatype to be used.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    int num=args[ARG_num].u_int;
    if (args[ARG_baudrate].u_int > 0) {
        self->baudrate = args[ARG_baudrate].u_int;
        //baud rate vandhuruchu
        uart_array[num]->baudrate = self->baudrate; // Sic!
    }
    else{
        self->baudrate=115200;
        uart_array[num]->baudrate=115200;
    }

    // set data bits
    if((args[ARG_bits].u_int !=NULL)&& ((args[ARG_bits].u_int>=5)&& (args[ARG_bits].u_int<=8))){
        uint8_t value=args[ARG_bits].u_int;
        self->bits=value;
        //there are only 2 bits allocated for 5,6,7,8
        switch(value){
            case 5:
                uart_array[num]->char_size=0;
                break;
            case 6:
                uart_array[num]->char_size=1;
                break;
            case 7:
                uart_array[num]->char_size=2;
                break;
            case 8:
                uart_array[num]->char_size=3;
                break;
        }
        //typically 8
    }
    else{
        self->bits=8;
        uart_array[num]->char_size=3;

    }
    // {NO_PARITY  ,ODD_PARITY ,EVEN_PARITY}

    // set parity
    if (args[ARG_parity].u_obj != MP_OBJ_NULL) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            uart_array[num]->parity = 0;
            //UartDev.exist_parity = UART_STICK_PARITY_DIS;
            self->parity = 0;
        } else {
            mp_int_t parity = mp_obj_get_int(args[ARG_parity].u_obj); //for getting int value from the parity object
            if (parity & 1) {
                uart_array[num]->parity=1;
                //defined in uart.h
                self->parity = 1;
            } else {
                uart_array[num]->parity =2;
                self->parity = 2;
            }
        }
    }

    // set stop bits
    if (args[ARG_stop].u_obj != mp_const_none){
    switch (args[ARG_stop].u_int) {
        case 0:
            uart_array[num]->stop_bits = 0; //1
            self->stop = 0;
            break;
        case 1:
            uart_array[num]->stop_bits = 1;
            self->stop = 1;
            break;
        case 2:
            uart_array[num]->stop_bits = 2;
            self->stop = 2;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid stop bits"));
            break;
    }
    }
    else{
        uart_array[num]->stop_bits=0;
        self->stop=0;
    }
    if (args[ARG_timeout].u_int!=0){
        uart_array[num]->delay=args[ARG_timeout].u_int;
    }

    if(args[ARG_rx_thresh].u_int){
        self->rx_thresh=args[ARG_rx_thresh].u_int;
        uart_array[num]->rxthreshold=self->rx_thresh;
    }
    else{
        self->rx_thresh=10;
        uart_array[num]->rxthreshold=10;
    }

    mp_printf(&mp_plat_print, "before the start of uart_init");
    UART_Init(uart_array[num]);
    //#//define UART0_STATIC_RXBUF_LEN 128
    //uint8_t* buf=uart_buffers[UART_Config_1->uart_num].buffer;
    //uart0_set_rxbuf(buf, UART0_STATIC_RXBUF_LEN);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_machine_uart_init_helper_obj, 1, mp_machine_uart_init_helper);


uint8_t uart_rx_any(UART_Config_t *uart_config) {
//     if (uart_ringbuf.iget != uart_ringbuf.iput) {
//         return true; // have at least 1 char ready for reading
//         //we have a similar fn though UART_Available
//     }
//     return false;
//     }  
    uint8_t value=UART_Available(uart_config);
    return value;
}  

static bool uart_rx_wait(machine_uart_obj_t *self,uint32_t timeout) {
    uint32_t start = mp_hal_ticks_ms();
    do {
        if (uart_rx_any(uart_array[self->uart_id])) {
            return true;
        }
    } while ((mp_hal_ticks_ms() - start) < timeout);
    return false;
    //If not available the recieved bits ,leave
    //shoud be added inside the drivers ,so not used for now :/
}

    //-------------------------------------------------------------------------------------------------------------------------------
static mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    machine_uart_obj_t *uart = mp_obj_get_uart_obj(args[0]);
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        //machine_pin_obj_init_helper(uart, n_args - 1, args + 1, &kw_args);
        mp_machine_uart_init_helper(uart, n_args - 1, args + 1, &kw_args);
    return MP_OBJ_FROM_PTR(uart);
}


// int uart_tx_any_room(uint8_t uart){
//     //(self->uart_id)
//     int num= UART_Config_1->uart_num;
//     int val =buffer_is_full(uart_buffers[num]);
//     return ~val;
// }

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return uart_rx_any(self->uart_id); //WE dont really need this input
    //for checking what uart is currently active in the system
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_uart_any_obj, mp_machine_uart_any);

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    //return !!mp_uart_tx_any(self->uart_id);
    if(UART_Tx_empty(uart_array[self->uart_id])==1){
        //Since it is created as am pointer directly paasing of the variable value is sufficient
        return true;
    }
    return false;

}

static mp_uint_t mp_machine_uart_read(machine_uart_obj_t *self,mp_uint_t size) {
    mp_printf(&mp_plat_print,"in uart read function\n");
    uint16_t len =(uint16_t)size;
    uint8_t s;
    struct uart_buf inst;
    //if (uart_array[self->uart_id]->transfer_mode==0){
    uint8_t mode =uart_array[self->uart_id]->transfer_mode;
    __attribute__ ((aligned(4))) uint8_t array_buffer [128];
    switch(mode){
        case 0:{
        //its of type 8
        uint8_t* buffer=array_buffer;

        //should include 256
        //struct uart_buf inst;
        inst.uart_data=buffer;
        inst.len=len;
        s=UART_Read_String(uart_array[self->uart_id] , &inst);
        if (s==0){
        //mp_printf(&mp_plat_print, "In uart read buff: %s",inst.uart_data);
        buffer=inst.uart_data;
        for(int i=0;i<len;i++){
            mp_printf(&mp_plat_print, "In uart read buff: %c",buffer[i]);
        }
        return len;
        }
        break;
    }
    //}
    //else if (uart_array[self->uart_id]->transfer_mode==1){
        case 1:{
        //its of type 16
        uint16_t *buffer=(uint16_t *)array_buffer;
        //struct uart_buf inst;
        inst.uart_data=buffer;
        inst.len=len;
        //s=UART_Write(uart_array[self->uart_id] , &inst);
        s=UART_Read_String(uart_array[self->uart_id] , &inst);
        if (s==0){
        //mp_printf(&mp_plat_print, "In uart read buff: %s",inst.uart_data);
        buffer=inst.uart_data;
        for(int i=0;i<len;i++){
            mp_printf(&mp_plat_print, "In uart read buff: %x",buffer[i]);
        }
        return len;
        }
        break;}
    //}
    //else if (uart_array[self->uart_id]->transfer_mode==2){
        //its of type 32
        case 2:
        {
        uint32_t *buffer=(uint32_t *)array_buffer;
        //struct uart_buf inst;
        inst.uart_data=buffer;
        inst.len=len;
        s=UART_Read_String(uart_array[self->uart_id] , &inst);
        if (s==0){
        //mp_printf(&mp_plat_print, "In uart read buff: %s",inst.uart_data);
        buffer=inst.uart_data;
        for(int i=0;i<len;i++){
            mp_printf(&mp_plat_print, "In uart read buff: %x",buffer[i]);
        }
        return len;
        }
        break;
        }

    }
    return 0;
    //issue uhh
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_machine_uart_read_obj, 1, mp_machine_uart_read);

static mp_uint_t mp_machine_uart_write(machine_uart_obj_t *self, const void *buf_in) {
    //machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    //const byte *buf = buf_in; -->No 
    mp_printf(&mp_plat_print, "In uart write buff: %s",buf_in);
    // write the data
    //mp_printf(&mp_plat_print, "In uart write size: %u",size);
    uint16_t len=0;
    uint8_t s;
    if (uart_array[self->uart_id]->transfer_mode==0){
        //its of type 8
        uint8_t *buffer=NULL;
        buffer = (uint8_t *)buf_in;
        len=StrLen(buffer);
        //should include 256
        struct uart_buf inst;
        inst.uart_data=buffer;
        inst.len=len;
        s=UART_Write(uart_array[self->uart_id] , &inst);
    }
    else if (uart_array[self->uart_id]->transfer_mode==1){
        //its of type 16
        uint8_t *buffer=NULL;
        buffer = (uint16_t *)buf_in;
        len=StrLen(buffer);
        struct uart_buf inst;
        inst.uart_data=buffer;
        inst.len=len;
        s=UART_Write(uart_array[self->uart_id] , &inst);
    }
    else if (uart_array[self->uart_id]->transfer_mode==2){
        //its of type 32
        uint8_t *buffer=NULL;
        buffer = (uint32_t *)buf_in;
        len=StrLen(buffer);
        struct uart_buf inst;
        inst.uart_data=buffer;
        inst.len=len;
        s=UART_Write(uart_array[self->uart_id] , &inst);
    }
    // char input[300];
    // const char *buf = (const char *)buf_in;
    // size_t copy_len = size < sizeof(input) ? size : sizeof(input) - 1;
    // memcpy(input, buf, copy_len);
    // input[copy_len] = '\0'; // null-terminate
    if (s==0)
        return len;
    return 0;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_machine_uart_write_obj, 1, mp_machine_uart_write);

uint8_t UART_Tx_empty(UART_Config_t *uart_config)
{
  return (((uart_instance[uart_config->uart_num]->STATUS_REG) & STATUS_TX_EMPTY));
  //to check if the returned value is 0 or 1
}


int uart_txdone(uint8_t uart) {
    return(uart_instance[uart]->STATUS_REG & STATUS_TX_EMPTY);
    //0 or 1 as it is a zeroth bit :)
}

uint8_t UART_Deinit(machine_uart_obj_t *self)
{
    UART_Config_t *uart_config=uart_array[self->uart_id];
	log_info("\nDeinitializing UART instance %d", uart_config->uart_num);

	// Wait until all transmissions are complete
	UART_Write_Wait(uart_config);//just to ensure that no data will be lost in the transmission

	// Disable interrupts
	uart_instance[uart_config->uart_num]->INTR_EN = 0;

	// Clear control register to disable UART peripheral
    //Is this okay?
	uart_instance[uart_config->uart_num]->CTRL = 0;
	UART_Flush(uart_config);
	uart_instance[uart_config->uart_num] = NULL;

	log_info("UART%d deinitialized successfully", uart_config->uart_num);
    if (self->uart_id>=3){
        PINMUX_EnableGPIO(self->uart_id);
    }
	return SUCCESS;
}

//STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_machine_uart_init_helper_obj, 1, mp_machine_uart_init_helper);
//MP_REGISTER_ROOT_POINTER(byte * uart0_rxbuf);
//Not using this anymore


// const mp_obj_type_t machine_uart_type = {
//     { &mp_type_type },
//     .name = MP_QSTR_UART,
//     .make_new = machine_uart_make_new,
//     .print = mp_machine_uart_print,
//     .protocol = &uart_stream_p,
//     .locals_dict = (mp_obj_dict_t *)&mp_machine_uart_locals_dict,
// };


// MP_DEFINE_CONST_OBJ_TYPE(
//     machine_uart_type,
//     MP_QSTR_UART,
//     MP_TYPE_FLAG_NONE,
//     machine_uart_make_new,
//     mp_machine_uart_print,
//     &mp_machine_uart_locals_dict
// );

static const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_machine_uart_init_helper_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_machine_uart_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_machine_uart_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&mp_machine_uart_any_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&UART_Deinit_obj) },
    //{MP_ROM_QSTR(MP_QSTR_uart_print),MP_ROM_PTR(&mp_machine_uart_print_obj)}
};

static MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);


    // static mp_stream_p_t uart_stream_p = {
    // .read = mp_machine_uart_read,
    // .write = mp_machine_uart_write,
    // .ioctl = mp_machine_uart_ioctl,
    // .is_text = false,
    // };

MP_DEFINE_CONST_OBJ_TYPE(
    machine_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_NONE,
    make_new, machine_uart_make_new,
    print, mp_machine_uart_print,
    locals_dict, &machine_uart_locals_dict
);


