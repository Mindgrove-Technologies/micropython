// #include "py/obj.h"
// #include "py/runtime.h"
// #include "py/builtin.h"
// #define STATIC static
// // === 2. Global QSTR-Function Mapping Table ===
// STATIC const mp_rom_map_elem_t mymodule_globals_table[] = {
//     { MP_ROM_QSTR(MP_QSTR_testfunc), MP_ROM_PTR(&testfunc_obj) },
// };
// STATIC MP_DEFINE_CONST_DICT(mymodule_globals, mymodule_globals_table);


// // === 3. The Module Object ===

// mp_obj_t testfunc(void);

// const mp_obj_module_t mymodule_module = {
//     .base = { &mp_type_module },
//     .globals = (mp_obj_dict_t *)&mymodule_globals,
// };
