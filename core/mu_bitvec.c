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

#include "mu_bitvec.h"
#include <stdlib.h>
#include <stddef.h>

// =============================================================================
// local types and definitions

#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif

// signature for write_bit_op function
typedef void (*bit_op)(uint8_t *p, uint8_t mask);

// =============================================================================
// local (forward) declarations

static mu_bitvec_t *write_bit_op(mu_bitvec_t *bv, int index, bit_op op);
static mu_bitvec_t *write_all_op(mu_bitvec_t *bv, bit_op op);
static void set_bit_op(uint8_t *p, uint8_t mask);
static void clear_bit_op(uint8_t *p, uint8_t mask);
static void toggle_bit_op(uint8_t *p, uint8_t mask);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_bitvec_t *mu_bitvec_init(mu_bitvec_t *bv, size_t length, uint8_t *bits, size_t byte_count) {
  bv->length = length;
  bv->byte_count = byte_count;
  bv->bits = bits;
  return mu_bitvec_clear_all(bv);
}

uint8_t *mu_bitvec_bits(mu_bitvec_t *bv) {
  return bv->bits;
}

size_t mu_bitvec_length(mu_bitvec_t *bv) {
  return bv->length;
}

size_t mu_bitvec_byte_count(mu_bitvec_t *bv) {
  return bv->byte_count;
}

// operations on single bits. Return null on indexing error.

mu_bitvec_t *mu_bitvec_write_bit(mu_bitvec_t *bv, int i, bool value) {
  if (value) {
    return mu_bitvec_set_bit(bv, i);
  } else {
    return mu_bitvec_clear_bit(bv, i);
  }
}

bool mu_bitvec_read_bit(mu_bitvec_t *bv, int index) {
  uint8_t *b, m;
  if (index >= bv->length) {
    return false;  // silent error
  }
  b = &(bv->bits[index/8]);
  m = 1<<(index & 7);
  return *b & m;
}

mu_bitvec_t *mu_bitvec_set_bit(mu_bitvec_t *bv, int i) {
  return write_bit_op(bv, i, set_bit_op);
}

mu_bitvec_t *mu_bitvec_clear_bit(mu_bitvec_t *bv, int i) {
  return write_bit_op(bv, i, clear_bit_op);
}

mu_bitvec_t *mu_bitvec_toggle_bit(mu_bitvec_t *bv, int i) {
  return write_bit_op(bv, i, toggle_bit_op);
}

mu_bitvec_t *mu_bitvec_set_all(mu_bitvec_t *bv) {
  return write_all_op(bv, set_bit_op);
}

mu_bitvec_t *mu_bitvec_clear_all(mu_bitvec_t *bv) {
  return write_all_op(bv, clear_bit_op);
}

mu_bitvec_t *mu_bitvec_toggle_all(mu_bitvec_t *bv) {
  return write_all_op(bv, toggle_bit_op);
}

/**
 * Copy the bits contents of src to dst.  In the event that the bit vectors are
 * different lengths, only the bits in the shorter vector are copied.
 */
mu_bitvec_t *mu_bitvec_copy(mu_bitvec_t *dst, mu_bitvec_t *src) {
  size_t dst_len = mu_bitvec_length(dst);
  size_t src_len = mu_bitvec_length(src);
  size_t len = MIN(dst_len, src_len);
  for (int i=0; i<len; i++) {
    mu_bitvec_write_bit(dst, i, mu_bitvec_read_bit(src, i));
  }
  return dst;
}

mu_bitvec_t *mu_bitvec_and(mu_bitvec_t *dst, mu_bitvec_t *src) {
  size_t dst_len = mu_bitvec_length(dst);
  size_t src_len = mu_bitvec_length(src);
  size_t len = MIN(dst_len, src_len);
  for (int i=0; i<len; i++) {
    // could be much, much more efficient.
    bool value = mu_bitvec_read_bit(dst, i) & mu_bitvec_read_bit(src, i);
    mu_bitvec_write_bit(dst, i, value);
  }
  return dst;
}
mu_bitvec_t *mu_bitvec_or(mu_bitvec_t *dst, mu_bitvec_t *src) {
  size_t dst_len = mu_bitvec_length(dst);
  size_t src_len = mu_bitvec_length(src);
  size_t len = MIN(dst_len, src_len);
  for (int i=0; i<len; i++) {
    // could be much, much more efficient.  and compact.
    bool value = mu_bitvec_read_bit(dst, i) | mu_bitvec_read_bit(src, i);
    mu_bitvec_write_bit(dst, i, value);
  }
  return dst;
}

mu_bitvec_t *mu_bitvec_xor(mu_bitvec_t *dst, mu_bitvec_t *src) {
  size_t dst_len = mu_bitvec_length(dst);
  size_t src_len = mu_bitvec_length(src);
  size_t len = MIN(dst_len, src_len);
  for (int i=0; i<len; i++) {
    // could be much, much more efficient.  and compact.
    bool value = mu_bitvec_read_bit(dst, i) ^ mu_bitvec_read_bit(src, i);
    mu_bitvec_write_bit(dst, i, value);
  }
  return dst;
}

bool mu_bitvec_equals(mu_bitvec_t *bv1, mu_bitvec_t *bv2) {
  size_t bv1_len = mu_bitvec_length(bv1);
  size_t bv2_len = mu_bitvec_length(bv2);
  size_t len = MIN(bv1_len, bv2_len);
  for (int i=0; i<len; i++) {
    if (mu_bitvec_read_bit(bv1, i) != mu_bitvec_read_bit(bv2, i)) {
      return false;
    }
  }
  return true;
}

bool mu_bitvec_is_all_ones(mu_bitvec_t *bv) {
  size_t len = mu_bitvec_length(bv);
  for (int i=0; i<len; i++) {
    if (mu_bitvec_read_bit(bv, i) == false) {
      return false;
    }
  }
  return true;
}

bool mu_bitvec_is_all_zeros(mu_bitvec_t *bv) {
  size_t len = mu_bitvec_length(bv);
  for (int i=0; i<len; i++) {
    if (mu_bitvec_read_bit(bv, i) == true) {
      return false;
    }
  }
  return true;
}

int mu_bitvec_count_ones(mu_bitvec_t *bv) {
  size_t len = mu_bitvec_length(bv);
  int total = 0;
  for (int i=0; i<len; i++) {
    if (mu_bitvec_read_bit(bv, i) == true) {
      total += 1;
    }
  }
  return total;
}

int mu_bitvec_count_zeros(mu_bitvec_t *bv) {
  size_t len = mu_bitvec_length(bv);
  int total = 0;
  for (int i=0; i<len; i++) {
    if (mu_bitvec_read_bit(bv, i) == false) {
      total += 1;
    }
  }
  return total;
}

// =============================================================================
// local (static) code

static mu_bitvec_t *write_bit_op(mu_bitvec_t *bv, int index, bit_op op) {
  uint8_t *b, m;
  if (index >= bv->length) {
    return NULL;
  }
  b = &(bv->bits[index/8]);
  m = 1<<(index & 7);
  op(b, m);
  return bv;
}

static mu_bitvec_t *write_all_op(mu_bitvec_t *bv, bit_op op) {
  uint8_t *b, m;
  for (int index=0; index<bv->length; index++) {
    b = &(bv->bits[index/8]);
    m = 1<<(index & 7);
    op(b, m);
  }
  return bv;
}

static void set_bit_op(uint8_t *p, uint8_t mask) {
  *p |= mask;
}

static void clear_bit_op(uint8_t *p, uint8_t mask) {
  *p &= ~mask;
}

static void toggle_bit_op(uint8_t *p, uint8_t mask) {
  *p ^= mask;
}
