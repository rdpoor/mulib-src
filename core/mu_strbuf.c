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
#include <stddef.h>
#include <string.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

// =============================================================================
// local storage

// =============================================================================
// public code

mu_strbuf_t *mu_strbuf_init(mu_strbuf_t *s, mu_strbuf_data_t *data, size_t capacity) {
  s->data = data;
  s->capacity = capacity;
  return s;
}

mu_strbuf_t *mu_strbuf_init_from_cstr(mu_strbuf_t *s, const char *cstr) {
  s->data = (mu_strbuf_data_t *)cstr;
  s->capacity = strlen(cstr);  // does not include null termination
  return s;
}

mu_strbuf_t *mu_strbuf_to_cstr(mu_strbuf_t *s, char *cstr, size_t cstr_length) {
  memmove(cstr, s->data, cstr_length-1); // leave one byte for null termination
  cstr[cstr_length-1] = '\0';            // null terminate
  return s;
}

size_t mu_strbuf_capacity(mu_strbuf_t *s) {
  return s->capacity;
}

mu_strbuf_data_t *mu_strbuf_data(mu_strbuf_t *s) {
  return s->data;
}

mu_strbuf_err_t mu_strbuf_ref(mu_strbuf_t *s, size_t index, mu_strbuf_data_t **p) {
  if (index >= s->capacity) {
    *p = NULL;
    return MU_STR_ERR_INDEX;
  } else {
    *p = &s->data[index];
    return MU_STR_ERR_NONE;
  }
}

mu_strbuf_err_t mu_strbuf_get(mu_strbuf_t *s, size_t index, mu_strbuf_data_t *d) {
  if (index >= s->capacity) {
    *d = '\0';
    return MU_STR_ERR_INDEX;
  } else {
    *d = s->data[index];
    return MU_STR_ERR_NONE;
  }
}

mu_strbuf_err_t mu_strbuf_put(mu_strbuf_t *s, size_t index, mu_strbuf_data_t d) {
  if (index >= s->capacity) {
    return MU_STR_ERR_INDEX;
  } else {
    s->data[index] = d;
    return MU_STR_ERR_NONE;
  }
}

// =============================================================================
// local (static) code
