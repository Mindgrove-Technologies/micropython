#include <stdint.h>

// options to control how MicroPython is built

// Use the minimal starting configuration (disables all optional features).
//#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_MINIMUM)
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES)
//#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES)
// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
//-----------------------------------------------------------------------------
#define MICROPY_PY_BUILTINS    (1)

#define MICROPY_PY_MYMODULE (1)
#define MODULE_MYMODULE_ENABLED (1)
#define MICROPY_QSTR_BYTES_IN_HASH (0)
//------------------------------------------------------------------------------------
#define MICROPY_MODULE_BUILTIN_INIT (1)
//------------------------------------------------------------------------------
#define MICROPY_PY_SYS           (1)
#define MICROPY_PY_ARRAY         (1)
#define MICROPY_PY_COLLECTIONS   (1)
#define MICROPY_PY_IO            (1)
#define MICROPY_PY_IO_FILEIO      (1)
#define MICROPY_PY_STRUCT        (1)
#define MICROPY_PY_GC            (1)
#define MICROPY_PY_MICROPYTHON   (1)
#define MICROPY_ENABLE_COMPILER     (1)
#define MICROPY_PY_ERRNO (1)

#undef MICROPY_QSTR_EXTRA_POOL 
//#define MICROPY_QSTR_EXTRA_POOL           mp_qstr_frozen_const_pool
//#define MICROPY_QSTR_EXTRA_POOL           NULL
//defined in frozen_mpy.c file in ports
#define MICROPY_ENABLE_GC                 (1)
#define MICROPY_HELPER_REPL               (1)
//#define MICROPY_MODULE_FROZEN_MPY         (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT    (1)
#define MICROPY_READER_POSIX               (0)
#define MICROPY_ENABLE_IMPORT_STAT         (0)
#define MICROPY_ALLOC_PATH_MAX            (256)
#define MICROPY_PY_MACHINE_UART            (1)
#define MICROPY_REPL_HISTORY (1)
#define MICROPY_READLINE_HISTORY_SIZE (8)

#define MICROPY_PY_MACHINE_UART_INCLUDEFILE "./uart.h"
// Use the minimum headroom in the chunk allocator for parse nodes.
//#define MICROPY_ALLOC_PARSE_CHUNK_INIT    (16)
#define MICROPY_ALLOC_QSTR_CHUNK_INIT (64)  // Increase from default 10
#define MICROPY_READER_VFS         (1)  // disables mp_lexer_new_from_file
#define MICROPY_PY_BUILTINS_OPEN    (1)
#define MICROPY_PY_BUILTINS_INPUT      (1)
#define MICROPY_PY_BUILTINS_PRINT      (1)
//#define MICROPY_PY_BUILTINS_STR_UNICODE (1)  // Enable Unicode support in str
#define MICROPY_PY_BUILTINS_EXEC (1)         // Optional: exec() function
#define MICROPY_PY_BUILTINS_EVAL (1)         // Optional: eval() function
//#define MICROPY_PY_BUILTINS_OPEN 1
#define MICROPY_ENABLE_FILE_IMPORT (1)  // disables mp_import_stat
#define MICROPY_MODULE_FROZEN_MPY  (0)
#define MICROPY_MODULE_FROZEN (0) //Have to disable this along witb the mpy file also
#define MICROPY_MODULE_FROZEN_STR  (0)
// type definitions for the specific machine
//#define MICROPY_ENABLE_COMPILER         (0)
#define MICROPY_MODULE_WEAK_LINKS       (0)
#define MICROPY_MODULE_BUILTIN_INIT      (1)
//For thye support of the filw sytems 
#define MICROPY_VFS               (0) //Adds posix implementation which causes error
#define MICROPY_VFS_FAT           (0)   // For FAT filesystem support
//#define MICROPY_MODULE_FROZEN_MPY       (0) //--As we are not using frozen mpy files but py files directly into the build system
#define MICROPY_PY_BUILTINS_HELP       (1)
//--------------------------------------------
#define MICROPY_MODULE_SYS        (1)
//#define MICROPY_PY_SYS_PATH        (1)
#define MICROPY_PY_SYS_ARGV         (1)
//#define MP_STATE_VM_SYS_MUTABLE      (1)
//#define MICROPY_PY_SYS_MUTABLE 1
//#define MP_SYS_MUTABLE_PATH  1 // Causes syntax error
//means that MP_SYS_MUTABLE_PATH is defined as a macro with a numeric value, but used as an enum identifier
//#define mp_sys_path (MP_STATE_VM(sys_mutable[MP__PATH]))
//#define mp_sys_argv (MP_STATE_VM(sys_mutable[MP_SYS_MUTABLE_ARGV]))

#define MP_STATE_PORT_ROOT_POINTERS \
    vstr_t *repl_line; \
    const char *readline_hist[MICROPY_READLINE_HISTORY_SIZE]; \
    .readline_hist = {0}; 
//setting up of the initial value of readline to take the value of 0    
//const char *readline_hist[MICROPY_READLINE_HISTORY_SIZE]; \

#define MICROPY_PY_SYS_STDIO_BUFFER  (1)
#define MICROPY_PY_SYS_STDOUT     (1)

typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;
//#define MP_STATE_PORT_VM 
    //void *uart0_rxbuf;
//we have to define uart0_rxbuf for future use
// We need to provide a declaration/definition of alloca()
#include <alloca.h>

// #define MICROPY_PORT_BUILTINS 
//    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

#define MICROPY_HW_BOARD_NAME "mindgrove-secure-iot"
#define MICROPY_HW_MCU_NAME "secure-iot"
// #define MP_STATE_PORT_ROOT_POINTERS 
// mp_obj_t uart0_rxbuf;
//Using a custom buffer
//Need to declare uart0_rxbuf in port specific mp_state_vm_t

#if defined(__linux__) || defined(__APPLE__)
#define MICROPY_MIN_USE_STDOUT (1)
#define MICROPY_HEAP_SIZE      (25*1024) // heap size 25 kilobytes
#endif

#ifdef __thumb__
//I dont need this BS
#define MICROPY_MIN_USE_CORTEX_CPU (1)
#define MICROPY_MIN_USE_STM32_MCU (1)
#define MICROPY_HEAP_SIZE      (2048) // heap size 2 kilobytes
#endif

//#define MP_STATE_PORT MP_STATE_VM
//----------------------------------------------------------------------------
// #define MICROPY_PY_TIME_TICKS      (1)
// #define MICROPY_PY_TIME            (1)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//For numpy and Scipy --> Support
#define ULAB_MAX_DIMS                (4)
#define ULAB_SUPPORTS_COMPLEX        (0)
#define ULAB_HAS_DTYPE_OBJECT        (1)
#define ULAB_HAS_SCIPY               (1)
#define ULAB_HAS_UTILS_MODULE        (1)
#define ULAB_NUMPY_HAS_FFT           (1)
#define ULAB_NUMPY_HAS_LINALG        (1)
#define ULAB_NUMPY_HAS_RANDOM        (1)
#define ULAB_NUMPY_HAS_IO            (1)
#define ULAB_NUMPY_HAS_CREATE_MODULE (1)
#define ULAB_NUMPY_HAS_NUMERICAL     (1)
//------------------------------------------------------------------------------
//math module in --> py/modmath.h
#define MICROPY_PY_MATH (1)
#define MICROPY_FLOAT_IMPL        (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_BUILTINS_FLOAT (1)
//for float implementation.
//--------------------------------------------------------------------------------
#define ULAB_SUPPORTS_COMPLEX     (0)
#define ULAB_MAX_DIMS             (4) //for random this support is needed
#define ULAB_HAS_DTYPE_OBJECT     (1)
#define ULAB_HAS_SCIPY            (1)
#define ULAB_HAS_UTILS_MODULE     (1)
//#define MICROPY_FLOAT_IMPL        (MICROPY_FLOAT_IMPL_DOUBLE) // or FLOAT
#define ULAB_NUMPY_HAS_RANDOM (1) //support for random module
#define ULAB_NUMPY_HAS_LINALG (1)
#define ULAB_NUMPY_HAS_FFT           (1)
#define ULAB_NUMPY_HAS_IO            (1)
#define ULAB_NUMPY_HAS_CREATE_MODULE (1)
#define ULAB_NUMPY_HAS_NUMERICAL     (1)

//--------------------------------------------------------------------------------
//for removing the complex value dependency
#define MICROPY_PY_BUILTINS_COMPLEX   (1)