/**
 * MIT License
 *
 * Copyright (c) 2020 R. D. Poor <rdpoor@gmail.com>
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
// Includes

#include "mu_array.h"
#include <stdint.h>

// =============================================================================
// Local types and definitions

// =============================================================================
// Local storage

// =============================================================================
// Local (forward) declarations

static void enheap(void *elements,
                   size_t element_count,
                   mu_array_cmp_fn comparison_fn,
                   size_t element_size);
static void ensift(void *elements,
                   mu_array_cmp_fn comparison_fn,
                   int start,
                   int end,
                   size_t element_size);
static void *aref(void *elements, int index, size_t element_size);
static void swap(void *elements, int i1, int i2, size_t element_size);

// =============================================================================
// Public code

void mu_array_sort(void *elements,
                   size_t element_count,
                   mu_array_cmp_fn comparison_fn,
                   size_t element_size) {
  enheap(elements, element_count, comparison_fn, element_size);
  size_t end = element_count - 1;
  while (end > 0) {
    swap(elements, end, 0, element_size);
    end -= 1;
    ensift(elements, comparison_fn, 0, end, element_size);
  }
}

// =============================================================================
// Local (static) code

static void enheap(void *elements,
                   size_t element_count,
                   mu_array_cmp_fn comparison_fn,
                   size_t element_size) {
  int start = (element_count - 2) / 2; // index of last parent node

  while (start >= 0) {
    ensift(elements, comparison_fn, start, element_count-1, element_size);
    start -= 1;
  }
}

static void ensift(void *elements,
                   mu_array_cmp_fn comparison_fn,
                   int start,
                   int end,
                   size_t element_size) {
  int root = start;
  while (root * 2 + 1 <= end) {
    // root has at least one child...
    int child = root * 2 + 1;      // left child
    if ((child + 1 <= end) &&
        comparison_fn(aref(elements, child, element_size),
                      aref(elements, child+1, element_size)) < 0) {
      // child has a sibling and its value is less than the sibling's...
      child += 1; // then act on right child instead
    }
    if (comparison_fn(aref(elements, root, element_size),
                      aref(elements, child, element_size)) < 0) {
      // not in heap order...
      swap(elements, root, child, element_size);
      root = child; // continue sifting down the child
    } else {
      return;
    }
  }
}

static void *aref(void *elements, int index, size_t element_size) {
  uint8_t *p = (uint8_t *)elements;
  return (void *)(&p[index * element_size]);
}

static void swap(void *elements, int i1, int i2, size_t element_size) {
  uint8_t temp;
  uint8_t *p1 = aref(elements, i1, element_size);
  uint8_t *p2 = aref(elements, i2, element_size);

  for (size_t i=0; i<element_size; i++) {
    temp = p1[i];
    p1[i] = p2[i];
    p2[i] = temp;
  }
}
