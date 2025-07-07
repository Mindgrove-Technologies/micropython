#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "module.h"
#include "py/objint.h"

// Force QSTR generation
// STATIC void _force_qstr(void) {
//     //MP_UNUSED(MP_QSTR_myvar);
// }

#define STATIC static
// === 1. C Function to Expose ===
STATIC mp_obj_t testfunc(void) {
    printf("Hello from testfunc!\n");
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(testfunc_obj, testfunc);

// === 2. Global QSTR-Function Mapping Table ===
STATIC const mp_rom_map_elem_t mymodule_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_testfunc), MP_ROM_PTR(&testfunc_obj) },
    //for the defining of constant value
    //{ MP_ROM_QSTR(MP_QSTR_myvar), MP_OBJ_NEW_SMALL_INT(42) }
};
STATIC MP_DEFINE_CONST_DICT(mymodule_globals, mymodule_globals_table);

// // === 3. The Module Object ===

const mp_obj_module_t mymodule = {
     .base = { &mp_type_module },
     .globals = (mp_obj_dict_t *)&mymodule_globals,
};
// Force QSTR generation (even if not used elsewhere)
//const qstr __unused1 = MP_QSTR_mymodule;
//const qstr __unused2 = MP_QSTR_myval;

// === 4. Register the Module for Import ===
MP_REGISTER_MODULE(MP_QSTR_mymodule, mymodule);
// #define MICROPY_REGISTERED_MODULES 
//     { MP_ROM_QSTR(MP_QSTR_mymodule), MP_ROM_PTR(&mymodule) },
const struct _mp_obj_module_t *const force_link_mymodule __attribute__((used, section(".modules"))) = &mymodule;

