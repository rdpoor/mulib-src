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

#include "mu_spsc.h"
#include <stddef.h>
#include <stdint.h>

// =============================================================================
// private types and definitions

// =============================================================================
// private declarations

#define IS_POWER_OF_TWO(n) (((n) & ((n)-1)) == 0)

// =============================================================================
// local storage

// =============================================================================
// public code

mu_spsc_err_t mu_spsc_init(mu_spsc_t *q, mu_spsc_item_t *store,
                           uint16_t capacity) {
  if ((capacity == 0) || !IS_POWER_OF_TWO(capacity)) {
    return MU_SPSC_ERR_SIZE;
  }
  q->mask = capacity - 1;
  q->store = store;
  return mu_spsc_reset(q);
}

mu_spsc_err_t mu_spsc_reset(mu_spsc_t *q) {
  q->head = 0;
  q->tail = 0;
  return MU_SPSC_ERR_NONE;
}

/**
 * @brief To be called by Producer only: update tail only after setting item.
 */
mu_spsc_err_t mu_spsc_put(mu_spsc_t *q, mu_spsc_item_t item) {
  mu_spsc_err_t err = MU_SPSC_ERR_NONE;
  uint16_t next_tail = (q->tail + 1) & q->mask;

  if (next_tail == q->head) {
    err = MU_SPSC_ERR_FULL;
  } else {
    q->store[q->tail] = item;
    q->tail = next_tail;
  }

  return err;
}

/**
 * @brief To be called by Consumer only: update head only after fetching item.
 */
mu_spsc_err_t mu_spsc_get(mu_spsc_t *q, mu_spsc_item_t *item) {
  mu_spsc_err_t err = MU_SPSC_ERR_NONE;

  if (q->head == q->tail) {
    err = MU_SPSC_ERR_EMPTY;
  } else {
    *item = q->store[q->head];
    q->head = (q->head + 1) & q->mask;
  }

  return err;
}

// =============================================================================
// private code
