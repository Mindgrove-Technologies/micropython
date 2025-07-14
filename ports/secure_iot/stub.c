#include "py/runtime.h"
#include "py/compile.h"
#include "py/obj.h"
#include "py/builtin.h"

 mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}
#include "py/qstr.h"

const qstr_pool_t mp_qstr_frozen_const_pool = {
    .prev = NULL,
    .total_prev_len = 0,
    .len = 0,
    .alloc = 0,
    .qstrs = {},
};
/*When you do import foo in Python, MicroPython may call mp_import_stat("foo.py") to check if a file or module exists.
The return type mp_import_stat_t is an enum with values like:
MP_IMPORT_STAT_NO_EXIST (does not exist)
MP_IMPORT_STAT_FILE (is a file)
MP_IMPORT_STAT_DIR (is a directory)
Defines an empty QSTR pool for "frozen" constants (precompiled Python code embedded in the firmware).
qstr_pool_t is a structure that holds a pool of unique strings used by MicroPython for efficient string handling.

*/

