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

// =============================================================================
// Includes

#include "mu_random.h"
#include <stdint.h>

// =============================================================================
// Local types and definitions

#define RAND_A ((uint32_t)1103515245)
#define RAND_C ((uint32_t)12345)

// =============================================================================
// Local (forward) declarations

// =============================================================================
// Local storage

static uint32_t s_random_seed = ((uint32_t)123456789);

// =============================================================================
// Public code

uint32_t mu_random(void) {
  s_random_seed = (uint32_t)((RAND_A * s_random_seed + RAND_C) & 0x7fffffff);
  return s_random_seed;
}

uint32_t mu_random_range(uint32_t min, uint32_t max) {
  return min + (mu_random() % (max - min));
}

void mu_random_seed(uint32_t seed) {
  s_random_seed = seed;
}

// =============================================================================
// Local (static) code
