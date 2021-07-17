/**
 * MIT License
 *
 * Copyright (c) 2020 R. D. Poor <rdpoor@gmail.com>
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
 * @file mu_array.h
 *
 * @brief A suite of general utility functions that operate on arrrays
 */

#ifndef MU_ARRAY_H_
#define MU_ARRAY_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stddef.h>

// =============================================================================
// types and definitions

/**
 * @brief Signature of an array comparison function.
 */
typedef int (*mu_array_cmp_fn)(void *e1, void *e2);


// =============================================================================
// declarations

/**
 * @brief In-place heapsort
 */
void mu_array_sort(void *elements,
                   size_t element_count,
                   mu_array_cmp_fn comparison_fn,
                   size_t element_size);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef MU_ARRAY_H_ */
