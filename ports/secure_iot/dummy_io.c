#include "py/obj.h"
#include "py/runtime.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/objstr.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mperrno.h"


// mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
//     mp_raise_OSError(MP_ENOENT);
// }
mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    mp_raise_OSError(MP_ENOENT);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
// const mp_obj_type_t mp_type_stringio = {0};
// const mp_obj_type_t mp_type_bytesio = {0};
