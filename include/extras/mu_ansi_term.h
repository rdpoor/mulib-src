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

#include <stdbool.h>
#include <stdint.h>


// =============================================================================
// types and definitions

#define DEFINE_ANSI_TERM_COLORS                                                \
  ANSI_TERM_COLOR(MU_ANSI_TERM_BLACK, 30, 40)                                  \
  ANSI_TERM_COLOR(MU_ANSI_TERM_RED, 31, 41)                                    \
  ANSI_TERM_COLOR(MU_ANSI_TERM_GREEN, 32, 42)                                  \
  ANSI_TERM_COLOR(MU_ANSI_TERM_YELLOW, 33, 43)                                 \
  ANSI_TERM_COLOR(MU_ANSI_TERM_BLUE, 34, 44)                                   \
  ANSI_TERM_COLOR(MU_ANSI_TERM_MAGENTA, 35, 45)                                \
  ANSI_TERM_COLOR(MU_ANSI_TERM_CYAN, 36, 46)                                   \
  ANSI_TERM_COLOR(MU_ANSI_TERM_WHITE, 37, 47)                                  \
  ANSI_TERM_COLOR(MU_ANSI_TERM_GRAY, 90, 100)                                  \
  ANSI_TERM_COLOR(MU_ANSI_TERM_BRIGHT_RED, 91, 101)                            \
  ANSI_TERM_COLOR(MU_ANSI_TERM_BRIGHT_GREEN, 92, 102)                          \
  ANSI_TERM_COLOR(MU_ANSI_TERM_BRIGHT_YELLOW, 93, 103)                         \
  ANSI_TERM_COLOR(MU_ANSI_TERM_BRIGHT_BLUE, 94, 104)                           \
  ANSI_TERM_COLOR(MU_ANSI_TERM_BRIGHT_MAGENTA, 95, 105)                        \
  ANSI_TERM_COLOR(MU_ANSI_TERM_BRIGHT_CYAN, 96, 106)                           \
  ANSI_TERM_COLOR(MU_ANSI_TERM_DEFAULT_COLOR, 39, 49)                          \
  ANSI_TERM_COLOR(MU_ANSI_TERM_BRIGHT_WHITE, 97, 107)

#undef ANSI_TERM_COLOR
#define ANSI_TERM_COLOR(_name, _foreground, _background) _name,
typedef enum {
  DEFINE_ANSI_TERM_COLORS
} mu_ansi_term_color_t;

#define MU_ANSI_TERM_ESC "\33["
#define MU_ANSI_TERM_RESET "0m"
#define MU_ANSI_SHOW_CURSOR "?25h"
#define MU_ANSI_HIDE_CURSOR "?25l"


// =============================================================================
// declarations

/**
 * @brief Initialize the ansi terminal module.
 */
void mu_ansi_term_init(void);

void mu_ansi_term_terminal_bell(void);
/**
 * @brief Move cursor to 0, 0
 */
void mu_ansi_term_home(void);

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

/**
 * @brief Set Cursor Position
 */
void mu_ansi_term_set_cursor_position(uint8_t row, uint8_t col);

/**
 * @brief Get Cursor Position
 *
 * Return true if the returned string was the correct format.
 */
bool mu_ansi_term_get_cursor_position(uint8_t *row, uint8_t *col);

/**
 * @brief Set foreground and background color
 */
void mu_ansi_term_set_colors(mu_ansi_term_color_t fg, mu_ansi_term_color_t bg);



/**
 * @brief Get foreground and background color
 */
void mu_ansi_term_get_colors(mu_ansi_term_color_t *fg, mu_ansi_term_color_t *bg);

/**
 * @brief Standard ansi term reset
 */
void mu_ansi_term_reset();

/**
 * @brief Set cursor visibility
 */
void mu_ansi_term_set_cursor_visible(bool isVisible);

/**
 * @brief Standard ansi reset and make cursor visible
*/
void mu_ansi_term_restore_colors_and_cursor();



#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_ANSI_TERM_H_ */
