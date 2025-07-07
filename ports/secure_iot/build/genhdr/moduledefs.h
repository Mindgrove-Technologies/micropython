// Contains the list of modules to be finally added after linking / The modules that will be visible from python.
#include "py/obj.h"
#include "module.h"
#include "ulab.h"
#include "numpy/numpy.h" //should have the module names --> Internaly 
//But the module objects should be present in the respective files only
#include "numpy/fft/fft.h"
#include "numpy/linalg/linalg.h"
#include "numpy/random/random.h"
#include "numpy/io/io.h"

#include "scipy/scipy.h"
#include "scipy/integrate/integrate.h"
#include "scipy/linalg/linalg.h"
#include "scipy/optimize/optimize.h"
#include "scipy/signal/signal.h"
#include "scipy/special/special.h"

#include "utils/utils.h"
#include "modmachine.h"
//#include "py/modmath.h" --> It doesnt exist

extern const struct _mp_obj_module_t mp_module_array;
#undef MODULE_DEF_ARRAY
#define MODULE_DEF_ARRAY { MP_ROM_QSTR(MP_QSTR_array), MP_ROM_PTR(&mp_module_array) },

extern const struct _mp_obj_module_t mp_module_collections;
#undef MODULE_DEF_COLLECTIONS
#define MODULE_DEF_COLLECTIONS { MP_ROM_QSTR(MP_QSTR_collections), MP_ROM_PTR(&mp_module_collections) },

extern const struct _mp_obj_module_t mp_module_io;
#undef MODULE_DEF_IO
#define MODULE_DEF_IO { MP_ROM_QSTR(MP_QSTR_io), MP_ROM_PTR(&mp_module_io) },

extern const struct _mp_obj_module_t mp_module_struct;
#undef MODULE_DEF_STRUCT
#define MODULE_DEF_STRUCT { MP_ROM_QSTR(MP_QSTR_struct), MP_ROM_PTR(&mp_module_struct) },

extern const struct _mp_obj_module_t mp_module___main__;
#undef MODULE_DEF___MAIN__
#define MODULE_DEF___MAIN__ { MP_ROM_QSTR(MP_QSTR___main__), MP_ROM_PTR(&mp_module___main__) },

extern const struct _mp_obj_module_t mp_module_builtins;
#undef MODULE_DEF_BUILTINS
#define MODULE_DEF_BUILTINS { MP_ROM_QSTR(MP_QSTR_builtins), MP_ROM_PTR(&mp_module_builtins) },

extern const struct _mp_obj_module_t mp_module_gc;
#undef MODULE_DEF_GC
#define MODULE_DEF_GC { MP_ROM_QSTR(MP_QSTR_gc), MP_ROM_PTR(&mp_module_gc) },

extern const struct _mp_obj_module_t mp_module_micropython;
#undef MODULE_DEF_MICROPYTHON
#define MODULE_DEF_MICROPYTHON { MP_ROM_QSTR(MP_QSTR_micropython), MP_ROM_PTR(&mp_module_micropython) },

extern const struct _mp_obj_module_t mp_module_sys;
#undef MODULE_DEF_SYS
#define MODULE_DEF_SYS { MP_ROM_QSTR(MP_QSTR_sys), MP_ROM_PTR(&mp_module_sys) },

extern const struct _mp_obj_module_t mymodule;
#undef MODULE_DEF_MYMODULE
#define MODULE_DEF_MYMODULE { MP_ROM_QSTR(MP_QSTR_mymodule), MP_ROM_PTR(&mymodule) },

extern const struct _mp_obj_module_t mp_module_math;
#undef MODULE_DEF_MATH
#define MODULE_DEF_MATH{ MP_ROM_QSTR(MP_QSTR_math), MP_ROM_PTR(&mp_module_math) },

extern const mp_obj_module_t ulab_user_cmodule;
#undef MODULE_DEF_ULAB
#define MODULE_DEF_ULAB{ MP_ROM_QSTR(MP_QSTR_ulab), MP_ROM_PTR(&ulab_user_cmodule) },

// Numpy module inside ulab
extern const struct _mp_obj_module_t ulab_numpy_module;
#undef MODULE_DEF_NUMPY
#define MODULE_DEF_NUMPY{ MP_ROM_QSTR(MP_QSTR_ulab_dot_numpy), MP_ROM_PTR(&ulab_numpy_module) },

//Inside the numpy module --> The sub modules
extern const struct _mp_obj_module_t ulab_fft_module;
#undef MODULE_DEF_NUMPY_FFT
#define MODULE_DEF_NUMPY_FFT{ MP_ROM_QSTR(MP_QSTR_ulab_dot_numpy_dot_fft), MP_ROM_PTR(&ulab_fft_module) },

extern const struct _mp_obj_module_t ulab_linalg_module;
#undef MODULE_DEF_NUMPY_LINALG
#define MODULE_DEF_NUMPY_LINALG{ MP_ROM_QSTR(MP_QSTR_ulab_dot_numpy_dot_linalg), MP_ROM_PTR(&ulab_linalg_module) },

extern const struct _mp_obj_module_t ulab_numpy_random_module;
#undef MODULE_DEF_NUMPY_RANDOM
#define MODULE_DEF_NUMPY_RANDOM{ MP_ROM_QSTR(MP_QSTR_ulab_dot_numpy_dot_random), MP_ROM_PTR(&ulab_numpy_random_module) },

//Scipy module inside ulab
extern const struct _mp_obj_module_t ulab_scipy_module;
#undef MODULE_DEF_SCIPY
#define MODULE_DEF_SCIPY{ MP_ROM_QSTR(MP_QSTR_ulab_dot_scipy), MP_ROM_PTR(&ulab_scipy_module) },

//Modules inside scipy
extern const struct _mp_obj_module_t ulab_scipy_integrate_module;
#undef MODULE_DEF_SCIPY_INTEGRATE
#define MODULE_DEF_SCIPY_INTEGRATE{ MP_ROM_QSTR(MP_QSTR_ulab_dot_scipy_dot_integrate), MP_ROM_PTR(&ulab_scipy_integrate_module) },

extern const struct _mp_obj_module_t ulab_scipy_linalg_module;
#undef MODULE_DEF_SCIPY_LINALG
#define MODULE_DEF_SCIPY_LINALG{ MP_ROM_QSTR(MP_QSTR_ulab_dot_scipy_dot_linalg), MP_ROM_PTR(&ulab_scipy_linalg_module) },

extern const struct _mp_obj_module_t ulab_scipy_optimize_module;
#undef MODULE_DEF_SCIPY_OPTIMIZE
#define MODULE_DEF_SCIPY_OPTIMIZE{ MP_ROM_QSTR(MP_QSTR_ulab_dot_scipy_dot_optimize), MP_ROM_PTR(&ulab_scipy_optimize_module) },

extern const struct _mp_obj_module_t ulab_scipy_signal_module;
#undef MODULE_DEF_SCIPY_SIGNAL
#define MODULE_DEF_SCIPY_SIGNAL{ MP_ROM_QSTR(MP_QSTR_ulab_dot_scipy_dot_signal), MP_ROM_PTR(&ulab_scipy_signal_module) },

extern const struct _mp_obj_module_t ulab_scipy_special_module;
#undef MODULE_DEF_SCIPY_SPECIAL
#define MODULE_DEF_SCIPY_SPECIAL{ MP_ROM_QSTR(MP_QSTR_ulab_dot_scipy_dot_special), MP_ROM_PTR(&ulab_scipy_special_module) },

//machine module :machine_module
extern const struct _mp_obj_module_t machine_module;
#undef MODULE_DEF_MACHINE
#define MODULE_DEF_MACHINE{ MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&machine_module) },

#define MICROPY_REGISTERED_MODULES \
    MODULE_DEF_BUILTINS \
    MODULE_DEF_GC \
    MODULE_DEF_MICROPYTHON \
    MODULE_DEF_SYS \
    MODULE_DEF___MAIN__ \
    MODULE_DEF_MYMODULE \
    MODULE_DEF_MATH \
// MICROPY_REGISTERED_MODULES

#define MICROPY_REGISTERED_EXTENSIBLE_MODULES \
    MODULE_DEF_ARRAY \
    MODULE_DEF_COLLECTIONS \
    MODULE_DEF_IO \
    MODULE_DEF_STRUCT \
    MODULE_DEF_ULAB \
    MODULE_DEF_NUMPY \
    MODULE_DEF_NUMPY_FFT \
    MODULE_DEF_NUMPY_LINALG \
    MODULE_DEF_NUMPY_RANDOM \
    MODULE_DEF_SCIPY \
    MODULE_DEF_SCIPY_INTEGRATE \
    MODULE_DEF_SCIPY_LINALG \
    MODULE_DEF_SCIPY_OPTIMIZE \
    MODULE_DEF_SCIPY_SIGNAL \
    MODULE_DEF_SCIPY_SPECIAL \
    MODULE_DEF_MACHINE \
// MICROPY_REGISTERED_EXTENSIBLE_MODULES

extern void mp_module_sys_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
#define MICROPY_MODULE_DELEGATIONS \
    { MP_ROM_PTR(&mp_module_sys), mp_module_sys_attr }, \
// MICROPY_MODULE_DELEGATIONS
