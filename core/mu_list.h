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
 * @file Singly-linked list support.
 *
 * # mu_list functions take a reference to the head of the list.
 *
 * Note that almost all functions take a reference (i.e. a pointer) to the head
 * of the list.  This is becase many operations (pusk, pop, delete, etc) may
 * need to modify the list head itself.
 *
 * # MU_LIST_REF and MU_LIST_CONTAINER
 *
 * A linked list with no associated data isn't particularly useful.  Instead,
 * you will typically embed the list head field in a structure that contains
 * other data.  To simplify accessing the list and its enclosing structure,
 * mu_list procides two macros: MU_LIST_REF and MU_LIST_CONTAINER.  To show how
 * they work, consider this example for a linked list of symbols:
 *
 * @code
 * typedef struct {
 *   const char *name;
 *   int value;
 *   mu_list_t link_field;
 * } symbol_t;
 *
 * // Access the next symbol in the list, or NULL if at the end of the list.
 * symbol_t *symbol_next(symbol_t *symbol) {
 *     mu_list_t *link = MU_LIST_REF(symbol, link_field);
 *     mu_list_t *next = mu_list_next(link);
 *     if (mu_list_is_empty(next)) {
 *         return NULL;   // end of list
 *     } else {
 *         return MU_LIST_CONTAINER(next, symbol_t, link_field);
 *     }
 * }
 * @endcode
 *
 * Note that the MU_LIST_REF extracts the list reference from the structure,
 * while MU_LIST_CONTAINER extracts() gets a reference to the structure
 * containing a list refernce.
 */

#ifndef _MU_LIST_H_
#define _MU_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// types and definitions

typedef struct _mu_list {
  struct _mu_list *next;
} mu_list_t;

/**
 * @brief Signature for a function passed to mu_iist_traverse.
 *
 * @param ref A pointer to the list head.
 * @param arg A user-supplied argument.
 * @return A NULL value to continue traversing, a non-null value to stop.
 */
typedef void *(*mu_list_traverse_fn)(mu_list_t *ref, void *arg);

#define MU_LIST_END ((mu_list_t *)NULL)

/**
 * @brief Given a pointer to a structure and the name of the slot containing a
 * mu_list_t, return a pointer to the mu_list_t.
 */
#define MU_LIST_REF(_struct_p, _list_slot) (&((_struct_p)->_list_slot))

/**
 * @brief Given a pointer to a mu_list_t slot within a containing structure,
 * return a pointer to the containing structure.
 */
#define MU_LIST_CONTAINER(_ptr, _type, _member) \
  ((_type *)((char *)(1 ? (_ptr) : &((_type *)0)->_member) - offsetof(_type, _member)))

// =============================================================================
// declarations

/**
 * @brief Return a reference to the next element in the list, or NULL if the
 * list is empty.
 *
 * Note: as a convenience, a NULL reference is treated as an empty list.
 *
 * @param ref A pointer to the list head.
 * @return A reference to the next list element, or NULL if at the list end.
 */
mu_list_t *mu_list_rest(mu_list_t *ref);

/**
 * @brief Set the next element of a ilst.
 *
 * @param ref A pointer to the list head.
 * @param element The element to be set.
 */
mu_list_t *mu_list_set_rest(mu_list_t *ref, mu_list_t *rest);

/**
 * @brief Return true if the list is the null list (is empty).
 *
 * @param ref A pointer to the list head.
 * @return True if ref or ref->next is NULL.
 */
bool mu_list_is_empty(mu_list_t *ref);

/**
 * @brief Push an item onto a list.  Modifies list head by reference.
 *
 * TODO: create a safe and a fast version of this function.
 *
 * @param ref A pointer to the list head
 * @param item A pointer to list item.
 * @return The updated list head.
 */
mu_list_t *mu_list_push(mu_list_t *ref, mu_list_t *item);

/**
 * @brief Remove the first element from a list.
 *
 * @param ref A pointer to the list head.
 * @return THe first element, removed from the list.  Also returns NULL if the
 * list is empty.
  */
mu_list_t *mu_list_pop(mu_list_t *ref);

/**
 * @brief Call traverse_fn with each successive element of a list, stopping at
 * the end of the list or when the traverse_fn returns a non-NULL value.
 *
 * @param ref A pointer to the ilst head.
 * @param fn The function to call on each element of the list.
 * @param arg A user-supplied argument, passed as the second argument to fn.
 * @return The final value returned from fn.
 */
void *mu_list_traverse(mu_list_t *ref, mu_list_traverse_fn fn, void *arg);

/**
 * @brief Return the number of elements in a list.
 *
 * @param ref A pointer to the list head.
 * @return The number of elements in the list.
 */
int mu_list_length(mu_list_t *ref);

/**
 * @brief Find an item in a list.
 *
 * If the item is present in the list, returns a reference to the item, else
 * returns MU_LIST_END
 *
 * @param ref A pointer to the list head.
 * @param item The item to be found.
 * @return A reference to the found item.
 */
mu_list_t *mu_list_find(mu_list_t *ref, mu_list_t *item);

/**
 * @brief Return true if an item is a member of the list.
 *
 * @param ref A pointer to the list head.
 * @param item The item to be checked for membership.
 * @return True if the item is a member of the list.
 */
bool mu_list_contains(mu_list_t *ref, mu_list_t *item);

/**
 * @brief Delete an item from a list.
 *
 * @param ref A pointer to the list head.
 * @param item The item to be deleted.
 * @return item if the item was removed from the list, NULL otherwise.
 */
mu_list_t *mu_list_delete(mu_list_t *ref, mu_list_t *item);

/**
 * @brief In-place list reversal.
 *
 * @param ref A pointer to the list head.
 * @return ref, now pointing to the reversed list.
 */
mu_list_t *mu_list_reverse(mu_list_t *ref);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_LIST_H_ */
