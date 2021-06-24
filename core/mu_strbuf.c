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

mu_strbuf_t *mu_strbuf_init_ro(mu_strbuf_t *buf,
                               const uint8_t *const rdata,
                               size_t capacity) {
  buf->rdata = rdata;
  buf->capacity = capacity;
  return buf;
}

mu_strbuf_t *mu_strbuf_init_wr(mu_strbuf_t *buf,
                               uint8_t *wdata,
                               size_t capacity) {
  buf->wdata = wdata;
  buf->capacity = capacity;
  return buf;
}

mu_strbuf_t *mu_strbuf_init_from_cstr(mu_strbuf_t *buf,
                                      const char *const cstr) {
  return mu_strbuf_init_ro(buf, (const uint8_t *const)cstr, strlen(cstr));
}

const uint8_t *const mu_strbuf_rdata(const mu_strbuf_t *buf) {
  return buf->rdata;
}

uint8_t *mu_strbuf_wdata(const mu_strbuf_t *buf) { return buf->wdata; }

size_t mu_strbuf_capacity(const mu_strbuf_t *const buf) {
  return buf->capacity;
}

// =============================================================================
// local (static) code
