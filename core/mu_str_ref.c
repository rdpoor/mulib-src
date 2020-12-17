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

#include "mu_str_ref.h"
#include "mu_str_rref.h"
#include "mu_str_wref.h"
#include "mulib.h"
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

mu_str_ref_t *mu_str_ref_init_for_read(mu_str_ref_t *ref, mu_str_rbuf_t *rbuf) {
  ref->rbuf = rbuf;
  return mu_str_ref_read_reset(ref);
}

mu_str_ref_t *mu_str_ref_init_for_write(mu_str_ref_t *ref, mu_str_wbuf_t *wbuf) {
  ref->wbuf = wbuf;
  return mu_str_ref_write_reset();
}

mu_str_ref_t *mu_str_ref_read_reset(mu_str_ref_t *ref) {
  ref->get_i = 0;
  ref->put_i = mu_str_rbuf_capacity(rbuf);
  return ref;
}

mu_str_ref_t *mu_str_ref_write_reset(mu_str_ref_t *ref) {
  ref->get_i = 0;
  ref->put_i = 0;
  return ref;
}

mu_str_ref_t *mu_str_ref_copy(mu_str_ref_t *dst, const mu_str_ref_t const *src) {
  return (mu_str_ref_t *)memcpy(dst, src, sizeof(mu_str_ref_t));
}

 mu_str_ref_t *mu_str_ref_slice(mu_str_ref_t *dst,
                                const mu_str_ref_t const *src,
                                int start,
                                int end) {
  size_t len = mu_str_readable_count(src);
  size_t s1, e1;

  mu_str_ref_copy(dst, src);

  if (start > 0) {
    s1 = start;
  } else {
    s1 = len + start;
  }

  if (end > 0) {
    e1 = end;
  } else {
    e1 = len + end;
  }

  // clamp.
  if (s1 > len) {
    s1 = len;
  }
  if (e1 < 0) {
    e1 = 0;
  }
  if (s1 > e1) {
    s1 = e1;
  }

  // convert relative s1, e1 to absolute get_i, put_i
  dst->get_i += s1;
  dst->put_i += e1;

  return dst;
}

size_t mu_str_ref_readabale_count(mu_str_ref_t *ref) {
  return ref->put_i - ref->get_i;
}

size_t mu_str_ref_writeable_count(mu_str_ref_t *ref) {
  return mu_str_wbuf_capacity(ref->wbuf) - ref->put_i;
}

bool mu_str_ref_read_byte(mu_str_ref_t *ref, uint8_t *byte) {
  if (mu_str_ref_readabale_count(ref) == 0) {
    return false;
  }
  *byte = ref->rbuf->store[ref->get_i++];
  return true;
}

bool mu_str_ref_write_byte(mu_str_ref_t *ref, uint8_t byte) {
  if (mu_str_ref_writeable_count(ref) == 0) {
    return false;
  }
  ref->wbuf->store[ref->put_i++] = byte;
  return true;
}

size_t mu_str_ref_append(mu_str_ref_t *dst, const mu_str_ref const *src) {
  // optimize later...
  uint8_t byte;
  size_t count = 0;
  while (mu_str_buf_read_byte(src, &byte)) {
    if (!mu_str_ref_write_byte(dst, buf)) {
      break;
    }
    count += 1;
  }
  return count;
}

size_t mu_str_ref_printf(mu_str_ref_t *dst, const char *fmt, ...) {
  size_t writeable = mu_str_ref_writeable_count(dst);

  if (writeable > 0) {
    size_t written;
    va_list ap;
    va_start(ap, fmt);
    written = vsnprintf(&dst->wbuf->store[dst->put_i], writeable, fmt, ap);
    va_end(ap);
    // Note: written is the number of bytes that *would* have been written if
    // there was enough room, NOT the number of bytes actually written.
    if (written > writeable) {
      written = writeable;
    }
    dst->put_i += written;
  }
  return dst;
}

size_t mu_str_ref_to_cstr(const mu_str_ref_t const *src,
                          uint8_t *buf, size_t buflen) {
  size_t copied = mu_str_ref_readabale_count(src);
  if (copied > buflen - 1) {
    copied = buflen - 1;      // always leave room for null termination
  }
  memcpy(buf, &src->rbuf->store[src->get_i], copied);
  buf[copied] = '\0';         // null terminate

  return copied-1;     // return # of bytes copied, not including null
}

// =============================================================================
// local (static) code
