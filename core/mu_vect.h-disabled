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

#ifndef _MU_VECT_H_
#define _MU_VECT_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stddef.h>
#include <stdbool.h>

// =============================================================================
// types and definitions

typedef struct {
  void *elements;
  size_t element_size;
  size_t capacity;
  size_t count;
} mu_vect_t;


/**
 * @brief Signature of a comparison function.
 */
typedef int (*mu_vect_cmp_fn)(void *e1, void *e2);

/**
 * @brief Signature of a traversal function.
 */
typedef void *(*mu_vect_find_fn)(void *e, void *arg);

typedef enum {
  MU_VECT_ERR_NONE,
  MU_VECT_ERR_EMPTY,
  MU_VECT_ERR_FULL,
  MU_VECT_ERR_INDEX,
  MU_VECT_ERR_NOT_FOUND,
} mu_vect_err_t;

// =============================================================================
// declarations

/**
 * @brief Initialize a mu_vect
 *
 * @param mu_vect The mu_vect structure to initialize.
 * @param elements A backing store of void * elements.
 * @param capacity The number of elements in the backing store.
 * @param element_size The size (in bytes) of each element.
 * @return mu_vect itself.
 */
mu_vect_t *mu_vect_init(mu_vect_t *vect, void *elements, size_t capacity, size_t element_size);

/**
 * @brief Empty a mu_vect.
 *
 * mu_vect_reset() sets the mu_vect's count to zero.
 *
 * @param mu_vect The mu_vect structure.
 * @return mu_vect itself.
 */
mu_vect_t *mu_vect_reset(mu_vect_t *vect);

/**
 * @brief Access a mu_vect's backing store.
 *
 * @param mu_vect The mu_vect structure.
 * @return Pointer the backing store.
 */
void *mu_vect_elements(mu_vect_t *vect);

/**
 * @brief Get the capacity of the mu_vect.
 *
 * This returns the maximum number of elements that the mu_vect can hold.
 *
 * @param mu_vect The mu_vect structure.
 * @return The capacity of the mu_vect.
 */
size_t mu_vect_capacity(mu_vect_t *vect);

/**
 * @brief Get the number of elements in the mu_vect.
 *
 * This returns the number of elements currently in the mu_vect.
 *
 * @param mu_vect The mu_vect structure.
 * @return The number of elements in the mu_vect.
 */
size_t mu_vect_count(mu_vect_t *vect);

/**
 * @brief Get the size of each element in the mu_vect.
 *
 * @param mu_vect The mu_vect structure.
 * @return The size (in bytes) of each element.
 */
size_t mu_vect_element_size(mu_vect_t *vect);

/**
 * @brief Return true if the mu_vect is empty.
 *
 * This returns true if there are no elements in the mu_vect.
 *
 * @param mu_vect The mu_vect structure.
 * @return true if the mu_vect is empty.
 */
bool mu_vect_is_empty(mu_vect_t *vect);

/**
 * @brief Return true if the mu_vect is full.
 *
 * This returns true if the mu_vect is full.
 *
 * @param mu_vect The mu_vect structure.
 * @return true if the mu_vect is full.
 */
bool mu_vect_is_full(mu_vect_t *vect);

/**
 * @brief Return a pointer to the referenced element.
 *
 * @param vect The mu_vect structure.
 * @param i An index.
 * @return A pointer to the referenced element if i is in bounds else NULL.
 */
void *mu_vect_ref(mu_vect_t *vect, size_t index);

/**
 * @brief Peek at the last element of the mu_vect.
 *
 * mu_vect_peek() returns the mu_vect's last element by reference in *e.  In
 * case of an error, *e is set to NULL.
 *
 * @param mu_vect The mu_vect structure.
 * @param e Pointer for storage location for peeked-at element.
 * @return MU_VECT_ERR_EMPTY if the mu_vect was empty before the call to
 *         mu_vect_peek, MU_VECT_ERR_NONE otherwise.
 */
mu_vect_err_t mu_vect_peek(mu_vect_t *vect, void *e);

/**
 * @brief Insert an element at the given index.
 *
 * mu_vect_insert_at() inserts an element at the given index of the mu_vect.
 *
 * @param mu_vect The mu_vect structure.
 * @param index The index at which to insert the element.
 * @param e Item to be inserted into the mu_vect.
 * @return MU_VECT_ERR_FULL if the mu_vect was full before the call to
 *         mu_vect_insert_at(), MU_VECT_ERR_INDEX if the given index is
 *         greater than mu_vect_count(), MU_VECT_ERR_NONE otherwise.
 */
mu_vect_err_t mu_vect_insert_at(mu_vect_t *vect, size_t index, void *e);

/**
 * @brief Push an element onto the end of the mu_vect.
 *
 * @param mu_vect The mu_vect structure.
 * @param e Pointer to the element to be pushed onto the end of the mu_vect.
 * @return MU_VECT_ERR_FULL if the mu_vect was full before the push operation,
 *         MU_VECT_ERR_NONE otherwise.
 */
mu_vect_err_t mu_vect_push(mu_vect_t *vect, void *e);

/**
 * @brief Delete an element.
 *
 * mu_vect_delete() removes an element from the mu_vect,
 *
 * @param mu_vect The mu_vect structure.
 * @param index The index at which to insert the element.
 * @param e Pointer to storage to receive the deleted item or NULL.
 * @return MU_VECT_ERR_EMPTY if the mu_vect was empty before the call to
 *         mu_vect_insert_at(), MU_VECT_ERR_INDEX if the given index is
 *         greater or equal to mu_vect_count(), MU_VECT_ERR_NONE otherwise.
 */
mu_vect_err_t mu_vect_delete_at(mu_vect_t *vect, size_t index, void *e);

/**
 * @brief Pop an element from the end of the mu_vect.
 *
 * mu_vect_pop() returns the popped element by reference in *e.  In case of
 * an error, *e is set to NULL.
 *
 * @param mu_vect The mu_vect structure.
 * @param e Pointer for storage location for popped element.
 * @return MU_VECT_ERR_EMPTY if the mu_vect was empty before the pop operation,
 *         MU_VECT_ERR_NONE otherwise.
 */
mu_vect_err_t mu_vect_pop(mu_vect_t *vect, void *e);

/**
 * @brief Insert an element into a sorted list according to a user-supplied
 * comparison function.
 *
 * Note: the list must sorted prior to the call to mu_vect_insert_sorted(),
 * else the results are undefined.  See mu_vect_sort() if you need to sort
 * the store first.
 *
 * The user-supplied comparison function is called with two void * arguments,
 * and should return an integer less than, equal to, or greater than zero if
 * the first argument is less than, equal to, or greater than the second
 * argument.
 *
 * @param mu_vect The mu_vect structure.
 * @param e The element to be inserted into the mu_vect.
 * @param cmp The user supplied comparison function.
 * @return MU_VECT_ERR_FULL if the mu_vect was full prior to the call to
 *         mu_vect_insert_sorted, MU_VECT_ERR_NONE otherwise.
 */
mu_vect_err_t mu_vect_insert_sorted(mu_vect_t *vect, void *e, mu_vect_cmp_fn cmp);

/**
 * @brief In-place sorting of a elements in a mu_vect.
 *
 * mu_vect_sort() performs an in-place heapsort on its elements according to
 * a user-supplied comparison function.  The heapsort operation is O(log N)
 * with very low overhead.
 *
 * The user-supplied comparison function is called with two void * arguments,
 * and should return an integer less than, equal to, or greater than zero if
 * the first argument is less than, equal to, or greater than the second
 * argument.
 *
 * @param mu_vect The mu_vect structure.
 * @param cmp The user supplied comparison function.
 * @return MU_VECT_ERR_NONE
 */
mu_vect_err_t mu_vect_sort(mu_vect_t *vect, mu_vect_cmp_fn cmp);

/**
 * @brief Traverse the mu_vect with a user-supplied function.
 *
 * mu_vect_traverse visits each element of the vector starting with element 0,
 * calling the user-supplied traversal function with each element.  If the user
 * supplied function returns a non-null value, traversal stops and that value is
 * returned.
 *
 * @param mu_vect the mu_vect structure.
 * @param find_fn The user-supplied traversal function, which is called with
 * each element of the vector until the find_fn returns a non-null value or
 * the last element is visited.
 * @return Last value returned by find_fn.
 */
void *mu_vect_traverse(mu_vect_t *vect, mu_vect_find_fn find_fn, void *arg);

int mu_vect_find_index(mu_vect_t *vect, mu_vect_find_fn find_fn, void *arg);

bool mu_vect_contains(mu_vect_t *vect, mu_vect_find_fn find_fn, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_VECT_H_ */
