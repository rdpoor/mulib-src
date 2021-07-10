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

#ifndef _MU_PSTORE_H_
#define _MU_PSTORE_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdbool.h>
#include <string.h>

// =============================================================================
// types and definitions

typedef enum {
  MU_PSTORE_ERR_NONE,
  MU_PSTORE_ERR_EMPTY,
  MU_PSTORE_ERR_FULL,
  MU_PSTORE_ERR_INDEX,
  MU_PSTORE_ERR_NOT_FOUND,
} mu_pstore_err_t;

typedef void * mu_pstore_item_t;

typedef struct {
  mu_pstore_item_t *items;
  size_t capacity;
  size_t count;
} mu_pstore_t;


/**
 * @brief Signature for comparison function.
 *
 * A comparison function should return a negative, zero, or positive value if
 * the item referred to by item1 is less than, equal to , or greater than the
 * item referred to by item2.
 */
typedef int (*mu_pstore_compare_fn)(void *item1, void *item2);

/**
 * @brief Signature for filter function.
 *
 * A filter function should return true if the indicated item matches a user-
 * specified criterion.
 */
typedef bool (*mu_pstore_filter_fn)(void *item);

// =============================================================================
// declarations

/**
 * @brief Initialize a pstore
 *
 * @param pstore The pstore structure to initialize.
 * @param items A backing store of void * items.
 * @param capacity The number of items in the backing store.
 * @return pstore itself.
 */
mu_pstore_t *mu_pstore_init(mu_pstore_t *pstore, mu_pstore_item_t *items, size_t capacity);

/**
 * @brief Empty a pstore.
 *
 * mu_pstore_reset() sets the pstore's count to zero.
 *
 * @param pstore The pstore structure.
 * @return pstore itself.
 */
mu_pstore_t *mu_pstore_reset(mu_pstore_t *pstore);

/**
 * @brief Access a pstore's backing store.
 *
 * @param pstore The pstore structure.
 * @return Pointer the backing store.
 */
mu_pstore_item_t *mu_pstore_items(mu_pstore_t *pstore);

/**
 * @brief Get the capacity of the pstore.
 *
 * This returns the maximum number of items that the pstore can hold.
 *
 * @param pstore The pstore structure.
 * @return The capacity of the pstore.
 */
size_t mu_pstore_capacity(mu_pstore_t *pstore);

/**
 * @brief Get the number of items in the pstore.
 *
 * This returns the number of items currently in the pstore.
 *
 * @param pstore The pstore structure.
 * @return The number of items in the pstore.
 */
size_t mu_pstore_count(mu_pstore_t *pstore);

/**
 * @brief Return true if the pstore is empty.
 *
 * This returns true if there are no items in the pstore.
 *
 * @param pstore The pstore structure.
 * @return true if the pstore is empty.
 */
bool mu_pstore_is_empty(mu_pstore_t *pstore);

/**
 * @brief Return true if the pstore is full.
 *
 * This returns true if the pstore is full.
 *
 * @param pstore The pstore structure.
 * @return true if the pstore is full.
 */
bool mu_pstore_is_full(mu_pstore_t *pstore);

/**
 * @brief return true if the pstore contains the given item
 *
 * @param pstore The pstore structure.
 * @param item The item to be to be tested.
 * @return true if the item is in the pstore.
 */
bool mu_pstore_contains(mu_pstore_t *pstore, mu_pstore_item_t item);

/**
 * @brief return the index of an item, or -1 if not present in the store
 *
 * @param pstore The pstore structure.
 * @param item The item to be to be searched
 * @return the index of the item or -1 if not present in the store
 */
int mu_pstore_index_of(mu_pstore_t *pstore, mu_pstore_item_t item);

/**
 * @brief Push an item onto the end of the pstore.
 *
 * @param pstore The pstore structure.
 * @param item The item to be pushed onto the end of the pstore.
 * @return MU_PSTORE_ERR_FULL if the pstore was full before the push operation,
 *         MU_PSTORE_ERR_NONE otherwise.
 */
mu_pstore_err_t mu_pstore_push(mu_pstore_t *pstore, mu_pstore_item_t item);

/**
 * @brief Pop an item from the end of the pstore.
 *
 * mu_pstore_pop() returns the popped item by reference in *item.  In case of
 * an error, *item is set to NULL.
 *
 * @param pstore The pstore structure.
 * @param item Pointer for item popped from the end of the pstore.
 * @return MU_PSTORE_ERR_EMPTY if the pstore was empty before the pop operation,
 *         MU_PSTORE_ERR_NONE otherwise.
 */
mu_pstore_err_t mu_pstore_pop(mu_pstore_t *pstore, mu_pstore_item_t *item);

/**
 * @brief Peek at the last item of the pstore.
 *
 * mu_pstore_peek() returns the pstore's last item by reference in *item.  In
 * case of an error, *item is set to NULL.
 *
 * @param pstore The pstore structure.
 * @param item Pointer for item fetched from the end of the pstore.
 * @return MU_PSTORE_ERR_EMPTY if the pstore was empty before the call to
 *         mu_pstore_peek, MU_PSTORE_ERR_NONE otherwise.
 */
mu_pstore_err_t mu_pstore_peek(mu_pstore_t *pstore, mu_pstore_item_t *item);

/**
 * @brief Insert an item at the given index.
 *
 * mu_pstore_insert_at() inserts an item at the given index of the pstore.
 *
 * @param pstore The pstore structure.
 * @param item Item to be inserted into the pstore.
 * @param index The index at which to insert the item.
 * @return MU_PSTORE_ERR_FULL if the pstore was full before the call to
 *         mu_pstore_insert_at(), MU_PSTORE_ERR_INDEX if the given index is
 *         greater than mu_pstore_count(), MU_PSTORE_ERR_NONE otherwise.
 */
mu_pstore_err_t mu_pstore_insert_at(mu_pstore_t *pstore, mu_pstore_item_t item, size_t index);

/**
 * @brief Delete an item.
 *
 * mu_pstore_delete_at() removes an item from the pstore,
 *
 * @param pstore The pstore structure.
 * @param item The item to removed from the pstore.
 * @return item if the item was removed, NULL if the item was not present.
 */
mu_pstore_item_t mu_pstore_delete(mu_pstore_t *pstore, mu_pstore_item_t item);

/**
 * @brief Delete an item at the given index.
 *
 * mu_pstore_delete_at() removes an item at the given index of the pstore,
 * returning the item by reference in *item.
 *
 * @param pstore The pstore structure.
 * @param item Pointer to the item to removed from the pstore.
 * @param index The index from which to remove the item.
 * @return MU_PSTORE_ERR_EMPTY if the pstore was empty before the call to
 *         mu_pstore_delete_at(), MU_PSTORE_ERR_INDEX if the given index is
 *         greater than mu_pstore_count(), MU_PSTORE_ERR_NONE otherwise.
 */
mu_pstore_err_t mu_pstore_delete_at(mu_pstore_t *pstore, mu_pstore_item_t *item, size_t index);

/**
 * @brief In-place filtering of a pstore, leaving only those that satisfy the
 * given filter functins.
 *
 * The user-supplied filter function is called with a void * argument, and
 * should return true if that element is to be preserved and false if it is
 * to be removed from the pstore.
 *
 * TODO: NOT YET IMPLEMENTED.
 *
 * @param pstore The pstore structure.
 * @param match The user supplied filter function.
 * @return MU_PSTORE_ERR_NONE.
 */
mu_pstore_err_t mu_pstore_filter(mu_pstore_t *pstore, mu_pstore_filter_fn match);

/**
 * @brief Insert an item into a sorted list according to a user-supplied
 * comparison function.
 *
 * Note: the list must sorted prior to the call to mu_pstore_insert_sorted(),
 * else the results are undefined.  See mu_pstore_sort() if you need to sort
 * the store first.
 *
 * The user-supplied comparison function is called with two void * arguments,
 * and should return an integer less than, equal to, or greater than zero if
 * the first argument is less than, equal to, or greater than the second
 * argument.
 *
 * @param pstore The pstore structure.
 * @param item The item to be inserted into the pstore.
 * @param cmp The user supplied comparison function.
 * @return MU_PSTORE_ERR_FULL if the pstore was full prior to the call to
 *         mu_pstore_insert_sorted, MU_PSTORE_ERR_NONE otherwise.
 */
mu_pstore_err_t mu_pstore_insert_sorted(mu_pstore_t *pstore, mu_pstore_item_t item, mu_pstore_compare_fn cmp);

/**
 * @brief In-place sorting of a items in a pstore.
 *
 * mu_pstore_sort() performs an in-place heapsort on its items according to
 * a user-supplied comparison function.  The heapsort operation is O(log N)
 * with very low overhead.
 *
 * The user-supplied comparison function is called with two void * arguments,
 * and should return an integer less than, equal to, or greater than zero if
 * the first argument is less than, equal to, or greater than the second
 * argument.
 *
 * @param pstore The pstore structure.
 * @param cmp The user supplied comparison function.
 * @return MU_PSTORE_ERR_NONE
 */
mu_pstore_err_t mu_pstore_sort(mu_pstore_t *pstore, mu_pstore_compare_fn cmp);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_PSTORE_H_ */
