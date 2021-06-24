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
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

static void *list_find_aux(mu_list_t *element, void *arg);

static void *list_delete_aux(mu_list_t *element, void *arg);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_list_t *mu_list_init(mu_list_t *list) {
  list->next = NULL;
  return list;
}

bool mu_list_is_empty(mu_list_t *list) {
  return list->next == NULL;
}

size_t mu_list_length(mu_list_t *list) {
  size_t length = 0;
  while (list->next != NULL) {
    length += 1;
    list = list->next;
  }
  return length;
}

bool mu_list_contains(mu_list_t *list, mu_list_t *element) {
  return mu_list_find(list, element) != NULL;
}

mu_list_t *mu_list_first(mu_list_t *list) {
  return list->next;
}

mu_list_t *mu_list_push(mu_list_t *list, mu_list_t *element) {
  element->next = list->next;
  list->next = element;
  return list;
}

mu_list_t *mu_list_pop(mu_list_t *list) {
  mu_list_t *element = NULL;

  if (list->next != NULL) {
    element = list->next;
    list->next = element->next;
    element->next = NULL;
  }
  return element;
}

mu_list_t *mu_list_find(mu_list_t *list, mu_list_t *element) {
  return mu_list_traverse(list, list_find_aux, element);
}

mu_list_t *mu_list_delete(mu_list_t *list, mu_list_t *element) {
  return mu_list_traverse(list, list_delete_aux, element);
}

mu_list_t *mu_list_reverse(mu_list_t *list) {
  mu_list_t reversed;

  mu_list_init(&reversed);
  while (!mu_list_is_empty(list)) {
    mu_list_push(&reversed, mu_list_pop(list));
  }
  list->next = reversed.next;

  return list;
}

void *mu_list_traverse(mu_list_t *list, mu_list_traverse_fn fn, void *arg) {
  mu_list_t *prev = list;
  void *result = NULL;

  while (prev != NULL && result == NULL) {
    result = fn(prev, arg);
    prev = prev->next;
  }
  return result;
}

mu_list_t *mu_list_next_element(mu_list_t *element) {
  return element->next;
}

// =============================================================================
// local (static) code

static void *list_find_aux(mu_list_t *prev, void *arg) {
  if (prev->next == arg) {
    return arg;
  } else {
    return NULL;
  }
}

static void *list_delete_aux(mu_list_t *prev, void *arg) {
  if (prev->next == arg) {
    return mu_list_pop(prev);
  } else {
    return NULL;
  }
}
