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
Implementation Notes:

Each element of a dlist has a next and a prev field.

A dlist *always* implements a circular list:
- A "list head" argument is a sentinal (in the CRL terminology)
- An empty list is represented by the list head with next and prev pointers
  pointing to itself.
- If an element is part of a list, both next and prev fields are non-null
- If an element is not part of a dlist, both next and prev fields are null.

However, depending on your application, you can pass any element of a dlist
as the head.

*/


// =============================================================================
// includes

#include "mulib.h"
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

static void *list_find_aux(mu_dlist_t *element, void *arg);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_dlist_t *mu_dlist_init(mu_dlist_t *list) {
  list->next = list;
  list->prev = list;
  return list;
}

bool mu_dlist_is_empty(mu_dlist_t *list) {
  return list->next == list;
}

size_t mu_dlist_length(mu_dlist_t *list) {
  size_t length = 0;
  mu_dlist_t *e = list;
  while (e->next != list) {
    length += 1;
    e = e->next;
  }
  return length;
}

bool mu_dlist_contains(mu_dlist_t *list, mu_dlist_t *element) {
  return mu_dlist_find(list, element) != NULL;
}

mu_dlist_t *mu_dlist_first(mu_dlist_t *list) {
  mu_dlist_t *first = list->next;
  if (first == list) {
    return NULL;
  } else {
    return first;
  }
}

mu_dlist_t *mu_dlist_last(mu_dlist_t *list) {
  mu_dlist_t *last = list->prev;
  if (last == list) {
    return NULL;
  } else {
    return last;
  }
}

mu_dlist_t *mu_dlist_push(mu_dlist_t *list, mu_dlist_t *element) {
  element->next = list->next;
  list->next->prev = element;
  list->next = element;
  element->prev = list;
  if (list->prev == list) {
    list->prev = element;
  }
  return list;
}

mu_dlist_t *mu_dlist_lpush(mu_dlist_t *list, mu_dlist_t *element) {
  element->prev = list->prev;
  list->prev->next = element;
  list->prev = element;
  element->next = list;
  if (list->next == list) {
    list->next = element;
  }
  return list;
}

mu_dlist_t *mu_dlist_pop(mu_dlist_t *list) {
  mu_dlist_t *element = NULL;

  if (list->next != list) {
    element = list->next;
    element->next->prev = element->prev;
    list->next = element->next;
    element->next = NULL;
    element->prev = NULL;
    if (list->prev == element) {
      list->prev = list;
    }
  }
  return element;
}

mu_dlist_t *mu_dlist_lpop(mu_dlist_t *list) {
  mu_dlist_t *element = NULL;

  if (list->prev != list) {
    element = list->prev;
    element->prev->next = element->next;
    list->prev = element->prev;
    element->next = NULL;
    element->prev = NULL;
    if (list->next == element) {
      list->next = list;
    }
  }
  return element;
}

mu_dlist_t *mu_dlist_find(mu_dlist_t *list, mu_dlist_t *element) {
  return mu_dlist_traverse(list, list_find_aux, element);
}

mu_dlist_t *mu_dlist_lfind(mu_dlist_t *list, mu_dlist_t *element) {
  return mu_dlist_ltraverse(list, list_find_aux, element);
}

mu_dlist_t *mu_dlist_reverse(mu_dlist_t *list) {
  mu_dlist_t reversed;

  mu_dlist_init(&reversed);
  while (!mu_dlist_is_empty(list)) {
    mu_dlist_push(&reversed, mu_dlist_pop(list));
  }
  if (!mu_dlist_is_empty(&reversed)) {
    list->next = reversed.next;
    list->prev = reversed.prev;
    list->next->prev = list;
    list->prev->next = list;
  }

  return list;
}

void *mu_dlist_traverse(mu_dlist_t *list, mu_dlist_traverse_fn fn, void *arg) {
  mu_dlist_t *element = list->next;
  void *result = NULL;

  while (element != list && result == NULL) {
    result = fn(element, arg);
    element = element->next;
  }
  return result;
}

void *mu_dlist_ltraverse(mu_dlist_t *list, mu_dlist_traverse_fn fn, void *arg) {
  mu_dlist_t *element = list->prev;
  void *result = NULL;

  while (element != list && result == NULL) {
    result = fn(element, arg);
    element = element->prev;
  }
  return result;
}

mu_dlist_t *mu_dlist_next_element(mu_dlist_t *element) {
  return element->next;
}

mu_dlist_t *mu_dlist_prev_element(mu_dlist_t *element) {
  return element->prev;
}

bool mu_dlist_element_is_linked(mu_dlist_t *element) {
  return element->next != NULL;
}

mu_dlist_t *mu_dlist_unlink_element(mu_dlist_t *element) {
  // Assumes element is in list!
  element->next->prev = element->prev;
  element->prev->next = element->next;
  element->next = NULL;
  element->prev = NULL;
  return element;
}

// =============================================================================
// local (static) code

static void *list_find_aux(mu_dlist_t *element, void *arg) {
  return (element == arg) ? arg : NULL;
}
