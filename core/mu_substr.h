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

#ifndef _MU_SUBSTR_H_
#define _MU_SUBSTR_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "mu_strbuf.h"

// =============================================================================
// types and definitions

typedef enum {
  MU_SUBSTR_ERR_NONE,
  MU_SUBSTR_ERR_INDEX
} mu_substr_err_t;

typedef struct {
  mu_strbuf_t *str;      // backing store
  size_t start;       // starting index (inclusive)
  size_t end;         // ending index (exclusive)
} mu_substr_t;


// =============================================================================
// declarations

mu_substr_t *mu_substr_init(mu_substr_t *s, mu_strbuf_t *str);

mu_substr_t *mu_substr_duplicate(mu_substr_t *dst, mu_substr_t *src);

mu_substr_t *mu_substr_reset(mu_substr_t *s);

mu_substr_t *mu_substr_clear(mu_substr_t *s);

mu_strbuf_t *mu_substr_str(mu_substr_t *s);

size_t mu_substr_start(mu_substr_t *s);

size_t mu_substr_end(mu_substr_t *s);

size_t mu_substr_length(mu_substr_t *s);

size_t mu_substr_remaining(mu_substr_t *s);

mu_substr_err_t mu_substr_ref(mu_substr_t *s, size_t index, mu_strbuf_data_t **p);

mu_substr_err_t mu_substr_get(mu_substr_t *s, size_t index, mu_strbuf_data_t *d);

mu_substr_err_t mu_substr_put(mu_substr_t *s, size_t index, mu_strbuf_data_t d);

mu_substr_err_t mu_substr_slice_str(mu_substr_t *s, int start, int end);

mu_substr_t *mu_substr_copy(mu_substr_t *dst, mu_substr_t *src);

int mu_substr_cmp(mu_substr_t *s1, mu_substr_t *s2);

bool mu_substr_equals(mu_substr_t *s1, mu_substr_t *s2);

mu_substr_t *mu_substr_append(mu_substr_t *dst, mu_substr_t *src);

mu_substr_t *mu_substr_strstr(mu_substr_t *dst, mu_substr_t *needle, mu_substr_t *haystack);

mu_substr_t *mu_substr_to_cstr(mu_substr_t *src, char *cstr, size_t cstr_length);

/**
 * @brief Append formatted data to substring, increment end pointer.
 */
mu_substr_t *mu_substr_printf(mu_substr_t *dst, const char *fmt, ...);

/**
 * @brief Append a C-style string to substring, incrementing end pointer.
 */
mu_substr_t *mu_substr_puts(mu_substr_t *dst, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_SUBSTR_H_ */
