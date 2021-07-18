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


/**
 * Random number generator using a permuted congruential generator (PCG)
 * https://en.wikipedia.org/wiki/Permuted_congruential_generator
 */


// =============================================================================
// Includes

#include "mu_random.h"
#include <stdint.h>

// =============================================================================
// Local types and definitions
#define INITIAL_RANDOM_STATE (0x4d595df4d0f33173)

// =============================================================================
// Local (forward) declarations

static uint32_t rotr32(uint32_t x, unsigned r);

// =============================================================================
// Local storage
static uint64_t       state      = INITIAL_RANDOM_STATE;
static uint64_t const multiplier = 6364136223846793005u;
static uint64_t const increment  = 1442695040888963407u;


// =============================================================================
// Public code

uint32_t mu_random(void) {
  uint64_t x = state;
  unsigned count = (unsigned)(x >> 59);   // 59 = 64 - 5
  state = x * multiplier + increment;
  x ^= x >> 18;               // 18 = (64 - (32 - 5))/2
  return rotr32((uint32_t)(x >> 27), count);  // 27 = 32 - 5
}


uint32_t mu_random_range(uint32_t min, uint32_t max) {
  return min + (mu_random() % (max - min));
}

void mu_random_seed(uint32_t seed) {
  state = seed + increment;
  (void)mu_random();
}

void mu_random_reset() {
  state = INITIAL_RANDOM_STATE;
}


uint64_t hash_from_string(unsigned char *str)
{
    uint64_t hash = 5381;
    int c;
    while ((c = *str++) > 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

// =============================================================================
// Local (static) code

static uint32_t rotr32(uint32_t x, unsigned r)
{
  return x >> r | x << (-r & 31);
}



