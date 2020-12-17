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

#ifndef _MU_STRREF_H_
#define _MU_STRREF_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include "mu_strbuf.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// =============================================================================
// types and definitions

typedef enum {
  MU_STRREF_ERR_NONE,
  MU_STRREF_ERR_FULL,
  MU_STRREF_ERR_EMPTY
} mu_strref_err_t;

typedef struct {
  mu_strbuf_t *s;   // backing store
  size_t get_i;     // get index (where next char is fetched)
  size_t put_i;     // put endex (where next char is stored)
  size_t capacity;  // cached from strbuf
  bool is_readonly; // true if this is a read-only strref
} mu_strref_t;

// =============================================================================
// declarations

/**
 * Initialize a string reference object to point to an underlying string buffer.
 * It is an error to initialize a strref with a strbuf whose capacity is zero.
 */
mu_strref_t *mu_strref_init(mu_strref_t *r, mu_strbuf_t *s, bool is_readonly);

/**
 * For a readonly strref, set get and put pointers to th start and end,
 * respectively, of the underlying strbuf.  Otherwise set get and put
 * pointers to zero.
 */
mu_strref_t *mu_strref_reset(mu_strref_t *r);

/**
 * Return the underlying strbuf.
 */
mu_strbuf_t *mu_strref_strbuf(mu_strref_t *r);

/**
 * Return true if this is readonly strref.
 */
bool mu_strref_is_readonly(mu_strref_t *r);

/**
 * Return the number of bytes in the underlying strbuf.
 */
size_t mu_strref_capacity(mu_strref_t *r);

/**
 * Return true if mu_strref_get() will succeed.
 */
bool mu_strref_can_get(mu_strref_t *r);

/**
 * Return true if mu_strref_put() will succeed.
 */
bool mu_strref_can_put(mu_strref_t *r);

/**
 * Read a byte from the buffer.  Returns an error code if the byte cannot be
 * read.
 */
mu_strref_err_t mu_strref_get(mu_strref_t *r, mu_strbuf_data_t *d);

/**
 * Write a byte into the buffer.  Returns an error code if the byte cannot be
 * written.
 */
mu_strref_err_t mu_strref_put(mu_strref_t *r, mu_strbuf_data_t d);

/**
 * Append the contents of src to dst.  Returns the number of bytes transferred.
 */
size_t mu_strref_append(mu_strref_t *dst, mu_strref_t *src);

/**
 * @brief Append a null-terminated C-style string to strref.  Returns the number
 * of bytes transferred.
 */
size_t mu_strref_puts(mu_strref_t *dst, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_STRREF_H_ */
