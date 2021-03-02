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
 * @file Doubly-linked list support.
 *
 * # MU_DLIST_REF and MU_DLIST_CONTAINER
 *
 * A linked list with no associated data isn't particularly useful.  Instead,
 * you will typically embed the mu_dlist_t structure in a your that contains
 * other data.  To simplify accessing the list and its enclosing structure,
 * mu_dlist provides two macros: MU_DLIST_REF and MU_DLIST_CONTAINER.  To show
 * how they work, consider this example for a linked list of symbols:
 *
 * @code
 * typedef struct {
 *   const char *name;
 *   int value;
 *   mu_dlist_t link_field;
 * } symbol_t;
 *
 * // Access the next symbol in the list, or NULL if at the end of the list.
 * symbol_t *symbol_next(symbol_t *symbol) {
 *     if (symbol == NULL) {
 *         return NULL;
 *     }
 *     mu_dlist_t *link = MU_DLIST_REF(symbol, link_field);
 *     mu_dlist_t *next = mu_dlist_next(link);
 *     if (mu_dlist_is_empty(next)) {
 *         return NULL;   // end of list
 *     } else {
 *         return MU_DLIST_CONTAINER(next, symbol_t, link_field);
 *     }
 * }
 * @endcode
 *
 * Note that the MU_DLIST_REF extracts the list reference from the structure,
 * while MU_DLIST_CONTAINER extracts() gets a reference to the structure
 * containing a dlist refernce.
 *
 * # dlist operations
 *
 * Every dlist is has a header which is not considered part of the list; it is
 * a referent to the list.  And all operations on dlist objects take a pointer
 * to that header object.  What's more, header->next points to the first element
 * of the dlist and header->last points to the last element in the list.  This
 * approach simplifies the code, especially operations that modify the first (or
 * last) items in the list.  It also makes finding the first and last elements
 * of the list especially fast.
 */

#ifndef _MU_DLIST_H_
#define _MU_DLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// types and definitions

typedef struct _mu_dlist {
  struct _mu_dlist *prev;
  struct _mu_dlist *next;
} mu_dlist_t;

/**
 * @brief Signature for a function passed to mu_iist_traverse.
 *
 * @param list A pointer to the list head.
 * @param arg A user-supplied argument.
 * @return A NULL value to continue traversing, a non-null value to stop.
 */
typedef void *(*mu_dlist_traverse_fn)(mu_dlist_t *list, void *arg);

/**
 * @brief Given a pointer to a structure and the name of the slot containing a
 * mu_dlist_t, return a pointer to the mu_dlist_t.
 */
#define MU_DLIST_REF(_struct_p, _list_slot) (&((_struct_p)->_list_slot))

/**
 * @brief Given a pointer to a mu_dlist_t slot within a containing structure,
 * return a pointer to the containing structure.
 */
#define MU_DLIST_CONTAINER(_ptr, _type, _member) \
  ((_type *)((char *)(1 ? (_ptr) : &((_type *)0)->_member) - offsetof(_type, _member)))

// =============================================================================
// declarations

/**
 * @brief Initialize a list head.
 *
 * @param list A pointer to the list head.
 * @return A pointer to the initialized list head.
 */
mu_dlist_t *mu_dlist_init(mu_dlist_t *list);

/**
 * @brief Return true if the list is the null list (is empty).
 *
 * @param ref A pointer to the list head.
 * @return True if ref or ref->next is NULL.
 */
bool mu_dlist_is_empty(mu_dlist_t *list);

/**
 * @brief Return the number of elements in a list.
 *
 * @param ref A pointer to the list head.
 * @return The number of elements in the list.
 */
size_t mu_dlist_length(mu_dlist_t *list);

/**
 * @brief Return true if an item is a member of the list.
 *
 * @param ref A pointer to the list head.
 * @param item The item to be checked for membership.
 * @return True if the item is a member of the list.
 */
bool mu_dlist_contains(mu_dlist_t *list, mu_dlist_t *element);

/**
 * @brief Return a reference to the first element in the list, or NULL if the
 * list is empty.
 *
 * @param list A pointer to the list head.
 * @return A reference to the first list element, or NULL on empty list.
 */
mu_dlist_t *mu_dlist_first(mu_dlist_t *list);

/**
 * @brief Return a reference to the last element in the list, or NULL if the
 * list is empty.
 *
 * @param list A pointer to the list head.
 * @return A reference to the last list element, or NULL on empty list.
 */
mu_dlist_t *mu_dlist_last(mu_dlist_t *list);

/**
 * @brief Push an item onto the head ot the list.
 *
 * @param list A pointer to the list head
 * @param element A pointer to a list item.
 * @return The list head.
 */
mu_dlist_t *mu_dlist_push(mu_dlist_t *list, mu_dlist_t *element);

/**
 * @brief Push an item onto the tail of the list.
 *
 * @param list A pointer to the list head
 * @param element A pointer to list item.
 * @return The list head.
 */
mu_dlist_t *mu_dlist_lpush(mu_dlist_t *list, mu_dlist_t *element);

/**
 * @brief Remove the first element from a list.
 *
 * @param list A pointer to the list head.
 * @return The element removed from the list head, or NULL if the list is empty.
  */
mu_dlist_t *mu_dlist_pop(mu_dlist_t *list);

/**
 * @brief Remove the last element from a list.
 *
 * @param list A pointer to the list head.
 * @return The element removed from the list tail, or NULL if the list is empty.
 */
mu_dlist_t *mu_dlist_lpop(mu_dlist_t *list);

/**
 * @brief Find an item in a list, searching from the head.
 *
 * @param list A pointer to the list head.
 * @param element The item to be found.
 * @return A pointer to the found element or NULL if the element is not in the
 * list.
 */
mu_dlist_t *mu_dlist_find(mu_dlist_t *list, mu_dlist_t *element);

/**
 * @brief Find an item in a list, searching from the tail.
 *
 * @param list A pointer to the list head.
 * @param element The item to be found.
 * @return A pointer to the found element or NULL if the element is not in the
 * list.
 */
mu_dlist_t *mu_dlist_lfind(mu_dlist_t *list, mu_dlist_t *element);

/**
 * @brief In-place list reversal.
 *
 * @param list A pointer to the list head.
 * @return list, now pointing to the reversed list.
 */
mu_dlist_t *mu_dlist_reverse(mu_dlist_t *list);

/**
 * @brief Call traverse_fn with each successive element of a list, stopping at
 * the end of the list or when the traverse_fn returns a non-NULL value.
 *
 * @param list A pointer to the list head.
 * @param fn The function to call on each element of the list.
 * @param arg A user-supplied argument, passed as the second argument to fn.
 * @return The final value returned from fn.
 */
void *mu_dlist_traverse(mu_dlist_t *list, mu_dlist_traverse_fn fn, void *arg);

/**
 * @brief Call traverse_fn with each successive element of a list working from
 * the end of the list.  Stops at the head of the list or when traverse_fn
 * returns non-NULL value.
 *
 * @param list A pointer to the list head.
 * @param fn The function to call on each element of the list.
 * @param arg A user-supplied argument, passed as the second argument to fn.
 * @return The final value returned from fn.
 */
void *mu_dlist_ltraverse(mu_dlist_t *list, mu_dlist_traverse_fn fn, void *arg);

/**
 * @brief Return true if an element is part of a linked list.
 *
 * @param element The element to test.
 * @return true if the element is part of a list.
 */
bool mu_dlist_element_is_linked(mu_dlist_t *element);

/**
 * @brief Return the next element in a list.
 *
 * @param element The element
 * @return The next element in the list or NULL if at the end of the list.
 */
mu_dlist_t *mu_dlist_next_element(mu_dlist_t *element);

/**
 * @brief Return the prev element in a list.
 *
 * @param element The element
 * @return The next element in the list or NULL if at the end of the list.
 */
mu_dlist_t *mu_dlist_prev_element(mu_dlist_t *element);

/**
 * @brief Delete an element from a list.
 *
 * Note: Call this function only if the element is linked in a list.
 *
 * @param element A pointer the element to unlink.
 * @return element
 */
mu_dlist_t *mu_dlist_unlink_element(mu_dlist_t *element);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_DLIST_H_ */
