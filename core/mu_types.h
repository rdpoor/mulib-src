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

#ifndef MU_TYPES_H_
#define MU_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdbool.h>

// =============================================================================
// types and definitions

/**
 * @brief Signature for comparison function.
 *
 * A comparison function should return a negative, zero, or positive value if
 * the item referred to by item1 is less than, equal to , or greater than the
 * item referred to by item2.
 */
typedef int (*mu_compare_fn)(void *item1, void *item2);

/**
 * @brief Signature for a deferred function.
 *
 * A deferred function will be called with its context argument and a user-
 * supplied argument.  The value returned is implementation dependent, but two
 * common choices are the context and NULL.
 */
typedef void *(*mu_deferred_fn)(void *context, void *arg);

/**
 * @brief Signature for filter function.
 *
 * A filter function should return true if the indicated item matches a user-
 * specified criterion.
 */
typedef bool (*mu_filter_fn)(void *item);

// =============================================================================
// declarations

#ifdef __cplusplus
}
#endif

#endif /* #ifndef MU_TYPES_H_ */
