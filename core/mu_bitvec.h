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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// types and definitions

typedef struct {
  size_t byte_count;  //!< number of bytes in backing store
  size_t length;      //!< actual length of the bit vector
  uint8_t *bits;      //!< backing store
} mu_bitvec_t;

/**
 * @brief Convert bit count to byte count.
 *
 * Return the number of bytes requried to hold the given number of bits.
 */
#define BVEC_LENGTH_TO_BYTE_COUNT(_length) (((_length-1)/8)+1)

/**
 * @brief Convert a byte count to a bit count.
 *
 * Return the maximum number of bits available in the given number of bytes.
 */
#define BVEC_BYTE_COUNT_TO_LENGTH(_byte_count) ((_byte_count) * 8)

// =============================================================================
// declarations

/**
 * @brief Initialize a bit vector
 *
 * Initialize the bit vector object with a backing store to hold the bits.
 *
 * Note that in somce cases, you may want a bit vector that is not a multiple
 * of 8 bits; for this reason mu_bitvec_init() takes an additional length field
 * and the BVEC_LENGTH_TO_BYTE_COUNT macro.
 *
 * For example, if you wanted a bit vector that is 12 bits long, you could do
 * the following:
 *
 * @code
 * #define VECTOR_LENGTH 12
 * uint8_t backing_store[BVEC_LENGTH_TO_BYTE_COUNT(VECTOR_LENGTH)];
 * mu_bvec_t bitvec;
 * ...
 * mu_bvec_init(&bitvec, VECTOR_LENGTH, backing_store, sizeof(backing_store));
 * @endcode
 *
 * @param bv Pointer to a mu_bitvec_t structure.
 * @param length Maximum length of the bit vetor in bits.
 * @param bits Pointer to a byte array.
 * @param byte_count Number of bytes in the byte array.
 * @return bv
 */
mu_bitvec_t *mu_bitvec_init(mu_bitvec_t *bv, size_t length, uint8_t *bits, size_t byte_count);

/**
 * @brief Access the bytes underlying the bit vector.
 *
 * @param bv Pointer to a mu_bitvec_t structure.
 * @return Pointer to the byte array used to represent the bit vector.
 */
uint8_t *mu_bitvec_bits(mu_bitvec_t *bv);

/**
 * @brief Get the number of bits in a bit vector.
 *
 * @param bv Pointer to a mu_bitvec_t structure.
 * @return Maximum addressable number of bits in the bit vector.
 */
size_t mu_bitvec_length(mu_bitvec_t *bv);

/**
 * @brief Get the number of bytes used to store the bits.
 *
 * @param bv Pointer to a mu_bitvec_t structure.
 * @return Size of the underlying byte vector (in bytes).
 */
size_t mu_bitvec_byte_count(mu_bitvec_t *bv);

/**
 * @brief Read a bit from the bit vector.
 *
 * Note that if the index i is greater than or equal to the bit vector length,
 * mu_bitvec_read_bit silently ignores the error and returns false.
 *
 * @param bv Pointer to a mu_bitvec_t structure.
 * @param i zero-based bit position to read.
 * @return the value of the i'th bit in the bit vector.
 */
bool mu_bitvec_read_bit(mu_bitvec_t *bv, int i);

/**
 * @brief Write a bit in the bit vector.
 *
 * @param bv Pointer to a mu_bitvec_t structure.
 * @param i zero-based bit position to write.
 * @param value Value to set the i'th bit in the bit vector (true or false).
 * @return On success, return bv.  Return NULL if index is out of range.
 */
mu_bitvec_t *mu_bitvec_write_bit(mu_bitvec_t *bv, int i, bool value);

/**
 * @brief Set a bit in the bit vector to true.
 *
 * @param bv Pointer to a mu_bitvec_t structure.
 * @param i zero-based bit position to set.
 * @return On success, return bv.  Return NULL if index is out of range.
 */
mu_bitvec_t *mu_bitvec_set_bit(mu_bitvec_t *bv, int i);

/**
 * @brief Set a bit in the bit vector to false.
 *
 * @param bv Pointer to a mu_bitvec_t structure.
 * @param i zero-based bit position to clear.
 * @return On success, return bv.  Return NULL if index is out of range.
 */
mu_bitvec_t *mu_bitvec_clear_bit(mu_bitvec_t *bv, int i);

/**
 * @brief Invert a bit in the bit vector.
 *
 * @param bv Pointer to a mu_bitvec_t structure.
 * @param i zero-based bit position to toggle.
 * @return On success, return bv.  Return NULL if index is out of range.
 */
mu_bitvec_t *mu_bitvec_toggle_bit(mu_bitvec_t *bv, int i);

// operations on entire vector
mu_bitvec_t *mu_bitvec_set_all(mu_bitvec_t *bv);
mu_bitvec_t *mu_bitvec_clear_all(mu_bitvec_t *bv);
mu_bitvec_t *mu_bitvec_toggle_all(mu_bitvec_t *bv);

/**
 * Copy the bits contents of src to dst.  In the event that the bit vectors are
 * different lengths, only the bits in the shorter vector are copied.
 */
mu_bitvec_t *mu_bitvec_copy(mu_bitvec_t *dst, mu_bitvec_t *src);
mu_bitvec_t *mu_bitvec_and(mu_bitvec_t *dst, mu_bitvec_t *src);
mu_bitvec_t *mu_bitvec_or(mu_bitvec_t *dst, mu_bitvec_t *src);
mu_bitvec_t *mu_bitvec_xor(mu_bitvec_t *dst, mu_bitvec_t *src);

/**
 * @brief Compare two bit vectors up through the first mu_bitvec_length() bits of
 * the shorter of the two vectors, returning true if all the bits are equal.
 */
bool mu_bitvec_equals(mu_bitvec_t *bv1, mu_bitvec_t *bv2);

bool mu_bitvec_is_all_ones(mu_bitvec_t *bv);
bool mu_bitvec_is_all_zeros(mu_bitvec_t *bv);

int mu_bitvec_count_ones(mu_bitvec_t *bv);
int mu_bitvec_count_zeros(mu_bitvec_t *bv);

// Consider
// int mu_bitvec_find_{rightmost leftmost}_{one zero}(mu_bitvec_t *bv)
// void mu_bitvec_shift_left(mu_bitvec_t *bv, int n_bits)
// void mu_bitvec_rotate_left(mu_bitvec_t *bv, int n_bits)
// set_range, clear_range, xor_range

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_BVEC_H_ */
