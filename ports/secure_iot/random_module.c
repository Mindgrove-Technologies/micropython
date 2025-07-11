#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include <stdlib.h>
#include "random_module.h"
//#include <time.h>
#define STATIC static
//Written by scratch by me as numpy's random is wack
/*    Don’t rely on an operating system (no syscalls, ecall, or host filesystem interaction).

    Are purely computational or operate only in user space.

Examples:

    strlen, memcpy, memset, strncpy, strcmp, etc.
    math.h functions like sin, cos, sqrt (assuming the floating-point ABI is supported).
    stdlib.h functions like atoi, abs, rand (with caution).
    Stay safe of calls Baremetal*/
    //all credits go to me as I wrote this from scratch

// random() — float in [0.0, 1.0) ===
STATIC mp_obj_t mod_random_random(void) {
    float val = (float)rand() / (float)(RAND_MAX);
    //If want values specifically in the range 0-255 (Bitmask)
    //uint8_t r = rand() & 0xFF;  // range 0 to 255

    return mp_obj_new_float(val);
    /*The standard rand() function in C (from stdlib.h) generates a pseudo-random integer between 0 and RAND_MAX
    Since The max value it can take is --> RAND MAX */
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_random_random_obj, mod_random_random);

// randint(a, b) — random int between a and b inclusive ===
STATIC mp_obj_t mod_random_randint(mp_obj_t a_in, mp_obj_t b_in) {
    int a = mp_obj_get_int(a_in);
    int b = mp_obj_get_int(b_in);
    if (b < a) {
        mp_raise_ValueError(MP_ERROR_TEXT("b must be >= a"));
    }
    int r = a + rand() % (b - a + 1);
    //rand()/(upper_lim+1) for range from 0
    //but keep in mind the distribution is somewhat non uniform.
    //+1 for the case when a==b
    return mp_obj_new_int(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_random_randint_obj, mod_random_randint);

// randrange(stop) or randrange(start, stop[, step])
STATIC mp_obj_t mod_random_randrange(size_t n_args, const mp_obj_t *args) {
    int start = 0, stop = 0, step = 1;

    if (n_args == 1) {
        stop = mp_obj_get_int(args[0]);
    } else if (n_args >= 2) {
        start = mp_obj_get_int(args[0]);
        stop = mp_obj_get_int(args[1]);
        if (n_args == 3) {
            step = mp_obj_get_int(args[2]);
        }
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("randrange() requires at least 1 argument"));
    }

    if (step <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("step must be > 0"));
        //for now this is what I am supporting 
    }
    int range = (stop - start + step - 1) / step;
    if (range <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("empty range"));
    }
    int r = rand() % range;
    return mp_obj_new_int(start + r * step);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_random_randrange_obj, 1, 3, mod_random_randrange);

// seed(n) ===
STATIC mp_obj_t mod_random_seed(mp_obj_t n_in) {
    unsigned int seed_val = mp_obj_get_int(n_in);
    srand(seed_val);
    return mp_const_none;
    //This tells the random number generator to always start from the same internal state. This means you'll get the 
    //same sequence of random numbers each time you run your program (good for debugging and reproducibility).
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_random_seed_obj, mod_random_seed);

// getrandbits(k) ===
STATIC mp_obj_t mod_random_getrandbits(mp_obj_t k_in) {
    int k = mp_obj_get_int(k_in);
    if (k <= 0 || k > 32) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits must be in 1..32"));
        //checking condition for function.
    }
    uint32_t r = rand();
    return mp_obj_new_int_from_uint(r & ((1UL << k) - 1));
    //normally shifting by 1 should also work as it considers it as an uint32_t number.
    //the number left shifted -1 to give that many number of ones.

}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_random_getrandbits_obj, mod_random_getrandbits);
STATIC const mp_rom_map_elem_t randommodule_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_crandom) },
    { MP_ROM_QSTR(MP_QSTR_random), MP_ROM_PTR(&mod_random_random_obj) },
    { MP_ROM_QSTR(MP_QSTR_randint), MP_ROM_PTR(&mod_random_randint_obj) },
    { MP_ROM_QSTR(MP_QSTR_randrange), MP_ROM_PTR(&mod_random_randrange_obj) },
    { MP_ROM_QSTR(MP_QSTR_seed), MP_ROM_PTR(&mod_random_seed_obj) },
    { MP_ROM_QSTR(MP_QSTR_getrandbits), MP_ROM_PTR(&mod_random_getrandbits_obj) },
};

STATIC MP_DEFINE_CONST_DICT(randommodule_globals, randommodule_globals_table);

const mp_obj_module_t random_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&randommodule_globals,
};

// Register module
//MP_REGISTER_MODULE(MP_QSTR_crandom, random_user_cmodule);
