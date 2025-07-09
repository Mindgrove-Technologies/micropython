#include "py/compile.h" //It is there 
#include "py/runtime.h"
#include "py/repl.h"
#include "py/lexer.h"
#include "py/parse.h"
#include "py/stackctrl.h"
#include "py/gc.h"
//#include "py/pyexec.h"
#include "shared/runtime/pyexec.h"
#include "shared/runtime/gchelper.h"
#include <string.h>
#include "uart.h"
#include "py/qstr.h"
//#include <stdio.h>

#include"uart.h"
#include"io.h"
#include"secure_iot.h"
#include"gptimer.h"
#include"plic.h"
#include "log.h"
#include "utils.h"
//-----------------------------
//For the repl to work
#include "py/mpprint.h"
#include "shared/readline/readline.h"
#include "py/mpstate.h"
#include "genhdr/moduledefs.h"

extern vstr_t line; //line object created for the working
//----------------------------------

//static char heap[4096];
//Initial setup of micropyton :/
static char heap[25600];
extern void qstr_init(void);
//Same size as the one given in mpconfigport 
//printf("Looking for: %s\n", qstr_str(mymodule));
#include "py/objstr.h" // Needed for qstr_str()
//printf("Looking for: %s\n", qstr_str(MP_QSTR_mymodule));

extern const struct _mp_obj_module_t *__modules_start;
extern const struct _mp_obj_module_t *__modules_code_end;

void mp_module_init_from_section(void) {
    const struct _mp_obj_module_t **mod;

    for (mod = &__modules_start; mod < &__modules_code_end; ++mod) {
        mp_obj_t mod_obj = MP_OBJ_FROM_PTR(*mod);
        //Iterates over statically linked modules placed in the .modules section
        mp_map_elem_t *name_elem = mp_map_lookup(
            &((mp_obj_module_t *)(*mod))->globals->map,
            MP_OBJ_NEW_QSTR(MP_QSTR___name__),
            MP_MAP_LOOKUP
            //If it's a valid QSTR, stores the module into sys.modules
        );
        printf("MOD = %p\n", mod); //to show the value pointed (address in the modules section)
        printf("*MOD = %p\n", *mod);
        if (*mod == NULL) continue; 
        printf("MOD->globals = %p\n", (*mod)->globals);
        if (name_elem && mp_obj_is_qstr(name_elem->value)) {
            qstr mod_qstr = mp_obj_str_get_qstr(name_elem->value);
            //Initialized (qstr_init())
            //Contain MP_QSTR_mymodule, MP_QSTR___name__,
            mp_obj_dict_store(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_loaded_modules_dict)),
            //// dictionary with loaded modules (may be exposed as sys.modules)
                              MP_OBJ_NEW_QSTR(mod_qstr), mod_obj);
        }
    }
}
/*the code you're seeing is entering a POSIX-style system call handler via ecall, specifically for the _fstat function. This typically happens when MicroPython (or C stdlib) attempts to query the status of a file descriptor, such as checking if stdin is a terminal.*/



size_t my_strlen(const char *str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

int main(void) {
    short* baud =0x11300;
    *baud=16;
    short* value=0x11304;
    *value=65;
    //millis_init(); //for turning on the clock for reference
    /*
        // Set MSIP register to trigger a software interrupt
    volatile uint64_t* umsip     = (volatile uint64_t*)0x02000000;
    volatile uint64_t* umtime    = (volatile uint64_t*)0x0200BFF8;
    volatile uint64_t* umtimecmp = (volatile uint64_t*)0x02004000;
    *umsip = 0x01;
	//printf("msip: %p, mtime: %p, mtimecmp: %p\n", msip, mtime, mtimecmp);
	//uint64_t val;
    // Try reading
    // val = *msip;
    // printf("msip value: %llx\n", val);
    // val = *mtime;
    // printf("mtime value: %llx\n", val);
    // val = *mtimecmp;
    // printf("mtimecmp value: %llx\n", val);

    // Disable specific interrupts (for demonstration; adjust as needed)
    asm volatile("li      t0, 0xc0\t\n\r"    // Load immediate value
                 "csrrs   zero, mie, t0\t\n\r"); // Set bits in mie register
    // Enable global interrupts
    asm volatile("li      t0, 8\t\n\r"      // Load immediate value
                 "csrrs   zero, mstatus, t0\t\n\r"); // Set bits in mstatus register

    //last_count = * mtime; //changes made
    *umtimecmp = *umtime + 1500000;*/
    printf("hello");
    const qstr_pool_t *pool = MP_STATE_VM(last_pool);
    printf("QSTR POOL PTR = %p\n", pool);

    if (pool) {
        printf("POOL LEN = %lu\n", (unsigned long)pool->len);
        #if MICROPY_QSTR_BYTES_IN_HASH
            printf("POOL HASHES = %p\n", (void*)pool->hashes);
            //If the hashes are not enabled , It is compiled without space for hashes to save memory
        #endif

        printf("POOL QSTRS = %p\n", (void*)pool->qstrs);
    } else {
        printf("QSTR POOL is NULL\n");
    }
    // STATIC const mp_rom_map_elem_t test_module_globals_table[] = {
    // { MP_ROM_QSTR(MP_QSTR_testfunc), MP_ROM_PTR(&mp_const_none_obj) },
    // };
    //Making of the uart object:
    //UART_Config_t uart; /*Structure variable which hold the UART's specifications */
    //UART_Config_t *UART_Config_1=&uart; /*Creating a pointer to the above stucture variable*/
    extern UART_Config_t* UART_Config_1;
    UART_Config_1->baudrate=117186;
    UART_Config_1->uart_num = 0;  // or 1/2 if UART0 is reserved for log/printf
    // UART_Config_1->buffer_enable = 0;  // or 1 if you want buffering
    // UART_Config_1->char_size=8;
    // UART_Config_1->stop_bits=0;
    // UART_Config_1->parity=1;
    // UART_Init(UART_Config_1);  // ← this must happen before ANY UART use


    mp_stack_ctrl_init();
    mp_stack_set_limit(131072);//4kb stack limit
    //mp_stack_set_limit(40000 * (BYTES_PER_WORD / 4));
    gc_init(heap, heap + sizeof(heap));
    qstr_init(); //No issues till here
    //readline_init0();
    mp_init();
    #if MICROPY_MODULE_BUILTIN_INIT
    mp_module_init_from_section(); //--> causes an error
        //mp_module_init();
        //mp_module_init_from_section();
    // mp_obj_t mymodule_obj = mp_import_name(MP_QSTR_mymodule, mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
    // mp_obj_t globals = mp_obj_module_get_globals(mymodule_obj);
    // mp_map_t *map = mp_obj_dict_get_map(MP_OBJ_TO_PTR(globals));
    // mp_map_elem_t *elem = mp_map_lookup(map, MP_OBJ_NEW_QSTR(MP_QSTR_myvar), MP_MAP_LOOKUP);
    // if (elem) {
    //     mp_obj_t value = elem->value;
    //     // Use value (e.g., print it)
    // } else {
    //     printf("myvar not found!\n");
    // // }

    // mp_deinit();
    // return 0;
    // }

    #endif
    //mp_obj_list_init(mp_sys_path, 0);
    //mp_obj_list_init(mp_sys_argv, 0);

    const char *src = "print('Hello from fing micro python !')";
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
   // mp_obj_t module_fun = mp_compile(&parse_tree, lex->source_name, MP_EMIT_OPT_NONE, true);
    mp_obj_t module_fun = mp_compile(&parse_tree, lex->source_name, true);
   
    //mp_call_function_0(module_fun);
    if (module_fun == MP_OBJ_NULL) {
    printf("Compilation failed.\n");
    return -1;
    }
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // No exception
        mp_call_function_0(module_fun);
        nlr_pop();
        } 
    else {
        // Uncaught exception landed here
        mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }
    pyexec_friendly_repl(); //comment out if repl is not needed
    //gc.threshold(gc.mem_free()) // 4 + gc.mem_alloc())
    char str[10] = "MINDGROVE";
    int len = my_strlen(str);
    char new[100];/*Declared a string variable to receive */
    //UART_Write((UART_Config_t*)UART_Config_1,str,len);
    UART_Write((UART_Config_t *)UART_Config_1, (uint8_t *)str, len);
    UART_Write_Wait((UART_Config_t*)UART_Config_1);//Wait till the uart transmission.
    UART_Read_String((UART_Config_t*)UART_Config_1,new);
    printf("\nstring1 :%s\n", new);
    
    //This will provoke a GC when more than 25% of the currently free heap becomes occupied.
    if (lex == NULL) {
        printf("Failed to create lexer\n");
        return -1;
    }

    // Deinitialise the runtime.
    gc_sweep_all(); //A garbage collection can be demanded at any time using a garbage collector

    mp_deinit();
    return 0;
}

// Handle uncaught exceptions (should never be reached in a correct C implementation).
void nlr_jump_fail(void *val) {
    for (;;) {
    }
}

// Do a garbage collection cycle.
void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}


#if !MICROPY_DEBUG_PRINTERS
// With MICROPY_DEBUG_PRINTERS disabled DEBUG_printf is not defined but it
// is still needed by esp-open-lwip for debugging output, so define it here.
//#include <stdarg.h> //Function used to handle variable number of arguments
//int mp_vprintf(const mp_print_t *print, const char *fmt, va_list args);
//Where to send the output -> pointer to the console structure, formatte string ,argumnets passed as input
int DEBUG_printf(const char *fmt, ...) { //default
    va_list ap; //accessing the arguments in variabe 
    va_start(ap, fmt); //starts reading the arguments after fmt
    int ret = mp_printf(MICROPY_DEBUG_PRINTER, fmt, ap); //Internal function
    va_end(ap);
    return ret;
}
#endif
//for pretty printing of debug information
/*     text    data    bss     dec     hex filename
     10800  104    8272  19176  4a24 firmware.elf*/