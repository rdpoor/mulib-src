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

#include "mulib.h"
#include <stdbool.h>
#include <stddef.h>

// #include <stdio.h>  // debugging

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

// =============================================================================
// local storage

// =============================================================================
// public code

mu_queue_t *mu_queue_init(mu_queue_t *q) {
  mu_list_init(&q->takr);
  q->putr = NULL;
  return q;
}

mu_queue_t *mu_queue_add(mu_queue_t *q, mu_list_t *item) {
  item->next = NULL;
  if (q->putr == NULL) {
    // adding first item
    mu_list_push(&q->takr, item);
  } else {
    mu_list_push(q->putr, item);
  }
  q->putr = item;

  return q;
}

mu_list_t *mu_queue_remove(mu_queue_t *q) {
  mu_list_t *item = mu_list_pop(&q->takr);
  if (mu_list_is_empty(&q->takr)) {
    q->putr = NULL;       // removing last item;
  }
  return item;
}

bool mu_queue_is_empty(mu_queue_t *q) {
  return mu_list_is_empty(&q->takr);
}

void mu_queue_empty(mu_queue_t *q) {
  while (!mu_queue_is_empty(q)) {
    mu_queue_remove(q);
  }
}

bool mu_queue_contains(mu_queue_t *q, mu_list_t *item) {
  return mu_list_contains(&q->takr, item);
}

int mu_queue_length(mu_queue_t *q) {
  return mu_list_length(&q->takr);
}

// =============================================================================
// local (static) code
