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

#ifndef _PARSE_RFC_1123_H_
#define _PARSE_RFC_1123_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Includes

#include <time.h>  // for struct tm

// =============================================================================
// Types and definitions

#define MU_RFC_1123_MAX_LEN 30  // includes null terminator

// =============================================================================
// Declarations

/**
 * @brief Parse a date in RFC 1123 format and store the restults in a struct tm.
 *
 * Note: an RFC 1123 date has the following form:
 *
 *    Tue, 18 Jun 2019 16:06:21 GMT
 *
 * This parser is strict:
 * - Exactly one space appears between tokens
 * - Day and Month fields are Capitalized, GMT is UPPER CASE.
 *
 * On success, returns a pointer to the first character following "GMT" and the
 * struct tm is filled in.  On any error, returns NULL and struct tm is cleared.
 *
 * @param s Pointer to string to be parsed
 * @param tm time structure to be filled in
 * @return Pointer to character following "GMT" on success, NULL otherwise.
 */
const char *mu_rfc_1123_str_to_tm(const char *s, struct tm *tm);

/**
 * @brief Print the date and time in RFC 1123 format.
 *
 * Print the contents of a struct tm as an RFC 1123 formatted string, e.g.:
 *
 *    Tue, 18 Jun 2019 16:06:21 GMT
 *
 * @param tm Pointer to a struct tm containing the date and time.
 * @param s String buffer to receive the results
 * @param maxlen Capacity of the string buffer.
 * @return Pointer to the filled-in string buffer.
 */
char *mu_rfc_1123_tm_to_str(const struct tm *tm, char *s, int maxlen);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _PARSE_RFC_1123_H_ */
