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

static mu_ansi_term_color_t s_fg_color;
static mu_ansi_term_color_t s_bg_color;

#undef ANSI_TERM_COLOR
#define ANSI_TERM_COLOR(MU_ANSI_TERM__name, _fg, _bg) _fg,
static const uint8_t s_fg_colormap[] = { DEFINE_ANSI_TERM_COLORS };

#undef ANSI_TERM_COLOR
#define ANSI_TERM_COLOR(MU_ANSI_TERM__name, _fg, _bg) _bg,
static const uint8_t s_bg_colormap[] = { DEFINE_ANSI_TERM_COLORS };

// =============================================================================
// Local (forward) declarations

static char getint(uint8_t *val);
static void set_colors()
static uint8_t map_fg_color(mu_ansi_term_color_t color);
static uint8_t map_bg_color(mu_ansi_term_color_t color);

// =============================================================================
// Public code

void mu_ansi_term_init(void) {
  mu_ansi_term_set_foreground_color(MU_ANSI_TERM_WHITE);
  mu_ansi_term_set_background_color(MU_ANSI_TERM_BLACK);
}

/**
 * @brief Move cursor to 0, 0
 */
void mu_ansi_term_home(void) {
  puts(CSI "H");
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

/**
 * @brief Set Cursor Position
 *
 * Note: assumes row and col are 0 based, but converts to 1 based for ANSI spec.
 */
void mu_ansi_term_set_cursor_position(uint8_t row, uint8_t col) {
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

bool mu_ansi_term_get_cursor_position(uint8_t *row, uint8_t *col) {
  char ch;
  uint8_t temp_row;
  uint8_t temp_col;

  puts(CSI "6n");   // device status reports.  responds with ESC[<row>;<col>R
  ch = getchar();
  if (ch != '\e') {
    ungetc(ch);
    return false;
  }
  ch = getchar();
  if (ch != '[') {
    ungetc(ch);
    return false;
  }
  ch = getint(&temp_row);
  if (ch != ';') {
    ungetc(ch);
    return false;
  }
  ch = getint(&temp_col);
  if (ch != 'R') {
    ungetc(ch);
    return false;
  }

  *row = temp_row - 1;
  *col = temp_col - 1;
  return true;
}

/**
 * @brief Set foreground and background color
 */
void mu_ansi_term_set_colors(mu_ansi_term_color_t fg, mu_ansi_term_color_t bg) {
  s_fg_color = fg;
  s_bg_color = bg;
  printf(CSI "%d;%dm", map_fg_color(fg), map_bg_color(bg));
}

/**
 * @brief Get foreground and background color
 */
void mu_ansi_term_get_colors(mu_ansi_term_color_t *fg, mu_ansi_term_color_t *bg) {
  *fg = s_fg_color;
  *bg = s_bg_color;
}

// =============================================================================
// Local (static) code

static char getint(uint8_t *val) {
  char ch;
  *val = 0;
  while(1) {
    ch = getchar();
    if ((ch >= '0' && (ch <= '9'))) {
      *val = (*val * 10) + ch - '0';
    } else {
      break;
    }
  }
  return ch;  // return char that terminated the run of digits
}

static uint8_t map_fg_color(mu_ansi_term_color_t color) {
  return s_fg_colormap[color];
}

static uint8_t map_bg_color(mu_ansi_term_color_t color) {
  return s_bg_colormap[color];
}
