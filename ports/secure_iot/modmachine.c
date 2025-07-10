#include "py/obj.h"
#include "py/runtime.h"
//#include "extmod/machine_uart.h"
#include "modmachine.h"

extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_wdt_type;
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_i2c_type;

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&machine_uart_type) },
    { MP_ROM_QSTR(MP_QSTR_WDT), MP_ROM_PTR(&machine_wdt_type) },
    {MP_ROM_QSTR(MP_QSTR_Pin),MP_ROM_PTR(&machine_pin_type)},
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&machine_i2c_type) },

};
//defines the different object types --> Like the imported objects in machine
STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t machine_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};

// Register with MicroPython
MP_REGISTER_MODULE(MP_QSTR_machine, machine_module);
