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

// =============================================================================
// Includes

#include "mu_ansi_term.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// =============================================================================
// Local types and definitions

#define CSI "\e["

// =============================================================================
// Local storage

// =============================================================================
// Local (forward) declarations

// =============================================================================
// Public code

/**
 * @brief Move cursor to 0, 0
 */
void mu_ansi_term_home(void) {
  puts(CSI "H");
}

/**
 * @brief Cursor Position
 *
 * Note: assumes row and col are 0 based, but converts to 1 based for ANSI spec.
 */
void mu_ansi_term_cursor_position(uint8_t row, uint8_t col) {
  // optimize.
  if (row == 0) {
    if (col == 0) {
      puts(CSI "H");
    } else {
      printf(CSI ";%dH", col+1);
    }
  } else {
    if (col == 0) {
      printf(CSI "%dH", row+1);
    } else {
      printf(CSI "%d;%dH", row+1, col+1);
    }
  }
}

/**
 * @brief Erase screen and scrollback
 */
void mu_ansi_term_clear_buffer(void) {
  puts(CSI "3J");
}

/**
 * @brief Erase screen
 */
void mu_ansi_term_clear_screen(void) {
  puts(CSI "2J");
}

/**
 * @brief Erase screen from current cursor position
 */
void mu_ansi_term_clear_to_end_of_screen(void) {
  puts(CSI "J");
}

/**
 * @brief Erase current line
 */
void mu_ansi_term_clear_line(void) {
  puts(CSI "2K");
}

/**
 * @brief Erase line from current cursor position
 */
void mu_ansi_term_clear_to_end_of_line(void) {
  puts(CSI "K");
}

// =============================================================================
// Local (static) code
