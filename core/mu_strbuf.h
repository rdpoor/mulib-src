/**
 * MIT License
 *
 * Copyright (c) 2020 R. Dunbar Poor <rdpoor@gmail.com>
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

#ifndef _MU_STRBUF_H_
#define _MU_STRBUF_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// types and definitions

typedef char mu_strbuf_data_t;

typedef enum {
  MU_STR_ERR_NONE,
  MU_STR_ERR_INDEX
} mu_strbuf_err_t;

typedef struct {
  mu_strbuf_data_t *data;  // backing store
  size_t capacity;      // length of backing store
} mu_strbuf_t;


// =============================================================================
// declarations

mu_strbuf_t *mu_strbuf_init(mu_strbuf_t *s, mu_strbuf_data_t *data, size_t capacity);

mu_strbuf_t *mu_strbuf_init_from_cstr(mu_strbuf_t *s, const char *cstr);

mu_strbuf_t *mu_strbuf_to_cstr(mu_strbuf_t *s, char *cstr, size_t cstr_length);

size_t mu_strbuf_capacity(mu_strbuf_t *s);

mu_strbuf_data_t *mu_strbuf_data(mu_strbuf_t *s);

mu_strbuf_err_t mu_strbuf_ref(mu_strbuf_t *s, size_t index, mu_strbuf_data_t **p);

mu_strbuf_err_t mu_strbuf_get(mu_strbuf_t *s, size_t index, mu_strbuf_data_t *d);

mu_strbuf_err_t mu_strbuf_put(mu_strbuf_t *s, size_t index, mu_strbuf_data_t d);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_STRBUF_H_ */
