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

// =============================================================================
// includes

#include "mu_strbuf.h"
#include "mu_strref.h"
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

// #include <stdio.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

void get_internal(mu_strref_t *r, mu_strbuf_data_t *d);

void put_internal(mu_strref_t *r, mu_strbuf_data_t d);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_strref_t *mu_strref_init(mu_strref_t *r, mu_strbuf_t *s, bool is_readonly) {
  r->s = s;
  r->capacity = mu_strbuf_capacity(s);
  r->is_readonly = is_readonly;
  return mu_strref_reset(r);
}

mu_strref_t *mu_strref_reset(mu_strref_t *r) {
  r->get_i = 0;
  if (mu_strref_is_readonly(r)) {
    r->put_i = mu_strref_capacity(r);
  } else {
    r->put_i = 0;
  }
  return r;
}

mu_strbuf_t *mu_strref_strbuf(mu_strref_t *r) {
  return r->s;
}

bool mu_strref_is_readonly(mu_strref_t *r) {
  return r->is_readonly;
}

size_t mu_strref_capacity(mu_strref_t *r) {
  if (mu_strref_is_readonly(r)) {
    return r->capacity;
  } else {
    return r->capacity - 1;
  }
}

/**
 *    0  1  2
 *    gp       count = 0
 *    g  p     count = 1
 *    g     p  count = 2
 *       g  p  count = 1
 *          gp count = 0
 *    p     g  count = 1
 *       p  g  count = 2
 */
// until I can decide on a name for "how many gets are possible" and "how many
// puts are possible"...
//
// size_t mu_strref_count(mu_strref_t *r) {
//   size_t p = r->put_i;
//   if (p < r->get_i) {
//     p += r->capacity;
//   }
//   return p - r->get_i;
//   }
// }

bool mu_strref_can_get(mu_strref_t *r) {
  return r->get_i != r->put_i;
}

bool mu_strref_can_put(mu_strref_t *r) {
  if (mu_strref_is_readonly(r)) {
    return false;
  } else {
    size_t p = r->put_i;
    if (p < r->get_i) {
      p += r->capacity;
    }
    // here p is greater than or equal to r->get_i. (p - r->get_i) is the
    // number of bytes in the buffer.  And since p == r->get_i is used to
    // signify an empty buffer, the effective capacity is one less.
    return (p - r->get_i) < (r->capacity - 1);
  }
}

mu_strref_err_t mu_strref_get(mu_strref_t *r, mu_strbuf_data_t *d) {
  if (!mu_strref_can_get(r)) {
    *d = '\0';
    return MU_STRREF_ERR_EMPTY;
  }
  get_internal(r, d);
  return MU_STRREF_ERR_NONE;
}

mu_strref_err_t mu_strref_put(mu_strref_t *r, mu_strbuf_data_t d) {
  if (!mu_strref_can_put(r)) {
    return MU_STRREF_ERR_FULL;
  }
  put_internal(r, d);
  return MU_STRREF_ERR_NONE;
}

size_t mu_strref_append(mu_strref_t *dst, mu_strref_t *src) {
  mu_strbuf_data_t d;
  size_t copied = 0;
  while (mu_strref_can_get(src) && mu_strref_can_put(dst)) {
    get_internal(src, &d);
    put_internal(dst, d);
    copied += 1;
  }
  return copied;
}

size_t mu_strref_puts(mu_strref_t *r, const char *str) {
  size_t copied = 0;

  while (*str != '\0' && mu_strref_can_put(r)) {
    put_internal(r, *str++);
    copied += 1;
  }
  return copied;
}

// TODO: consider adding
// size_t mu_strref_getn(mu_strref_t *r, char *dst, size_t n);

// =============================================================================
// local (static) code

void get_internal(mu_strref_t *r, mu_strbuf_data_t *d) {
  *d = r->s->data[r->get_i++];
  if (r->get_i > r->capacity) {
    r->get_i = 0;
  }
}

void put_internal(mu_strref_t *r, mu_strbuf_data_t d) {
  r->s->data[r->put_i++] = d;
  if (r->put_i > r->capacity) {
    r->put_i = 0;
  }
}
