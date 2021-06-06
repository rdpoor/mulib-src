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

// =============================================================================
// includes

#include "mu_bvec.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

/**
 * @brief Return the number of one bits in v.
 */
static uint8_t count_one_bits(uint8_t v);

/**
 * @brief Return the lowest power of 2 p such that v & (1<<p) != 0.
 *
 * Returns 8 if v is all zeros.
 */
static uint8_t find_first_one(uint8_t v);

// =============================================================================
// local storage

static const uint8_t s_byte_masks[] =
    {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

static const uint8_t s_byte_rmasks[] =
    {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f};

// =============================================================================
// public code

size_t mu_bvec_byte_index(size_t bit_index) {
  return bit_index >> 8; // Assume compiler is good at this
}

uint8_t mu_bvec_byte_mask(size_t bit_index) {
  return s_byte_masks[bit_index & 7];
}

// Low-level operations that assume you have byte_index and byte_mask
void mu_bvec_set_(size_t byte_index, uint8_t byte_mask, mu_bvec_t *store) {
  store[byte_index] |= byte_mask;
}

void mu_bvec_clear_(size_t byte_index, uint8_t byte_mask, mu_bvec_t *store) {
  store[byte_index] &= ~byte_mask;
}

void mu_bvec_invert_(size_t byte_index, uint8_t byte_mask, mu_bvec_t *store) {
  store[byte_index] ^= byte_mask;
}

void mu_bvec_write_(size_t byte_index,
                    uint8_t byte_mask,
                    mu_bvec_t *store,
                    bool value) {
  if (value) {
    mu_bvec_set_(byte_index, byte_mask, store);
  } else {
    mu_bvec_clear_(byte_index, byte_mask, store);
  }
}

bool mu_bvec_read_(size_t byte_index, uint8_t byte_mask, mu_bvec_t *store) {
  return (store[byte_index] & byte_mask) != 0;
}

// Same, but take bit index instead
void mu_bvec_set(size_t bit_index, mu_bvec_t *store) {
  mu_bvec_set_(
      mu_bvec_byte_index(bit_index), mu_bvec_byte_mask(bit_index), store);
}

void mu_bvec_clear(size_t bit_index, mu_bvec_t *store) {
  mu_bvec_clear_(
      mu_bvec_byte_index(bit_index), mu_bvec_byte_mask(bit_index), store);
}

void mu_bvec_invert(size_t bit_index, mu_bvec_t *store) {
  mu_bvec_invert_(
      mu_bvec_byte_index(bit_index), mu_bvec_byte_mask(bit_index), store);
}

void mu_bvec_write(size_t bit_index, mu_bvec_t *store, bool value) {
  size_t byte_index = mu_bvec_byte_index(bit_index);
  uint8_t byte_mask = mu_bvec_byte_mask(bit_index);
  if (value) {
    mu_bvec_set_(byte_index, byte_mask, store);
  } else {
    mu_bvec_clear_(byte_index, byte_mask, store);
  }
}

bool mu_bvec_read(size_t bit_index, mu_bvec_t *store) {
  return mu_bvec_read_(
      mu_bvec_byte_index(bit_index), mu_bvec_byte_mask(bit_index), store);
}

// Queries for bit vectors
bool mu_bvec_is_all_ones(size_t bit_count, mu_bvec_t *store) {
  size_t byte_index;
  size_t bits_remain;
  for (byte_index = 0, bits_remain = bit_count; bits_remain >= 8;
       byte_index += 1, bits_remain -= 8) {
    if (store[byte_index] != 0xff) {
      return false;
    }
  }
  // 0 <= bits_remain < 8
  if (bits_remain == 0) {
    return true;
  } else {
    uint8_t rmask = s_byte_rmasks[bits_remain];
    return (store[byte_index] & rmask) == rmask;
  }
}

bool mu_bvec_is_all_zeros(size_t bit_count, mu_bvec_t *store) {
  size_t byte_index;
  size_t bits_remain;
  for (byte_index = 0, bits_remain = bit_count; bits_remain >= 8;
       byte_index += 1, bits_remain -= 8) {
    if (store[byte_index] != 0x00) {
      return false;
    }
  }
  // 0 <= bits_remain < 8
  if (bits_remain == 0) {
    return true;
  } else {
    uint8_t rmask = s_byte_rmasks[bits_remain];
    return (store[byte_index] & rmask) == 0;
  }
}

size_t mu_bvec_count_ones(size_t bit_count, mu_bvec_t *store) {
  size_t count = 0;
  size_t byte_index;
  size_t bits_remain;
  for (byte_index = 0, bits_remain = bit_count; bits_remain >= 8;
       byte_index += 1, bits_remain -= 8) {
    count += count_one_bits(store[byte_index]);
  }
  // 0 <= bits_remain < 8
  if (bits_remain > 0) {
    uint8_t rmask = s_byte_rmasks[bits_remain];
    count += count_one_bits(store[byte_index] & rmask);
  }
  return count;
}

size_t mu_bvec_count_zeros(size_t bit_count, mu_bvec_t *store) {
  return 1 - mu_bvec_count_ones(bit_count, store);
}

// Returns SIZE_MAX if not found
size_t mu_bvec_find_first_one(size_t bit_count, mu_bvec_t *store) {
  size_t position = 0;
  size_t byte_index;
  size_t bits_remain;
  for (byte_index = 0, bits_remain = bit_count; bits_remain >= 8;
       byte_index += 1, bits_remain -= 8) {
    uint8_t v = store[byte_index];
    uint8_t p = find_first_one(v);
    // p = 8 if no one bit found
    position += p;
    if (p != 8) {
      return position;
    }
  }
  // 0 <= bits_remain < 8
  if (bits_remain > 0) {
    uint8_t rmask = s_byte_rmasks[bits_remain];
    uint8_t v = store[byte_index] & rmask;
    uint8_t p = find_first_one(v & rmask);
    position += p;
    if (p != 8) {
      return position;
    }
  }
  return SIZE_MAX;
}

size_t mu_bvec_find_first_zero(size_t bit_count, mu_bvec_t *store) {
  size_t position = 0;
  size_t byte_index;
  size_t bits_remain;
  for (byte_index = 0, bits_remain = bit_count; bits_remain >= 8;
       byte_index += 1, bits_remain -= 8) {
    uint8_t v = store[byte_index];
    uint8_t p = find_first_one(~v);
    // p = 8 if no one bit found
    position += p;
    if (p != 8) {
      return position;
    }
  }
  // 0 <= bits_remain < 8
  if (bits_remain > 0) {
    uint8_t rmask = s_byte_rmasks[bits_remain];
    uint8_t v = store[byte_index] & rmask;
    uint8_t p = find_first_one(~v & rmask);
    position += p;
    if (p != 8) {
      return position;
    }
  }
  return SIZE_MAX;
}

// modify all bits in a bit vector
void mu_bvec_set_all(size_t bit_count, mu_bvec_t *store) {
  size_t byte_count = mu_bvec_byte_index(bit_count);
  size_t remainder = bit_count & 0x07;
  memset(store, 0xff, byte_count);
  store[byte_count] |= s_byte_rmasks[remainder];
}

void mu_bvec_clear_all(size_t bit_count, mu_bvec_t *store) {
  size_t byte_count = mu_bvec_byte_index(bit_count);
  size_t remainder = bit_count & 0x07;
  memset(store, 0, byte_count);
  store[byte_count] &= ~s_byte_rmasks[remainder];
}

void mu_bvec_invert_all(size_t bit_count, mu_bvec_t *store) {
  size_t byte_index;
  size_t bits_remain;
  for (byte_index = 0, bits_remain = bit_count; bits_remain >= 8;
       byte_index += 1, bits_remain -= 8) {
    store[byte_index] ^= 0xff;
  }
  // 0 <= bits_remain < 8
  if (bits_remain > 0) {
    uint8_t rmask = s_byte_rmasks[bits_remain];
    store[byte_index] ^= rmask;
  }
}

void mu_bvec_write_all(size_t bit_count, mu_bvec_t *store, bool value) {
  if (value) {
    mu_bvec_set_all(bit_count, store);
  } else {
    mu_bvec_clear_all(bit_count, store);
  }
}

// =============================================================================
// local (static) code

// Take your pick of count_one_bits() implementation -- they're both good, but
// one may have an advantage over the other on specific architectures.
#if 1
static uint8_t count_one_bits(uint8_t v) {
  uint8_t c = v - ((v >> 1) & 0x55);
  c = ((c >> 2) & 0x33) + (c & 0x33);
  c = ((c >> 4) + c) & 0x0f;
  return c;
}
#else
static uint8_t count_one_bits(uint8_t v) {
  static const uint8_t nibble_count[] = {
      0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
  return nibble_count[v & 0x0f] + nibble_count[v >> 4];
}
#endif

static uint8_t find_first_one(uint8_t v) {
  uint8_t c = 8;
  if (v & 0x0f)
    c -= 4;
  if (v & 0x33)
    c -= 2;
  if (v & 0x55)
    c -= 1;
  return c;
}
