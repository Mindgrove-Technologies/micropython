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
