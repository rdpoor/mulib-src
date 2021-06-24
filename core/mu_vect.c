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

#include "mu_vect.h"
#include <stddef.h>
#include <string.h>

// =============================================================================
// private types and definitions

// =============================================================================
// private declarations

/**
 * @brief mu_vect_ref() without error checking.
 */
static void *ref(mu_vect_t *vect, size_t index);

/**
 * @brief Find the "leftmost" insertion point.
 *
 * See http://rosettacode.org/wiki/Binary_search
 */
static size_t find_insertion_index(mu_vect_t *vect, void *e, mu_vect_cmp_fn cmp);

static void heapify(mu_vect_t *vect, mu_vect_cmp_fn cmp);

static void sift_down(mu_vect_t *vect, mu_vect_cmp_fn cmp, int start, int end);

static void swap(mu_vect_t *vect, int a, int b);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_vect_t *mu_vect_init(mu_vect_t *vect, void *elements, size_t capacity, size_t element_size) {
  vect->elements = elements;
  vect->capacity = capacity;
  vect->element_size = element_size;
  return mu_vect_reset(vect);
}

mu_vect_t *mu_vect_reset(mu_vect_t *vect) {
  memset(vect->elements, 0, vect->capacity * vect->element_size);
  vect->count = 0;
  return vect;
}

void *mu_vect_elements(mu_vect_t *vect) {
  return vect->elements;
}

size_t mu_vect_capacity(mu_vect_t *vect) {
  return vect->capacity;
}

size_t mu_vect_count(mu_vect_t *vect) {
  return vect->count;
}

size_t mu_vect_element_size(mu_vect_t *vect) {
  return vect->element_size;
}

bool mu_vect_is_empty(mu_vect_t *vect) {
  return vect->count == 0;
}

bool mu_vect_is_full(mu_vect_t *vect) {
  return vect->count == vect->capacity;
}

void *mu_vect_ref(mu_vect_t *vect, size_t index) {
  if (index >= mu_vect_count(vect)) {
    return NULL;
  }
  return ref(vect, index);
}

mu_vect_err_t mu_vect_peek(mu_vect_t *vect, void *e) {
  void *src = mu_vect_ref(vect, mu_vect_count(vect) - 1);
  if (!src) {
    return MU_VECT_ERR_EMPTY;
  }
  if (e) {
    memcpy(e, src, mu_vect_element_size(vect));
  }
  return MU_VECT_ERR_NONE;
}

mu_vect_err_t mu_vect_insert_at(mu_vect_t *vect, size_t index, void *e) {
  if (mu_vect_is_full(vect)) {
    return MU_VECT_ERR_FULL;
  }
  int to_move = mu_vect_count(vect) - index;
  if (to_move < 0) {
    return MU_VECT_ERR_INDEX;
  }
  void *at = ref(vect, index);
  // carve a hole if needed
  if (to_move > 0) {
    void *dst = ref(vect, index + 1);  // TODO: can optimize
    memmove(dst, at, mu_vect_element_size(vect) * to_move);
  }
  // copy e into elements
  memcpy(at, e, mu_vect_element_size(vect));

  vect->count += 1;
  return MU_VECT_ERR_NONE;
}

mu_vect_err_t mu_vect_push(mu_vect_t *vect, void *e) {
  return mu_vect_insert_at(vect, mu_vect_count(vect), e);
}

mu_vect_err_t mu_vect_delete_at(mu_vect_t *vect, size_t index, void *e) {
  if (mu_vect_is_empty(vect)) {
    return MU_VECT_ERR_EMPTY;
  }
  void *at = mu_vect_ref(vect, index);
  if (at == NULL) {
    return MU_VECT_ERR_INDEX;
  }
  if (e != NULL) {
    memcpy(e, at, mu_vect_element_size(vect));
  }
  // close hole as needed
  int to_move = mu_vect_count(vect) - index - 1;
  if (to_move > 0) {
    void *src = mu_vect_ref(vect, index + 1);
    memmove(at, src, mu_vect_element_size(vect) * to_move);
  }
  vect->count -= 1;
  return MU_VECT_ERR_NONE;
}

mu_vect_err_t mu_vect_pop(mu_vect_t *vect, void *e) {
  return mu_vect_delete_at(vect, mu_vect_count(vect) - 1, e);
}

mu_vect_err_t mu_vect_insert_sorted(mu_vect_t *vect, void *e, mu_vect_cmp_fn cmp) {
  if (mu_vect_is_full(vect)) {
    return MU_VECT_ERR_FULL;
  }
  int index = find_insertion_index(vect, e, cmp);
  return mu_vect_insert_at(vect, index, e);
}

mu_vect_err_t mu_vect_sort(mu_vect_t *vect, mu_vect_cmp_fn cmp) {
  heapify(vect, cmp);
  size_t end = mu_vect_count(vect) - 1;
  while (end > 0) {
    swap(vect, end, 0);
    end -= 1;
    sift_down(vect, cmp, 0, end);
  }
  return MU_VECT_ERR_NONE;
}

void *mu_vect_traverse(mu_vect_t *vect, mu_vect_find_fn find_fn, void *arg) {
  for (size_t i = 0; i<mu_vect_count(vect); i++) {
    void *result = find_fn(ref(vect, i), arg);
    if (result != NULL) return result;
  }
  return NULL;
}

int mu_vect_find_index(mu_vect_t *vect, mu_vect_find_fn find_fn, void *arg) {
  for (size_t i = 0; i<mu_vect_count(vect); i++) {
    void *result = find_fn(ref(vect, i), arg);
    if (result != NULL) return i;
  }
  return -1;
}

bool mu_vect_contains(mu_vect_t *vect, mu_vect_find_fn find_fn, void *arg) {
  return mu_vect_find_index(vect, find_fn, arg) != -1;
}

// =============================================================================
// local (static) code

static void *ref(mu_vect_t *vect, size_t index) {
  char *elements = (char *)vect->elements;
  return (void *)(&elements[index * vect->element_size]);
}

// Find "leftmost" insertion point, as described in
// http://rosettacode.org/wiki/Binary_search

static size_t find_insertion_index(mu_vect_t *vect, void *e, mu_vect_cmp_fn cmp) {
  int low = 0;
  int high = mu_vect_count(vect) - 1;
  while (low <= high) {
    int mid = (low + high) / 2;
    if (cmp(ref(vect, mid), e) > 0) {
      high = mid - 1;
    } else {
      low = mid + 1;
    }
  }
  return low;
}

static void heapify(mu_vect_t *vect, mu_vect_cmp_fn cmp) {
  int count = mu_vect_count(vect);
  int start = (count - 2) / 2; // index of last parent node

  while (start >= 0) {
    sift_down(vect, cmp, start, count - 1);
    start -= 1;
  }
}

static void sift_down(mu_vect_t *vect, mu_vect_cmp_fn cmp, int start, int end) {
  int root = start;
  while (root * 2 + 1 <= end) {
    // root has at least one child...
    int child = root * 2 + 1;      // left child
    if ((child + 1 <= end) && cmp(ref(vect, child), ref(vect, child+1)) < 0) {
      // child has a sibling and its value is less than the sibling's...
      child += 1; // then act on right child instead
    }
    if (cmp(ref(vect, root), ref(vect, child)) < 0) {
      // not in heap order...
      swap(vect, root, child);
      root = child; // continue sifting down the child
    } else {
      return;
    }
  }

}

static void swap(mu_vect_t *vect, int a, int b) {
  char temp;
  char *pa = (char *)ref(vect, a);
  char *pb = (char *)ref(vect, b);

  for (size_t i=0; i<mu_vect_element_size(vect); i++) {
    temp = pa[i];
    pa[i] = pb[i];
    pb[i] = temp;
  }
}
