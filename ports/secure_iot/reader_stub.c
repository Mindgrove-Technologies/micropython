#include "py/reader.h"
#include "py/lexer.h"
#include "py/mperrno.h"
#include "py/runtime.h"

// mp_reader_t mp_reader_new_file(const char *filename) {
//     // Return a dummy reader or throw error
//     mp_reader_t reader = {0};
//     mp_raise_OSError(MP_ENOENT); // No such file
//     return reader;
// }

// mp_lexer_t mp_reader_new_file (const char* filename){
//     mp_raise_OSError(MP_ENOENT);
// }

void mp_reader_new_file(mp_reader_t *reader,qstr filename){
    mp_raise_OSError(MP_ENOENT);
}