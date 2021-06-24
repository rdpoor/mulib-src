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

#include "mu_pstore.h"
#include <stddef.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

static void delete_at(mu_pstore_t *pstore, size_t index);
static size_t find_insertion_index(mu_pstore_t *pstore, mu_pstore_item_t item, mu_pstore_compare_fn cmp);
static void heapsort(mu_pstore_t *pstore, mu_pstore_compare_fn cmp);
static void heapify(mu_pstore_item_t *items, size_t count, mu_pstore_compare_fn cmp);
static void sift_down(mu_pstore_item_t *items, mu_pstore_compare_fn cmp, int start, int end);
static void swap(mu_pstore_item_t *items, int a, int b);

// =============================================================================
// local storage


// =============================================================================
// public code

/**
 * \brief initialize the pstore module.
 */
mu_pstore_t *mu_pstore_init(mu_pstore_t *pstore, mu_pstore_item_t *items, size_t capacity) {
  pstore->items = items;
  pstore->capacity = capacity;
  return mu_pstore_reset(pstore);
}

/**
 * \brief  Reset the pstore.
 */
mu_pstore_t *mu_pstore_reset(mu_pstore_t *pstore) {
  pstore->count = 0;
  return pstore;
}

mu_pstore_item_t *mu_pstore_items(mu_pstore_t *pstore) {
  return pstore->items;
}

size_t mu_pstore_capacity(mu_pstore_t *pstore) {
  return pstore->capacity;
}

size_t mu_pstore_count(mu_pstore_t *pstore) {
  return pstore->count;
}

bool mu_pstore_is_empty(mu_pstore_t *pstore) {
  return pstore->count == 0;
}

bool mu_pstore_is_full(mu_pstore_t *pstore) {
  return pstore->count == pstore->capacity;
}

bool mu_pstore_contains(mu_pstore_t *pstore, mu_pstore_item_t item) {
  return mu_pstore_index_of(pstore, item) != -1;
}

int mu_pstore_index_of(mu_pstore_t *pstore, mu_pstore_item_t item) {
  for (size_t i=0; i<pstore->count; i++) {
    if (pstore->items[i] == item) {
      return i;
    }
  }
  return -1;
}

mu_pstore_err_t mu_pstore_push(mu_pstore_t *pstore, mu_pstore_item_t item) {
  if (pstore->count == pstore->capacity) {
    return MU_PSTORE_ERR_FULL;
  }
  pstore->items[pstore->count++] = item;
  return MU_PSTORE_ERR_NONE;
}

mu_pstore_err_t mu_pstore_pop(mu_pstore_t *pstore, mu_pstore_item_t *item) {
  if (pstore->count == 0) {
    *item = NULL;
    return MU_PSTORE_ERR_EMPTY;
  }
  *item = pstore->items[--pstore->count];
  return MU_PSTORE_ERR_NONE;
}

mu_pstore_err_t mu_pstore_peek(mu_pstore_t *pstore, mu_pstore_item_t *item) {
  if (pstore->count == 0) {
    *item = NULL;
    return MU_PSTORE_ERR_EMPTY;
  }
  *item = pstore->items[pstore->count-1];
  return MU_PSTORE_ERR_NONE;
}

mu_pstore_err_t mu_pstore_insert_at(mu_pstore_t *pstore, mu_pstore_item_t item, size_t index) {
  int to_move = pstore->count - index;

  if (pstore->count == pstore->capacity) {
    return MU_PSTORE_ERR_FULL;
  } else if (to_move < 0) {
    return MU_PSTORE_ERR_INDEX;
  } else if (to_move == 0) {
    return mu_pstore_push(pstore, item);
  }

  // move existing items to make room for the new one
  mu_pstore_item_t *src = &pstore->items[index];
  mu_pstore_item_t *dst = &pstore->items[index+1];
  memmove(dst, src, sizeof(mu_pstore_item_t) * to_move);

  // insert the new item
  pstore->items[index] = item;
  pstore->count += 1;
  return MU_PSTORE_ERR_NONE;
}

mu_pstore_err_t mu_pstore_delete_at(mu_pstore_t *pstore, mu_pstore_item_t *item, size_t index) {
  int to_move = pstore->count - index - 1;

  if (pstore->count == 0) {
    return MU_PSTORE_ERR_EMPTY;
  } else if (to_move < 0) {
    *item = NULL;
    return MU_PSTORE_ERR_INDEX;
  } else if (to_move == 0) {
    return mu_pstore_pop(pstore, item);
  } else {
    *item = pstore->items[index];
    delete_at(pstore, index);
    return MU_PSTORE_ERR_NONE;
  }
}

mu_pstore_item_t mu_pstore_delete(mu_pstore_t *pstore, mu_pstore_item_t item) {
  int index = mu_pstore_index_of(pstore, item);
  if (index == -1) {
    return NULL;
  } else {
    delete_at(pstore, (size_t)index);
    return item;
  }
}

mu_pstore_err_t mu_pstore_filter(mu_pstore_t *pstore, mu_pstore_filter_fn match) {
  (void)(pstore);
  (void)(match);
  // TODO: NOT YET IMPLEMENTED
  return MU_PSTORE_ERR_NONE;
}

mu_pstore_err_t mu_pstore_insert_sorted(mu_pstore_t *pstore, mu_pstore_item_t item, mu_pstore_compare_fn cmp) {
  if (mu_pstore_count(pstore) == 0) {
    return mu_pstore_push(pstore, item);
  } else {
    int index = find_insertion_index(pstore, item, cmp);
    return mu_pstore_insert_at(pstore, item, index);
  }
}

mu_pstore_err_t mu_pstore_sort(mu_pstore_t *pstore, mu_pstore_compare_fn cmp) {
  if (mu_pstore_count(pstore) > 1) {
    heapsort(pstore, cmp);
  }
  return MU_PSTORE_ERR_NONE;
}

// =============================================================================
// local (static) code

// safe version of mu_pstore_delete_at
static void delete_at(mu_pstore_t *pstore, size_t index) {
  int to_move = pstore->count - index - 1;

  // move existing items down
  mu_pstore_item_t *dst = &pstore->items[index];
  mu_pstore_item_t *src = &pstore->items[index+1];
  memmove(dst, src, sizeof(mu_pstore_item_t) * to_move);
  pstore->count -= 1;
}

// Find "leftmost" insertion point, as described in
// http://rosettacode.org/wiki/Binary_search

static size_t find_insertion_index(mu_pstore_t *pstore, mu_pstore_item_t item, mu_pstore_compare_fn cmp) {
  mu_pstore_item_t *items = mu_pstore_items(pstore);
  int low = 0;
  int high = mu_pstore_count(pstore) - 1;
  while (low <= high) {
    int mid = (low + high) / 2;
    if (cmp(items[mid], item) > 0) {
      high = mid - 1;
    } else {
      low = mid + 1;
    }
  }
  return low;
}

static void heapsort(mu_pstore_t *pstore, mu_pstore_compare_fn cmp) {
  mu_pstore_item_t *items = mu_pstore_items(pstore);
  size_t count = mu_pstore_count(pstore);

  heapify(items, count, cmp);

  size_t end = count - 1;
  while (end > 0) {
    swap(items, end, 0);
    end -= 1;
    sift_down(items, cmp, 0, end);
  }
}

static void heapify(mu_pstore_item_t *items, size_t count, mu_pstore_compare_fn cmp) {
  int start = (count - 2) / 2; // index of last parent node

  while (start >= 0) {
    sift_down(items, cmp, start, count - 1);
    start -= 1;
  }
}

static void sift_down(mu_pstore_item_t *items, mu_pstore_compare_fn cmp, int start, int end) {
  int root = start;
  while (root * 2 + 1 <= end) {
    // root has at least one child...
    int child = root * 2 + 1;      // left child
    if ((child + 1 <= end) &&
        cmp(items[child], items[child + 1]) < 0) {
      // child has a sibling and its value is less than the sibling's...
      child += 1; // then act on right child instead
    }
    if (cmp(items[root], items[child]) < 0) {
      // not in heap order...
      swap(items, root, child);
      root = child; // continue sifting down the child
    } else {
      return;
    }
  }
}

static void swap(mu_pstore_item_t *items, int a, int b) {
  mu_pstore_item_t temp = items[a];
  items[a] = items[b];
  items[b] = temp;
}
