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

#ifndef _MU_STR_H_
#define _MU_STR_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Includes

#include "mu_strbuf.h"
#include <stdbool.h>
#include <stdint.h>

// =============================================================================
// Types and definitions

typedef struct {
  mu_strbuf_t *buf; // reference to underlying buffer
  size_t s;          // index of next byte to be read, or start of string
  size_t e;          // index of next byte to be written, or end of string
} mu_str_t;

// =============================================================================
// Declarations

/**
 * @brief Initialize a mu_str
 *
 * @param str The mu_str to be initialized
 * @param buf The underlying mu_str_buf
 * @return str
 */
mu_str_t *mu_str_init_for_read(mu_str_t *str, mu_strbuf_t *buf);

mu_str_t *mu_str_init_for_write(mu_str_t *str, mu_strbuf_t *buf);

/**
 * @brief Reset start and end pointers for a read buffer
 *
 * Note: s is set to 0, e is set to buf->capacity.
 *
 * @param str A mu_str
 * @return str
 */
mu_str_t *mu_str_read_reset(mu_str_t *str);

/**
* @brief Reset start and end pointers for a write buffer
*
* Note: s  and e are set to 0.
*
* @param str A mu_str
* @return str
 */
mu_str_t *mu_str_write_reset(mu_str_t *str);


/**
 * @brief Make a shallow copy of a mu_str
 *
 * After calling this, dst and src will refer to the same underlying string.
 *
 * @param dst The mu_str to be copied into.
 * @param src The mu_str to copy from
 * @return dst
 */
mu_str_t *mu_str_copy(mu_str_t *dst, const mu_str_t *src);

/**
 * @brief Make a slice of a mu_str
 *
 * mu_str_slice takes a substring of a mu_str.  A negative index mean "from
 * the end of src".  Assume src refers to the string "abcdefg":
 *
 *   start end  result     comment
 *       0   1  "a"
 *       0   7  "abcdefg"
 *       0   8  "abcdefg"  Out-of-range indeces are clamped
 *       0  -1  "abcdef"   Negative index counts from end
 *      -2  -1  "f"        Negative indexing works for start as well
 *       0   0  ""         Emptry string
 *       1   0  ""         Clamping is in effect here as well.
 *
 * Notes:
 * * The resulting dst is always equal to or shorter than the src
 * * mu_str_slice manipulates indecs; it does not move any data bytes.
 * * dst and src may pointe to the same object.
 *
 * @param dst The ref to be modified
 * @param src The ref from which to take a slice
 * @param start The starting index, relative to src start
 * @param end The ending index, relative to src start
 * @return dst
 */
mu_str_t *mu_str_slice(mu_str_t *dst, const mu_str_t *src, int start, int end);


/**
 * @brief Return the number of bytes available for reading.
 *
 * @param str A mu_str
 * @return The number of bytes available for reading, i.e. e-s
 */
size_t mu_str_read_available(const mu_str_t *str);

/**
 * @brief Return the number of bytes available for writing.
 *
 * @param str A mu_str
 * @return The number of bytes available for writing, i.e. capacity - e
 */
size_t mu_str_write_available(const mu_str_t *str);

/**
 * @brief Advance the start index of a mu_str
 *
 * This adds n_bytes to the start index of a mu_str.  It does not move any
 * data.
 *
 * @param str The mu_str to modify
 * @param n_bytes The amount by which to increment the start index. The start
 *        index is never moved beyond the end index.
 * @return The number of bytes by which the start index was incremented.
 */
size_t mu_str_read_increment(mu_str_t *str, size_t n_bytes);

/**
 * @brief Advance the end index of a mu_str
 *
 * This adds n_bytes to the end index of a mu_str.  It does not move any
 * data.
 *
 * @param str The mu_str to modify
 * @param n_bytes The amount by which to increment the end index. The end index
 *        is never moved beyond the capacity of the underlying string.
 * @return The number of bytes by which the end index was incremented.
 */
size_t mu_str_write_increment(mu_str_t *str, size_t n_bytes);

/**
 * @brief Return a pointer to the start of the underlyng string.
 *
 * @param str The mu_str
 * @return A pointer to the start of the underlying string
 */
const uint8_t *mu_str_read_ref(const mu_str_t *str);

/**
 * @brief Return a pointer to the end of the underlyng string.
 *
 * @param str The mu_str
 * @return A pointer to the end of the underlying string
 */
uint8_t *mu_str_write_ref(const mu_str_t *str);

/**
 * @brief Read one byte from the underlying string.  Return it by reference.
 *
 * @param str The mu_str
 * @param byte Pointer to the byte the receive the read byte.
 * @return True if the byte was read, false if there were no bytes available for
 *         reading.
 */
bool mu_str_read_byte(mu_str_t *str, uint8_t *byte);

/**
 * @brief Write one byte to the underlying string.
 *
 * @param str The mu_str
 * @param byte The byte to write.
 * @return True if the byte was written, false if the underlying string was
 *         already full.
 */
bool mu_str_write_byte(mu_str_t *str, uint8_t byte);

/**
 * @brief Append the contents of one mu_str to another.
 *
 * @param dst The mu_str to receive the bytes.
 * @param src The mu_str providing the bytes.
 * @return The number of bytes copied.
 */
size_t mu_str_append(mu_str_t *dst, const mu_str_t *src);

/**
 * @brief Append a null-terminated C-style string to a mu_str
 *
 * Note: the null byte itself is not written.
 *
 * @param dst The mu_str to receive the bytes.
 * @param cstr The null-terminated C-style string to provide the bytes.
 * @return The number of bytes copied.
 */
size_t mu_str_append_cstr(mu_str_t *dst, const char *cstr);

/**
 * @brief Printf to a mu_str.
 *
 * @param dst The mu_str to receive the bytes.
 * @param fmt The printf-style format string
 * @param ... Arguments to the printf
 * @return The number of bytes copied.
 */
size_t mu_str_printf(mu_str_t *dst, const char *fmt, ...);

/**
 * @brief Copy the contents of a mu_str to a C-style string
 *
 * Note: the resulting string is never longer than len, and is always null
 * terminated.
 *
 * @param src The mu_str from which to copy
 * @param cstr A pointer to the C-style string to receive the bytes
 * @param len The cacpacity of the C-string
 * @return The number of bytes copied, not including the null byte.
 */
size_t mu_str_to_cstr(const mu_str_t *src, char *cstr, size_t len);

// TBD:
// mu_str_cmp -- compare two strings
// mu_str_equals -- mu_str_cmp() == 0
// mu_str_eq ? -- do two mu_str object point to the same bytes?
// mu_str_find -- find first occurance of a substring within a mu_str

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_STR_H_ */
