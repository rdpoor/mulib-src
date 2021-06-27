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
A newly initialize element has its next and prev fields set to itself.  We can
use this to determine if an element is a member of a list:

            e
          +---+
     next | e |
          +---+
     prev | e |
          +---+

Most functions take a list head argument H that tracks the first and last
elements of the underlying list:

An empty list:
            H
          +---+
     next | H |
          +---+
     prev | H |
          +---+

One element
            H           a
          +---+       +---+
          | a |       | H |
          +---+       +---+
          | a |       | H |
          +---+       +---+

Two elements:
            H           b            a
          +---+       +---+       +---+
          | b |       | a |       | H |
          +---+       +---+       +---+
          | a |       | H |       | b |
          +---+       +---+       +---+

Three elements:
            H           c            b           a
          +---+       +---+       +---+       +---+
          | c |       | b |       | a |       | H |
          +---+       +---+       +---+       +---+
          | a |       | H |       | c |       | b |
          +---+       +---+       +---+       +---+

## An alternative

NOTE: This is not what's implemented.  I'm describing it as a possible
alternative since it might be more efficient.  This version makes a distinction
between list_head and list_item.

An empty list:
            H
          +---+
    items | ^ |
          +---+

One element
            H           a
          +---+       +---+
          | a |       | ^ |
          +---+       +---+
                      | ^ |
                      +---+

Two elements:
            H           b            a
          +---+       +---+       +---+
          | b |       | a |       | ^ |
          +---+       +---+       +---+
                      | ^ |       | b |
                      +---+       +---+

Three elements:
            H           c            b           a
          +---+       +---+       +---+       +---+
          | c |       | b |       | a |       | ^ |
          +---+       +---+       +---+       +---+
                      | ^ |       | c |       | b |
                      +---+       +---+       +---+


 */

// =============================================================================
// includes

#include "mu_dlist.h"
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

// =============================================================================
// operations on individual list elements

mu_dlist_t *mu_dlist_init(mu_dlist_t *e) {
  e->next = e->prev = e;
  return e;
}

bool mu_dlist_is_empty(mu_dlist_t *e) {
  return mu_dlist_next(e) == e;
}

mu_dlist_t *mu_dlist_next(mu_dlist_t *element) {
  return element->next;
}

mu_dlist_t *mu_dlist_prev(mu_dlist_t *element) {
  return element->prev;
}

mu_dlist_t *mu_dlist_insert_next(mu_dlist_t *list, mu_dlist_t *e) {
  e->prev = list;
  e->next = list->next;
  list->next->prev = e;
  list->next = e;
  return e;
}

mu_dlist_t *mu_dlist_insert_prev(mu_dlist_t *list, mu_dlist_t *e) {
  e->next = list;
  e->prev = list->prev;
  list->prev->next = e;
  list->prev = e;
  return e;
}

bool mu_dlist_is_linked(mu_dlist_t *e) {
  return !mu_dlist_is_empty(e);
}

mu_dlist_t *mu_dlist_unlink(mu_dlist_t *e) {
  if (mu_dlist_is_linked(e)) {
    e->prev->next = e->next;
    e->next->prev = e->prev;
    return mu_dlist_init(e);
  } else {
    return NULL;
  }
}

// =============================================================================
// operations on a list head

mu_dlist_t *mu_dlist_first(mu_dlist_t *head) {
  if (mu_dlist_is_empty(head)) {
    return NULL;
  } else {
    return mu_dlist_next(head);
  }
}

mu_dlist_t *mu_dlist_last(mu_dlist_t *head) {
  if (mu_dlist_is_empty(head)) {
    return NULL;
  } else {
    return mu_dlist_prev(head);
  }
}

size_t mu_dlist_length(mu_dlist_t *head) {
  mu_dlist_t *list = head;
  int length = 0;

  while (mu_dlist_next(list) != head) {
    length += 1;
    list = mu_dlist_next(list);
  }
  return length;
}

mu_dlist_t *mu_dlist_find(mu_dlist_t *head, mu_dlist_t *e) {
  return (mu_dlist_t *)mu_dlist_traverse(head, list_find_aux, e);
}

mu_dlist_t *mu_dlist_find_prev(mu_dlist_t *head, mu_dlist_t *e) {
  return (mu_dlist_t *)mu_dlist_traverse_prev(head, list_find_aux, e);
}

mu_dlist_t *mu_dlist_push(mu_dlist_t *head, mu_dlist_t *e) {
  mu_dlist_insert_next(head, e);
  return head;
}

mu_dlist_t *mu_dlist_push_prev(mu_dlist_t *head, mu_dlist_t *e) {
  mu_dlist_insert_prev(head, e);
  return head;
}

mu_dlist_t *mu_dlist_pop(mu_dlist_t *head) {
  if (mu_dlist_is_empty(head)) {
    return NULL;
  } else {
    return mu_dlist_unlink(mu_dlist_next(head));
  }
}

mu_dlist_t *mu_dlist_pop_prev(mu_dlist_t *head) {
  if (mu_dlist_is_empty(head)) {
    return NULL;
  } else {
    return mu_dlist_unlink(mu_dlist_prev(head));
  }
}

void *mu_dlist_traverse(mu_dlist_t *head, mu_dlist_traverse_fn fn, void *arg) {
  void *result = NULL;
  mu_dlist_t *list = mu_dlist_next(head);
  while (list != head && result == NULL) {
    result = fn(list, arg);
    list = mu_dlist_next(list);
  }
  return result;
}

void *mu_dlist_traverse_prev(mu_dlist_t *head, mu_dlist_traverse_fn fn, void *arg) {
  void *result = NULL;
  mu_dlist_t *list = mu_dlist_prev(head);
  while (list != head && result == NULL) {
    result = fn(list, arg);
    list = mu_dlist_prev(list);
  }
  return result;
}


mu_dlist_t *mu_dlist_reverse(mu_dlist_t *head) {
  if (mu_dlist_length(head) > 1) {
    mu_dlist_t reversed;
    mu_dlist_init(&reversed);

    do {
      mu_dlist_push(&reversed, mu_dlist_pop(head));
    } while (!mu_dlist_is_empty(head));
    head->next = reversed.next;
    head->prev = reversed.prev;
    head->next->prev = head;
    head->prev->next = head;
  }

  return head;
}

// =============================================================================
// local (static) code

static void *list_find_aux(mu_dlist_t *element, void *arg) {
  return (element == arg) ? arg : NULL;
}
