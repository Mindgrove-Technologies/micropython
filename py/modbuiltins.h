/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <assert.h>

#include "py/smallint.h"
#include "py/objint.h"
#include "py/objstr.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/stream.h"



 mp_obj_t mp_builtin___build_class__(size_t n_args, const mp_obj_t *args);

 mp_obj_t mp_builtin_abs(mp_obj_t o_in);

 mp_obj_t mp_builtin_all(mp_obj_t o_in);

 mp_obj_t mp_builtin_any(mp_obj_t o_in);

 mp_obj_t mp_builtin_bin(mp_obj_t o_in);

 mp_obj_t mp_builtin_callable(mp_obj_t o_in);

 mp_obj_t mp_builtin_chr(mp_obj_t o_in);

 mp_obj_t mp_builtin_dir(size_t n_args, const mp_obj_t *args) ;

 mp_obj_t mp_builtin_divmod(mp_obj_t o1_in, mp_obj_t o2_in) ;
 mp_obj_t mp_builtin_hash(mp_obj_t o_in) ;

 mp_obj_t mp_builtin_hex(mp_obj_t o_in);


#include "py/mphal.h"
#include "shared/readline/readline.h"

// A port can define mp_hal_readline if they want to use a custom function here
#ifndef mp_hal_readline
#define mp_hal_readline readline
#endif

 mp_obj_t mp_builtin_input(size_t n_args, const mp_obj_t *args) ;

 mp_obj_t mp_builtin_iter(mp_obj_t o_in) ;


 mp_obj_t mp_builtin_min_max(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs, mp_uint_t op);

 mp_obj_t mp_builtin_max(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);
 mp_obj_t mp_builtin_min(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);

//  mp_obj_t mp_builtin_next(size_t n_args, const mp_obj_t *args);


 mp_obj_t mp_builtin_next(mp_obj_t o);

 mp_obj_t mp_builtin_oct(mp_obj_t o_in);

 mp_obj_t mp_builtin_ord(mp_obj_t o_in);
 mp_obj_t mp_builtin_pow(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_builtin_print(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
 mp_obj_t mp_builtin___repl_print__(mp_obj_t o);
 mp_obj_t mp_builtin_repr(mp_obj_t o_in) ;
 mp_obj_t mp_builtin_round(size_t n_args, const mp_obj_t *args);
 mp_obj_t mp_builtin_sum(size_t n_args, const mp_obj_t *args);
 mp_obj_t mp_builtin_sorted(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);
 mp_obj_t mp_builtin_getattr(size_t n_args, const mp_obj_t *args);
 mp_obj_t mp_builtin_setattr(mp_obj_t base, mp_obj_t attr, mp_obj_t value);

 mp_obj_t mp_builtin_delattr(mp_obj_t base, mp_obj_t attr);


 mp_obj_t mp_builtin_hasattr(mp_obj_t object_in, mp_obj_t attr_in);
 mp_obj_t mp_builtin_globals(void);
 mp_obj_t mp_builtin_locals(void);
