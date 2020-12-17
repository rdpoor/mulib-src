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

#include "mu_list.h"
#include "mu_types.h"
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

static void *list_length_aux(mu_list_t *ref, void *arg);

static void *list_find_aux(mu_list_t *ref, void *arg);

static void *list_delete_aux(mu_list_t *ref, void *arg);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_list_t *mu_list_rest(mu_list_t *ref) {
  if (!ref) {
    return MU_LIST_END;
  } else {
    return ref->next;
  }
}

mu_list_t *mu_list_set_rest(mu_list_t *ref, mu_list_t *rest) {
  ref->next = rest;
  return ref;
}

bool mu_list_is_empty(mu_list_t *ref) {
  return (ref == NULL) || (ref->next == NULL);
}

mu_list_t *mu_list_push(mu_list_t *ref, mu_list_t *item) {
  // TODO check with godbolt: does returning a value increase code?
  if (ref != NULL) {
    item->next = ref->next;
    ref->next = item;
  }
  return ref;
}

mu_list_t *mu_list_pop(mu_list_t *ref) {
  mu_list_t *item = NULL;
  if (ref != NULL) {
    item = ref->next;
    if (item != NULL) {
      ref->next = item->next;
      item->next = NULL;
    }
  }
  return item;
}

void *mu_list_traverse(mu_list_t *ref, mu_list_traverse_fn fn, void *arg) {
  void *result = NULL;
  while (ref && result == NULL) {
    result = fn(ref, arg);
    ref = ref->next;
  }
  return result;
}

int mu_list_length(mu_list_t *ref) {
  int count = 0;
  mu_list_traverse(ref, list_length_aux, (void *)&count);
  return count;
}

mu_list_t *mu_list_find(mu_list_t *ref, mu_list_t *item) {
  return mu_list_traverse(ref, list_find_aux, item);
}

bool mu_list_contains(mu_list_t *ref, mu_list_t *item) {
  return mu_list_find(ref, item) != MU_LIST_END;
}

mu_list_t *mu_list_delete(mu_list_t *ref, mu_list_t *item) {
  return mu_list_traverse(ref, list_delete_aux, item);
}

mu_list_t *mu_list_reverse(mu_list_t *ref) {
  if (ref != NULL) {
    mu_list_t reversed = {.next = NULL};
    while (ref->next) {
      mu_list_push(&reversed, mu_list_pop(ref));
    }
    ref->next = reversed.next;
  }
  return ref;
}

// =============================================================================
// local (static) code

static void *list_length_aux(mu_list_t *ref, void *arg) {
  int *count = (int *)arg;
  if (ref->next != NULL) {
    *count += 1;
  }
  return NULL;
}

static void *list_find_aux(mu_list_t *ref, void *arg) {
  return (ref->next == arg) ? arg : NULL;
}

static void *list_delete_aux(mu_list_t *ref, void *arg) {
  return (ref->next == arg) ? mu_list_pop(ref) : NULL;
}
