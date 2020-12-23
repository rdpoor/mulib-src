/**
 * MIT License
 *
 * Copyright (c) 2020 R. D. Poor <rdpoor@gmail.com>
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _MU_THUNK_H_
#define _MU_THUNK_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include "mu_time.h"

// =============================================================================
// types and definitions

/**
 * A `mu_thunk` is a function that can be called later.  It comprises a function
 * pointer (`mu_thunk_fn`) and a context (`void *ctx`).  When called, the
 * function is passed the ctx argument and a caller-supplied `void *` argument.
 */

// The signature of a mu_thunk function.
typedef void *(*mu_thunk_fn)(void *ctx, void *arg);

typedef struct _mu_thunk {
  mu_thunk_fn fn; // function to call
  void *ctx;      // context to pass when called
} mu_thunk_t;

// =============================================================================
// Declarations

mu_thunk_t *mu_thunk_init(mu_thunk_t *thunk, mu_thunk_fn fn, void *ctx);

mu_thunk_fn mu_thunk_get_fn(mu_thunk_t *thunk);

void *mu_thunk_get_ctx(mu_thunk_t *thunk);

void *mu_thunk_call(mu_thunk_t *thunk, void *arg);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _MU_THUNK_H_
