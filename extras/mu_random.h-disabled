/**
 * MIT License
 *
 * Copyright (c) 2021 Klatu Networks, Inc
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

#ifndef _MU_RANDOM_H_
#define _MU_RANDOM_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Includes

#include <stdint.h>

// =============================================================================
// Types and definitions

// =============================================================================
// Declarations

/**
 * @brief Return a pseudo-random integer between 0 and 2^31 - 1
 *
 * @return A pseudo random integer.
 */
uint32_t mu_random(void);

/**
 * @brief Return a random integer between min (inclusive) and max (exclusive).
 *
 * @param min The minimum (inclusive) value to return.
 * @param max The maximum (exclusive) value to return.
 * @return A pseudo random integer between min (inclusive) and max (exclusive).
 */
uint32_t mu_random_range(uint32_t min, uint32_t max);

/**
 * @brief Set the pseudo random number generator seed.
 *
 * @param seed A 32-bit integer to use as the seed.
 */
void mu_random_seed(uint32_t seed);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_RANDOM_H_ */
