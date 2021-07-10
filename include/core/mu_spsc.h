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
 * @brief Implementation of a lock-free Single Producer / Single Consumer queue.
 *
 * spsc stores pointer sized objects in a queue.  In mulib, the scheduler uses
 * an instance of spsc to mediate between interrupt and foreground levels.
 */

#ifndef _MU_SPSC_H_
#define _MU_SPSC_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// types and definitions

typedef enum {
  MU_SPSC_ERR_NONE,
  MU_SPSC_ERR_EMPTY,
  MU_SPSC_ERR_FULL,
  MU_SPSC_ERR_SIZE,
} mu_spsc_err_t;

// mu_spsc manages pointer-sized objects
typedef void * mu_spsc_item_t;

typedef struct {
  uint16_t mask;
  volatile uint16_t head;
  volatile uint16_t tail;
  mu_spsc_item_t *store;
} mu_spsc_t;

// =============================================================================
// declarations

/**
 * @brief initialize a cqueue with a backing store.  capacity must be a power
 * of two.
 */
mu_spsc_err_t mu_spsc_init(mu_spsc_t *q, mu_spsc_item_t *store, uint16_t capacity);

/**
 * @brief reset the cqueue to empty.  Not interrupt safe!
 */
mu_spsc_err_t mu_spsc_reset(mu_spsc_t *q);

/**
 * @brief return the maximum number of items that can be stored in the cqueue.
 * May be called at any time.
 */
uint16_t mu_spsc_capacity(mu_spsc_t *q);

/**
 * @brief Insert an item at the tail of the queue.  May only be called by the
 * producer (interrupt level).
 */
mu_spsc_err_t mu_spsc_put(mu_spsc_t *q, mu_spsc_item_t item);

/**
 * @brief Remove an item from the head of the queue.  May only be called by the
 * consumer (foreground level).
 */
mu_spsc_err_t mu_spsc_get(mu_spsc_t *q, mu_spsc_item_t *item);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _MU_SPSC_H_
