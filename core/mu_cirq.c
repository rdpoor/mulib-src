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

/**
 * @file Single-producer, single-consumer, lock-free circular buffer.
 */

// =============================================================================
// includes

#include "mu_cirq.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

/**
 * Return the highest power of two that is less than or equal to n.
 */
static int hpo2leqn(int n);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_cirq_t *mu_cirq_init(mu_cirq_t *cirq, void *store, uint8_t n_elements) {
  cirq->store = store;
  cirq->mask = hpo2leqn(n_elements) - 1;
  return mu_cirq_reset(cirq);
}

mu_cirq_t *mu_cirq_reset(mu_cirq_t *cirq) {
  cirq->putr = 0;
  cirq->takr = 0;
  return cirq;
}

uint8_t mu_cirq_capacity(mu_cirq_t *cirq) {
  return cirq->mask;
}

bool mu_cirq_is_empty(mu_cirq_t *cirq) {
  return cirq->putr == cirq->takr;
}

bool mu_cirq_is_full(mu_cirq_t *cirq) {
  return ((cirq->putr + 1) & cirq->mask) == cirq->takr;
}

uint8_t mu_cirq_write_8(mu_cirq_t *cirq, const uint8_t *src, size_t count) {
  uint8_t written = 0;
  uint8_t *dst = (uint8_t *)cirq->store;
  while (count-- > 0) {
    // reserve space by incrementing putr before writing the item
    uint8_t i1 = cirq->putr;              // where the item will be written
    uint8_t i2 = (i1 + 1) & cirq->mask;   // post-write value of cirq->putr
    if (i2 == cirq->takr) break;          // queue was full -- don't write
    cirq->putr = i2;                      // increment putr
    dst[i1] = *src++;                     // copy element
    written += 1;
  }
  return written;
}

uint8_t mu_cirq_read_8(mu_cirq_t *cirq, uint8_t *dst, size_t count) {
  uint8_t read = 0;
  const uint8_t *src = (const uint8_t *)cirq->store;
  while (count-- > 0 && !mu_cirq_is_empty(cirq)) {
    // read the item before incrementing takr so it is always protected
    *dst++ = src[cirq->takr++];
    cirq->takr &= cirq->mask;
    read += 1;
  }
  return read;
}

uint8_t mu_cirq_write_16(mu_cirq_t *cirq, const uint16_t *src, size_t count) {
  uint8_t written = 0;
  uint16_t *dst = (uint16_t *)cirq->store;
  while (count-- > 0) {
    // reserve space by incrementing putr before writing the item
    uint8_t i1 = cirq->putr;              // where the item will be written
    uint8_t i2 = (i1 + 1) & cirq->mask;   // post-write value of cirq->putr
    if (i2 == cirq->takr) break;          // queue was full -- don't write
    cirq->putr = i2;                      // increment putr
    dst[i1] = *src++;                     // copy element
    written += 1;
  }
  return written;
}

uint8_t mu_cirq_read_16(mu_cirq_t *cirq, uint16_t *dst, size_t count) {
  uint8_t read = 0;
  const uint16_t *src = (const uint16_t *)cirq->store;
  while (count-- > 0 && !mu_cirq_is_empty(cirq)) {
    // read the item before incrementing takr so it is always protected
    *dst++ = src[cirq->takr++];
    cirq->takr &= cirq->mask;
    read += 1;
  }
  return read;
}

uint8_t mu_cirq_write_32(mu_cirq_t *cirq, const uint32_t *src, size_t count) {
  uint8_t written = 0;
  uint32_t *dst = (uint32_t *)cirq->store;
  while (count-- > 0) {
    // reserve space by incrementing putr before writing the item
    uint8_t i1 = cirq->putr;              // where the item will be written
    uint8_t i2 = (i1 + 1) & cirq->mask;   // post-write value of cirq->putr
    if (i2 == cirq->takr) break;          // queue was full -- don't write
    cirq->putr = i2;                      // increment putr
    dst[i1] = *src++;                     // copy element
    written += 1;
  }
  return written;
}

uint8_t mu_cirq_read_32(mu_cirq_t *cirq, uint32_t *dst, size_t count) {
  uint8_t read = 0;
  const uint32_t *src = (const uint32_t *)cirq->store;
  while (count-- > 0 && !mu_cirq_is_empty(cirq)) {
    // read the item before incrementing takr so it is always protected
    *dst++ = src[cirq->takr++];
    cirq->takr &= cirq->mask;
    read += 1;
  }
  return read;
}

uint8_t mu_cirq_write_n(mu_cirq_t *cirq, const void *src, size_t count, uint8_t element_size) {
  const uint8_t *s2 = (const uint8_t *)src;
  uint8_t *dst = (uint8_t *)cirq->store;

  uint8_t written = 0;
  while (count-- > 0) {
    // reserve space by incrementing putr before writing the item
    uint8_t i1 = cirq->putr;              // where the item will be written
    uint8_t i2 = (i1 + 1) & cirq->mask;   // post-write value of cirq->putr
    if (i2 == cirq->takr) break;          // queue was full -- don't write
    cirq->putr = i2;                      // increment putr
    memcpy(&dst[i1 * element_size], &s2[written * element_size], element_size);
    written += 1;
  }
  return written;
}

uint8_t mu_cirq_read_n(mu_cirq_t *cirq, void *dst, size_t count, uint8_t element_size) {
  uint8_t *d2 = (uint8_t *)dst;
  const uint8_t *src = (const uint8_t *)cirq->store;

  uint8_t read = 0;
  while (count-- > 0 && !mu_cirq_is_empty(cirq)) {
    // read the item before incrementing takr so it is always protected
    memcpy(&d2[read * element_size], &src[cirq->takr++ * element_size], element_size);
    cirq->takr &= cirq->mask;
    read += 1;
  }
  return read;
}

// =============================================================================
// local code

static int hpo2leqn(int n) {
  int p = 1;
  while (n > 0) {
    n >>= 1;
    p <<= 1;
  }
  return p >> 1;
}
