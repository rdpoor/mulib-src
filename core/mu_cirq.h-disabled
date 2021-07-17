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

#ifndef _MU_CIRQ_H_
#define _MU_CIRQ_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// types and definitions

typedef struct {
  void *store;         // item store, 2^n elements long (up to 256 max)
  uint8_t mask;         // 2^n - 1, and also capacity of circular buffer
  uint8_t putr;        // index where the next item will be stored
  uint8_t takr;        // index of the next item to be fetched
} mu_cirq_t;

// =============================================================================
// declarations

/**
 * @brief Initialize the Circular Queue.
 *
 * Note that the queue will hold a maximum of 255 elements, or one less than the
 * highest power of two less than or equal to n_elements, whichever is smaller.
 * The n_elements argument is rounded down to the nearest power of two that is
 * less than or equal to n_elements, and the store must be capable of holding
 * that many elements.  The actual capacity of the queue is one less than the
 * (adjusted) n_elements.
 *
 * @param cirq Pointer to the circular Queue structure to be initialized.
 * @param store Pointer to the data store.
 * @param n_elements Maximum number of elements to be held in the queue.
 */
mu_cirq_t *mu_cirq_init(mu_cirq_t *cirq, void *store, uint8_t n_elements);

mu_cirq_t *mu_cirq_reset(mu_cirq_t *cirq);

uint8_t mu_cirq_capacity(mu_cirq_t *cirq);

bool mu_cirq_is_empty(mu_cirq_t *cirq);

bool mu_cirq_is_full(mu_cirq_t *cirq);

uint8_t mu_cirq_write_8(mu_cirq_t *cirq, const uint8_t *src, size_t count);

uint8_t mu_cirq_read_8(mu_cirq_t *cirq, uint8_t *dst, size_t count);

uint8_t mu_cirq_write_16(mu_cirq_t *cirq, const uint16_t *src, size_t count);

uint8_t mu_cirq_read_16(mu_cirq_t *cirq, uint16_t *dst, size_t count);

uint8_t mu_cirq_write_32(mu_cirq_t *cirq, const uint32_t *src, size_t count);

uint8_t mu_cirq_read_32(mu_cirq_t *cirq, uint32_t *dst, size_t count);

uint8_t mu_cirq_write_n(mu_cirq_t *cirq, const void *src, size_t count, uint8_t element_size);

uint8_t mu_cirq_read_n(mu_cirq_t *cirq, void *dst, size_t count, uint8_t element_size);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_CIRQ_H_ */
