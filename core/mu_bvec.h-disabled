/**
 * @file
 *
 * @brief Support for bit vectors.
 *
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

#ifndef _MU_BVEC_H_
#define _MU_BVEC_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// =============================================================================
// types and definitions

/**
 * @brief Compute the number of bytes required to hold the given number of bits.
 */
#define MU_BVEC_COUNT_TO_BYTE_COUNT(_bit_count) (((_bit_count - 1) / 8) + 1)

/**
 * @brief A bit vector is an array of bytes.
 */
typedef uint8_t mu_bvec_t;

// =============================================================================
// declarations

size_t mu_bvec_byte_index(size_t bit_index);
uint8_t mu_bvec_byte_mask(size_t bit_index);

// Low-level operations that assume you have byte_index and byte_mask
void mu_bvec_set_(size_t byte_index, uint8_t byte_mask, mu_bvec_t *store);
void mu_bvec_clear_(size_t byte_index, uint8_t byte_mask, mu_bvec_t *store);
void mu_bvec_invert_(size_t byte_index, uint8_t byte_mask, mu_bvec_t *store);
void mu_bvec_write_(size_t byte_index,
                    uint8_t byte_mask,
                    mu_bvec_t *store,
                    bool value);
bool mu_bvec_read_(size_t byte_index, uint8_t byte_mask, mu_bvec_t *store);

// Same, but take bit index instead
void mu_bvec_set(size_t bit_index, mu_bvec_t *store);
void mu_bvec_clear(size_t bit_index, mu_bvec_t *store);
void mu_bvec_invert(size_t bit_index, mu_bvec_t *store);
void mu_bvec_write(size_t bit_index, mu_bvec_t *store, bool value);
bool mu_bvec_read(size_t bit_index, mu_bvec_t *store);

// Queries for bit vectors
bool mu_bvec_is_all_ones(size_t bit_count, mu_bvec_t *store);
bool mu_bvec_is_all_zeros(size_t bit_count, mu_bvec_t *store);

size_t mu_bvec_count_ones(size_t bit_count, mu_bvec_t *store);
size_t mu_bvec_count_zeros(size_t bit_count, mu_bvec_t *store);

// Returns SIZE_MAX if not found
size_t mu_bvec_find_first_one(size_t bit_count, mu_bvec_t *store);
size_t mu_bvec_find_first_zero(size_t bit_count, mu_bvec_t *store);
// size_t mu_bvec_find_last_one(size_t bit_count, mu_bvec_t *store);
// size_t mu_bvec_find_last_zero(size_t bit_count, mu_bvec_t *store);

// modify all bits in a bit vector
void mu_bvec_set_all(size_t bit_count, mu_bvec_t *store);
void mu_bvec_clear_all(size_t bit_count, mu_bvec_t *store);
void mu_bvec_invert_all(size_t bit_count, mu_bvec_t *store);
void mu_bvec_write_all(size_t bit_count, mu_bvec_t *store, bool value);

// Consider
// rotate and shift operations
// operations on sub-ranges

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_BVEC_H_ */
