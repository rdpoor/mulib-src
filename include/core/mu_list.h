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
 * # MU_LIST_REF and MU_LIST_CONTAINER
 *
 * A linked list with no associated data isn't particularly useful.  Instead,
 * you will typically embed the mu_list_t structure in a your that contains
 * other data.  To simplify accessing the list and its enclosing structure,
 * mu_list provides two macros: MU_LIST_REF and MU_LIST_CONTAINER.  To show
 * how they work, consider this example for a linked list of symbols:
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
 *     if (symbol == NULL) {
 *         return NULL;
 *     }
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
 *
 * # list operations
 *
 * Every list is has a header which is not considered part of the list; it is
 * a referent to the list.  And all operations on list objects take a pointer
 * to that header object.  What's more, header->next points to the first element
 * of the list and header->last points to the last element in the list.  This
 * approach simplifies the code, especially operations that modify the first (or
 * last) items in the list.  It also makes finding the first and last elements
 * of the list especially fast.
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
 * @param prev A pointer to the previous element in the list.
 * @param arg A user-supplied argument.
 * @return A NULL value to continue traversing, a non-null value to stop.
 */
typedef void *(*mu_list_traverse_fn)(mu_list_t *prev, void *arg);

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
 * @brief Initialize a list head.
 *
 * @param list A pointer to the list head.
 * @return A pointer to the initialized list head.
 */
mu_list_t *mu_list_init(mu_list_t *list);

/**
 * @brief Return true if the list is the null list (is empty).
 *
 * @param ref A pointer to the list head.
 * @return True if ref or ref->next is NULL.
 */
bool mu_list_is_empty(mu_list_t *list);

/**
 * @brief Return the number of elements in a list.
 *
 * @param ref A pointer to the list head.
 * @return The number of elements in the list.
 */
size_t mu_list_length(mu_list_t *list);

/**
 * @brief Return true if an item is a member of the list.
 *
 * @param ref A pointer to the list head.
 * @param item The item to be checked for membership.
 * @return True if the item is a member of the list.
 */
bool mu_list_contains(mu_list_t *list, mu_list_t *element);

/**
 * @brief Return a reference to the first element in the list, or NULL if the
 * list is empty.
 *
 * @param list A pointer to the list head.
 * @return A reference to the first list element, or NULL on empty list.
 */
mu_list_t *mu_list_first(mu_list_t *list);

/**
 * @brief Push an item onto the head ot the list.
 *
 * @param list A pointer to the list head
 * @param element A pointer to a list item.
 * @return The list head.
 */
mu_list_t *mu_list_push(mu_list_t *list, mu_list_t *element);

/**
 * @brief Remove the first element from a list.
 *
 * @param list A pointer to the list head.
 * @return The element removed from the list head, or NULL if the list is empty.
  */
mu_list_t *mu_list_pop(mu_list_t *list);

/**
 * @brief Find an item in a list, searching from the head.
 *
 * @param list A pointer to the list head.
 * @param element The item to be found.
 * @return A pointer to the found element or NULL if the element is not in the
 * list.
 */
mu_list_t *mu_list_find(mu_list_t *list, mu_list_t *element);

/**
 * @brief Delete an element from a list.
 *
 * @param list A pointer to the list head.
 * @param element A pointer the element to delete.
 * @return the element removed from the list, or NULL if it was not in the list
 */
mu_list_t *mu_list_delete(mu_list_t *list, mu_list_t *element);

/**
 * @brief In-place list reversal.
 *
 * @param list A pointer to the list head.
 * @return list, now pointing to the reversed list.
 */
mu_list_t *mu_list_reverse(mu_list_t *list);

/**
 * @brief Call traverse_fn with each successive element of a list, stopping at
 * the end of the list or when the traverse_fn returns a non-NULL value.
 *
 * @param list A pointer to the list head.
 * @param fn The function to call on each element of the list.
 * @param arg A user-supplied argument, passed as the second argument to fn.
 * @return The final value returned from fn.
 */
void *mu_list_traverse(mu_list_t *list, mu_list_traverse_fn fn, void *arg);

/**
 * @brief Return the next element in a list.
 *
 * @param element The element
 * @return The next element in the list or NULL if at the end of the list.
 */
mu_list_t *mu_list_next_element(mu_list_t *element);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_LIST_H_ */
