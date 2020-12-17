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

#ifndef _MU_STR_SLICE_H_
#define _MU_STR_SLICE_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Includes

#include <stddef.h>
#include <stdbool.h>

// =============================================================================
// Types and definitions

typedef struct {
  union {
    mu_str_rbuf_t *rbuf;
    mu_str_wbuf_t *wbuf;
  };
  size_t get_i;
  size_t put_i;
} mu_str_ref_t;

// =============================================================================
// Declarations

/**
 * @brief Initialize a ref from a readonly buffer.
 *
 * Note: The ref's get_i is set to 0, put_i is set to the end of the readonly
 * buffer.
 *
 * @param ref The ref to be initialized.
 * @param rbuf The readonly buffer object.
 * @return ref
 */
mu_str_ref_t *mu_str_ref_init_for_read(mu_str_ref_t *ref, mu_str_rbuf_t *rbuf);

/**
 * @brief Initialize a ref from a writeable buffer.
 *
 * Note: The ref's get_i and put_i are set to 0.
 *
 * @param ref The ref to be initialized.
 * @param wbuf The writeable buffer object.
 * @return ref
 */
mu_str_ref_t *mu_str_ref_init_for_write(mu_str_ref_t *ref, mu_str_wbuf_t *wbuf);

/**
 * @brief Reset a readonly buffer reference.
 *
 * Note: get_i is set to 0, put_i is set to the readonly buffer's capacity.
 *
 * @param ref The readonly reference.
 * @return ref
 */
mu_str_ref_t *mu_str_ref_read_reset(mu_str_ref_t *ref);

/**
 * @brief Reset a writeable buffer reference.
 *
 * Note: get_i and put_i are set to 0.
 *
 * @param ref The readonly reference.
 * @return ref
 */
mu_str_ref_t *mu_str_ref_write_reset(mu_str_ref_t *ref);

/**
 * @brief Initialize one ref from another.
 *
 * @param dst The ref to be copied into.
 * @param src The ref to be copied from.
 * @return dst
 */
mu_str_ref_t *mu_str_ref_copy(mu_str_ref_t *dst, const mu_str_ref_t const *src);

/**
 * @brief Take a slice of an existing ref.
 *
 * Note that the resulting dst ref is always equal to or shorter than the src
 * ref.  A negative index mean "from the end of src".  Note also this does not
 * actually move any bytes.
 *
 * Assume src refers to the string "abcdefg"
 * ```
 *   start end  result     comment
 *       0   1  "a"
 *       0   7  "abcdefg"
 *       0   8  "abcdefg"  Out-of-range indeces are clamped
 *       0  -1  "abcdef"   Negative index counts from end
 *      -2  -1  "f"        Negative indexing works for start as well
 *       0   0  ""         Emptry string
 *       1   0  ""         Clamping works here as well
 *
 * @param dst The ref to be modified
 * @param src The ref from which to take a slice
 * @param start The starting index, relative to src start
 * @param end The ending index, relative to src start
 * @return dst
 */
 mu_str_ref_t *mu_str_ref_slice(mu_str_ref_t *dst,
                                const mu_str_ref_t const *src,
                                int start,
                                int end);

/**
 * @brief Get the number of bytes that can be read from a ref.
 *
 * Note: This returns ref->put_i - ref->get_i, in other words, the length of
 * the ref.
 *
 * @param ref A ref object.
 * @return The number of bytes that are available for reading.
 */
size_t mu_str_ref_readabale_count(mu_str_ref_t *ref);

/**
 * @brief Get the number of bytes that can be written.
 *
 * Note: This returns wbuf->capacity ref->ut_i, in other words, the number of
 * bytes that can be written to the underlying writeable buffer.
 *
 * @param ref A ref object.
 * @return The number of bytes that are available for writing.
 */
size_t mu_str_ref_writeable_count(mu_str_ref_t *ref);

/**
 * @brief Read one byte from a readonly buffer.
 *
 * Note: If ref->get_i < ref->put_i, this reads one byte and stores it by
 * reference in byte, increments ref->get_i and returns true.  Otherwise it
 * returns false.
 *
 * @param ref A ref object.
 * @param byte A pointer to a byte in which to receive the read byte.
 * @return True on success, false on failure.
 */
bool mu_str_ref_read_byte(mu_str_ref_t *ref, uint8_t *byte);

/**
 * @brief Write one byte to a writeable buffer.
 *
 * Note: If mu_str_ref_writeable_count() > 0, this writes one byte into the
 * underlying writeable buffer, increments ref->put_i and returns true.
 * Otherwise it returns false.
 *
 * @param ref A ref object.
 * @param byte The byte to be written.
 * @return True on success, false on failure.
 */
bool mu_str_ref_write_byte(mu_str_ref_t *ref, uint8_t byte);

/**
 * @brief Append the contents of one ref to another.
 *
 * Note: This copies as many bytes from src as will fit in dst, returning the
 * actual number of bytes copied.  src is unchanged, dst->put_i is modified.
 *
 * @param dst Reference to a writeable buffer
 * @param src Reference to a readable buffer.
 * @return The number of bytes copied.
 */
size_t mu_str_ref_append(mu_str_ref_t *dst, const mu_str_ref const *src);

/**
 * @brief Print into a buffer.
 *
 * Note: this function performs printf() as you would expect, but will not
 * overrrun the underling writeable buffer.
 *
 * @param dst Reference to the underlying writable bufffer.
 * @param fmt the format string
 * @param ... Arguments to the format string
 * @return The number of bytes writtn to the underlying buffer.
 */
size_t mu_str_ref_printf(mu_str_ref_t *dst, const char *fmt, ...);

/**
 * @brief Copy a mu_str_ref to a C style null-terminated strings.
 *
 * Note the resulting C string is _always_ null terminated, even if it means
 * not copying all of the bytes from the source ref.
 *
 * @param src The ref to copy data from
 * @param buf The c string buffer to copy into
 * @laram buflen The length of the C string buffer.
 * @return The actual number of bytes copied, NOT including null termination.
 */
size_t mu_str_ref_to_cstr(const mu_str_ref_t const *src,
                            uint8_t *buf, size_t buflen);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_STR_RBUF_H_ */
