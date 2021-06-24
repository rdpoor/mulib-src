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
 * @file mu_ansi_term.h
 *
 * Support for the more common ANSI terminal escape sequences.
 */

#ifndef _MU_ANSI_TERM_H_
#define _MU_ANSI_TERM_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdint.h>

// =============================================================================
// types and definitions

// =============================================================================
// declarations

/**
 * @brief Move cursor to 0, 0
 */
void mu_ansi_term_home(void);

/**
 * @brief Cursor Position
 */
void mu_ansi_term_cursor_position(uint8_t row, uint8_t col);

/**
 * @brief Erase screen and scrollback
 */
void mu_ansi_term_clear_buffer(void);

/**
 * @brief Erase screen
 */
void mu_ansi_term_clear_screen(void);

/**
 * @brief Erase screen from current cursor position
 */
void mu_ansi_term_clear_to_end_of_screen(void);

/**
 * @brief Erase current line
 */
void mu_ansi_term_clear_line(void);

/**
 * @brief Erase line from current cursor position
 */
void mu_ansi_term_clear_to_end_of_line(void);



#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_ANSI_TERM_H_ */
