#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include <stdlib.h>
//#include <time.h>
#define STATIC static
extern const mp_obj_module_t random_user_cmodule;

// Register module
MP_REGISTER_MODULE(MP_QSTR_crandom, random_user_cmodule);
