

#include "py/mperrno.h"
//This file is part of the MicroPython core.
//return mp_raise_OSError(MP_EINVAL); --> Error invalid val 
#include "py/mphal.h" //provides cross platform macros
//#include "ets_sys.h" //It is also not there in these files :)
/*
ets_sys.h is a low-level system header from the ESP8266 Non-OS SDK (provided by Espressif). It provides:

Access to internal system functions
Interrupt control macros
Task scheduling functions (ets_* functions)
ROM-based function declarations*/
//A stripped-down, minimalist embedded Python interpreter.

// Protocol and dict
#include "py/obj.h" //for objects created 
#include "py/runtime.h"
#include "py/objtuple.h"
//#include "py/argcheck.c" --> Double includes
#include "py/ringbuf.h" //for the ring buffer data type
#include "py/mperrno.h"
#include "py/stream.h"
#include "uart.h"
#include "utils.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/mpirq.h"
#include "extmod/modmachine.h"
#include <stdint.h>
//#include "uart_driver.c"
#define timeout_char 60
#define STATIC static
#include "uart_core.h"
extern const mp_obj_type_t machine_uart_type;
//extern uart_buffers; //Kolar
extern CircularBuffer uart_buffers[3];
//extern UART_Config_t uart;
extern UART_Config_t *UART_Config_1; //the uart one
extern volatile UART_Type *uart_instance[MAX_UART_COUNT] ;
//assigning the initial a value to take zero
//The Python machine.UART() class is bound to the C object via MicroPython’s binding mechanism 
//(using MP_DEFINE_CONST_FUN_OBJ, mp_obj_type_t, etc.). --> Here just another struct...
typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint8_t uart_id;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop; //stop bit uhh
    uint32_t baudrate;
    uint8_t rx_thresh;

} machine_uart_obj_t;

uint8_t UART_Tx_empty(UART_Config_t *uart_config);
static const char *_parity_name[] = {"NO_PARITY" ,"ODD_PARITY" ,"EVEN_PARITY"}; // a lookup tabel for reference -> Where data is passed on to
#define UART0_STATIC_RXBUF_LEN 128
uint8_t uart_ringbuf_array[UART0_STATIC_RXBUF_LEN];
//uart_ringbuf_array=*buf; -->Not yet
//A part of the circular buffer -->buffer
static ringbuf_t uart_ringbuf = {uart_ringbuf_array, sizeof(uart_ringbuf_array), 0, 0};
//can take values from 0-2
/******************************************************************************/
// MicroPython bindings for UART

// The UART class doesn't have any constants for this port.
//#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS

//----------------------------------------------------------------------------------------------------------
//All the kolar functions are defined separately
static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode);
static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) ;
static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) ;
static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self);
static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_machine_uart_read_obj, 1, mp_machine_uart_read);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_machine_uart_write_obj, 1, mp_machine_uart_write);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_uart_any_obj, mp_machine_uart_any);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_uart_print_obj, mp_machine_uart_print);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_machine_uart_init_helper_obj, 1, mp_machine_uart_init_helper);

//------------------------------------------------------------------------------------------------------------End of kolar

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    //mp_print_t is a MicroPython internal type that abstracts printing, allowing you to print to different outputs (like REPL, file, buffer, etc.) using a consistent interface.
    //DEfined in the py folder
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
//-------------------------------------------------------------------------------------
void uart0_set_rxbuf(uint8_t *buf, int len) {
    //ETS_UART_INTR_DISABLE();
    uart_ringbuf.buf = buf;
    uart_ringbuf.size = len;
    uart_ringbuf.iget = 0;
    uart_ringbuf.iput = 0;
    //ETS_UART_INTR_ENABLE();
}
//-----------------------------------------------------------------------------------------------

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx, ARG_timeout, ARG_rx_thresh };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        //{MP_QSTR_rxbuf,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} }, 
        {MP_QSTR_timeout,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
        { MP_QSTR_rx_thresh, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} }
    };//.u vachu we are defining initial values
    //key word only arguments -> and object when we are not sure about the datatype to be used.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    //After this call, args[ARG_baudrate].u_int contains the value the user passed for "baudrate".
    //calls micropython internal function to parse and map arguments
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    //Q strings --> MP_QSTR_foo_rate
    //then define in qsterdefs.h as Q(foo_rate)

    UART_Config_1->buffer_enable=0;
    // set baudrate
    if (args[ARG_baudrate].u_int > 0) {
        self->baudrate = args[ARG_baudrate].u_int;
        //baud rate vandhuruchu
        UART_Config_1->baudrate = self->baudrate; // Sic!
    }
    else{
        self->baudrate=115200;
        UART_Config_1->baudrate=115200;
    }

    // set data bits
    if(args[ARG_bits].u_int !=NULL){
        self->bits=args[ARG_bits].u_int;
        UART_Config_1->char_size=self->bits;
        //typically 8
    }
    else{
        self->bits=8;

    }
    // {NO_PARITY  ,ODD_PARITY ,EVEN_PARITY}

    // set parity
    if (args[ARG_parity].u_obj != MP_OBJ_NULL) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            UART_Config_1->parity = 0;
            //UartDev.exist_parity = UART_STICK_PARITY_DIS;
            self->parity = 0;
        } else {
            mp_int_t parity = mp_obj_get_int(args[ARG_parity].u_obj);
            //UartDev.exist_parity = UART_STICK_PARITY_EN;
            if (parity & 1) {
                //UartDev.parity = UART_ODD_BITS;
                UART_Config_1->parity=1;
                //defined in uart.h
                self->parity = 1;
            } else {
                UART_Config_1->parity =2;
                self->parity = 2;
            }
        }
    }

    // set stop bits
    if (args[ARG_parity].u_obj != mp_const_none){
    switch (args[ARG_stop].u_int) {
        case 0:
            UART_Config_1->stop_bits = 0; //1
            self->stop = 0;
            break;
        case 1:
            UART_Config_1->stop_bits = 1;
            self->stop = 1;
            break;
        case 2:
            UART_Config_1->stop_bits = 2;
            self->stop = 2;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid stop bits"));
            break;
    }
    }
    else{
        UART_Config_1->stop_bits=0;
        self->stop=0;
    }

    if(args[ARG_rx_thresh].u_int){
        self->rx_thresh=args[ARG_rx_thresh].u_int;
        UART_Config_1->rxthreshold=self->rx_thresh;
    }
    else{
        self->rx_thresh=10;
        UART_Config_1->rxthreshold=10;
    }

    UART_Config_1->delay=1;
    mp_printf(&mp_plat_print, "before the start of uart_init");
    UART_Init(UART_Config_1);
    // set rx ring buffer
    /*typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    } CircularBuffer;*/
    //CircularBuffer uart_buffers[3];
    #define UART0_STATIC_RXBUF_LEN 128
    // if(arg[ARG_rxbuf].u_int ==mp_const_none){
    //     uint8_t* buf=NULL;
    //     uart0_set_rxbuf(buf, UART0_STATIC_RXBUF_LEN);
    //     uart_buffers[UART_Config_1->uart_num].buffer=buf;
    // }
    // else if (args[ARG_rxbuf].u_int > 0) { //This checks if the user has requested a non-zero RX buffer size (from function arguments
    //     uint16_t len = args[ARG_rxbuf].u_int + 1; // account for usable items in ringbuf 
    //     //ring buf wrap
    //     //+1 as rin buffere need +1 slot for difference between full and empty
    //     //actual values filled in the buffer
    //     uint8_t* buf=NULL;
    //     if (len <= UART0_STATIC_RXBUF_LEN) {
    //         buf = uart_buffers[0].buffer; //pointeruhh
    //         MP_STATE_VM(uart0_rxbuf) = NULL; // clear any old pointer
    //         uart_buffers[UART_Config_1->uart_num].buffer=buf;
    //         //MP_STATE_PORT(uart0_rxbuf) is set to NULL to clear any previously dynamically allocated memory.
    //     } else {
    //         //If the requested buffer size exceeds the static buffer size:
    //         buf = m_new(byte, len);
    //         MP_STATE_VM(uart0_rxbuf) = buf; // retain root pointer
    //         uart_buffers[UART_Config_1->uart_num].buffer=buf;
    //     }
    //     uart0_set_rxbuf(buf, len);
    // }

    // setup
    ///uart_setup(self->uart_id); -->very specific to esp 8266 and configuring priorities
    uint8_t* buf=uart_buffers[UART_Config_1->uart_num].buffer;
    uart0_set_rxbuf(buf, UART0_STATIC_RXBUF_LEN);
}
/*typedef struct _ringbuf_t {
    uint8_t *buf;
    uint16_t size;
    uint16_t iget;
    uint16_t iput;
} ringbuf_t; --> defined in ringbuf.h*/
//CircularBuffer uart_buffers[3];
/*typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
} CircularBuffer;*/
//------------------------------------------------------------------------------------------------------


int uart_rx_any(uint8_t uart) {
    if (uart_ringbuf.iget != uart_ringbuf.iput) {
        return true; // have at least 1 char ready for reading
        //we have a similar fn though UART_Available
    }
    return false;
    }    
static bool uart_rx_wait(uint32_t timeout) {
    uint32_t start = mp_hal_ticks_ms();
    do {
        if (uart_rx_any(0)) {
            return true;
        }
    } while ((mp_hal_ticks_ms() - start) < timeout);
    return false;
}

int uart_tx_any_room(uint8_t uart){
    //(self->uart_id)
    int num= UART_Config_1->uart_num;
    int val =buffer_is_full(uart_buffers[num]);
    return ~val;
}

    //---------------------------------------------------------------------------------------------------------------------------------
    //The kolar definition for creating uart hal
    /*static const mp_stream_p_t uart_stream_p = {
    .read = mp_machine_uart_read,
    .write = mp_machine_uart_write,
    .ioctl = mp_machine_uart_ioctl,
    .is_text = false,
    };
    const mp_obj_type_t machine_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .slot_index_print = mp_machine_uart_print,
    .slot_index_make_new = mp_machine_uart_make_new,
    .slot_index_protocol = &uart_stream_p,
    .slot_index_locals_dict = (mp_obj_dict_t*)&mp_machine_uart_locals_dict,
    };*/

    STATIC const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_machine_uart_init_helper_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_machine_uart_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_machine_uart_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&mp_machine_uart_any_obj) },
    {MP_ROM_QSTR(MP_QSTR_uart_print),MP_ROM_PTR(&mp_machine_uart_print_obj)}
    };

    STATIC MP_DEFINE_CONST_DICT(mp_machine_uart_locals_dict, machine_uart_locals_dict_table);

    static mp_stream_p_t uart_stream_p = {
    .read = mp_machine_uart_read,
    .write = mp_machine_uart_write,
    .ioctl = mp_machine_uart_ioctl,
    .is_text = false,
    };

    // MP_DEFINE_OBJ_TYPE(
    //     machine_uart_type,
    //     MP_QSTR_UART,
    //     MP_TYPE_FLAG_NONE,
    //     print, mp_machine_uart_print,
    //     make_new, mp_machine_uart_make_new,
    //     protocol, &uart_stream_p,
    //     locals_dict, &mp_machine_uart_locals_dict
    // );
    MP_DEFINE_CONST_OBJ_TYPE(
    machine_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_uart_make_new,
    print, mp_machine_uart_print,
    protocol, &uart_stream_p, //is this correct?
    locals_dict, &mp_machine_uart_locals_dict
    );


    //---------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------
static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    //Checks the number of input arguments
    // get uart id
    mp_int_t uart_id = mp_obj_get_int(args[0]);//first one of passed args was id
    if (uart_id != 2 && uart_id != 1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) does not exist"), uart_id);
    }
    // create instance --> Points to the object to be created newly 
    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);  //the struct defined in this page
    //Uart type is defined in modmachine.h
    //machine_uart_obj_t --> is defined in machine_uart.c -->struct
    self->uart_id = uart_id;
    self->baudrate = 115200;
    self->bits = 8; //The bits field refers to the number of data bits per UART frame during serial communication.
    self->parity = 0;
    self->stop = 1;
    self->rx_thresh=0; //have to change ---> for now interrupt is not enabled so
    //self->timeout = 0;
    //self->timeout_char = 0;

    //maybe UART_Config_1 is getting corrupted somewhere so defining the values again
    if(uart_id>2 | uart_id <0){
        uart_id=2;
    }
    UART_Config_1->uart_num=uart_id;
    UART_Config_1->baudrate=115200;
    UART_Config_1->buffer_enable=0;
    UART_Config_1->char_size=8;
    UART_Config_1->delay=1;
    UART_Config_1->parity=0;
    UART_Config_1->rxthreshold=10;
    UART_Config_1->stop_bits=0;

    // init the peripheral
    mp_map_t kw_args; //defined in obj.h
    /* typedef struct _mp_map_t {
    size_t all_keys_are_qstrs : 1;
    size_t is_fixed : 1;    // if set, table is fixed/read-only and can't be modified
    size_t is_ordered : 1;  // if set, table is an ordered array, not a hash map
    size_t used : (8 * sizeof(size_t) - 3);
    size_t alloc;
    mp_map_elem_t *table;
    } mp_map_t;*/
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args); //no.of kw args , pointing to address of keywrd args
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);
    //how are the arguments of the make passed to the helper?

    return MP_OBJ_FROM_PTR(self);
}


static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    (void)self; //making self as void 
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return uart_rx_any(self->uart_id); //WE dont really need this input
    //for checking what uart is currently active in the system
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    //return !!mp_uart_tx_any(self->uart_id);
    if(UART_Tx_empty(UART_Config_1)==1){
        //Since it is created as am pointer directly paasing of the variable value is sufficient
        return true;
    }
    return false;

}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    //self in is a micropython object passed as input to the function
    //size :number of bytes the caller wants to read
    //buf_in :data ->as a buffer that is passed
    //errcode: A pointer to an integer where an error code will be written if the read fails.
    //null pointer -->to buffer input 
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    //instance of the object
    // if (self->uart_id == 0) {
    //     mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("UART(0) can't read"));
    //     //blocking a certain uart id
    // }
    // if (self->uart_id == 1) {
    //     mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("UART(1) can't read"));
    //     //blocking a certain uart id
    // }
    // if (self->uart_id == 2) {
    //     mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("UART(2) can't read"));
    //     //blocking a certain uart id
    // }

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }


    // wait for first char to become available
    if (!uart_rx_wait(timeout_char * 1000)) {
        //blocking function that waits for uart signal to be available
        *errcode = MP_EAGAIN; //try again later :)
        //included from py/mperrno.h
        return MP_STREAM_ERROR; //defined in stream.h ---> Some error during r/w
    }

    // read the data
    uint8_t *buf = buf_in; //the location of the buffer
    //casts the generic buffer pointer buf_in to a uint8_t*, so we can store incoming bytes (char data) directly.
    //--------------------------------------------------------------------------------------------------------------
    // for (;;) { //inf
    //     *buf++ = uart_rx_char();
    //     if (--size == 0 || !uart_rx_wait(self->timeout_char * 1000)) { -->defined code for this
    //         // return number of bytes read
    //         return buf - (uint8_t *)buf_in; //gives the total number of bytes stored 
    //     }
    //}
    //replace this 
    for (;;) { //inf
         *buf++ = getchar();
         //from uart_driver.c code
         if (--size == 0 || !uart_rx_wait(timeout_char * 1000)) { //-->defined code for this
             // return number of bytes read
            return buf - (uint8_t *)buf_in; //gives the total number of bytes stored 
        }
    }
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const byte *buf = buf_in;

    mp_printf(&mp_plat_print, "In uart write buff: %s",buf_in);
    // write the data
    mp_printf(&mp_plat_print, "In uart write size: %u",size);
    for (size_t i = 0; i < size; ++i) {
        //uart_tx_one_char(self->uart_id, *buf++);
        int j= putchar(*buf++);
        mp_printf(&mp_plat_print, "%02X ", ((const unsigned char*)buf_in)[i]);
        //for printing if not null terminated
        //do nothing with it j lol jk
        if (j!=1){
            //repeat
            buf =buf-1;
        }

    }

    // return number of bytes written
    return size;
    //return mp_obj_new_int_from_uint(size);
    //no need to wrap it up as an micropython object
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    uint64_t start;
    //handles the input output control requests
    machine_uart_obj_t *self = self_in; //uart object
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) { //defined in stream poll.h
    //Check if UART is readable/writable
        mp_uint_t flags = arg;
        ret = 0;
        //checks if uart is ready for reading 
        if ((flags & MP_STREAM_POLL_RD) && uart_rx_any(self->uart_id)) {
            ret |= MP_STREAM_POLL_RD;
        }
        //checks if uart is ready for writing
        if ((flags & MP_STREAM_POLL_WR) && uart_tx_any_room(self->uart_id)) { //have to redefine
            ret |= MP_STREAM_POLL_WR;
        }
        //Flush output buffer (optional)
    } else if (request == MP_STREAM_FLUSH) { //waits till tx data is being flushed
        // The timeout is estimated using the buffer size and the baudrate.
        // Take the worst case assumptions at 13 bit symbol size times 2.
        uint64_t timeout = (uint64_t)(3 + 127) * 13000000ll * 2 / self->baudrate
            + millis();
        do {
            if (mp_machine_uart_txdone(self)) {
                return 0;
            }
            mp_event_wait_ms(1);
            uint64_t start =millis();
        } 
        
        while ((millis()-start)< timeout);

        *errcode = MP_ETIMEDOUT;
        ret = MP_STREAM_ERROR;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}
//STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_machine_uart_ioctl_obj,1,mp_machine_uart_ioctl); ---> WE cannot expose this function as it is internal it seems
//Me saddd
// STATIC MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

// const mp_obj_type_t machine_uart_type = {
//     .base = { &mp_type_type },
//     .name = MP_QSTR_UART,
//     .print = machine_uart_print,
//     .make_new = machine_uart_make_new,
//     .locals_dict = (mp_obj_dict_t *)&machine_uart_locals_dict,
// };


uint8_t UART_Tx_empty(UART_Config_t *uart_config)
{
  return (((uart_instance[uart_config->uart_num]->STATUS_REG) & STATUS_TX_EMPTY));
  //to check if the returned value is 0 or 1
}


int uart_txdone(uint8_t uart) {
    return(uart_instance[uart]->STATUS_REG & STATUS_TX_EMPTY);
    //0 or 1 as it is a zeroth bit :)
}



MP_REGISTER_ROOT_POINTER(byte * uart0_rxbuf);
