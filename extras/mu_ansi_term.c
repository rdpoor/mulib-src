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
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
// =============================================================================
// Local types and definitions

#define CSI "\e["

// =============================================================================
// Local storage

static mu_ansi_term_color_t s_fg_color;
static mu_ansi_term_color_t s_bg_color;

struct termios saved_attributes;
static bool _has_saved_attributes = false;

static int mu_ansi_cols = 80;
static int mu_ansi_rows = 24;

#undef ANSI_TERM_COLOR
#define ANSI_TERM_COLOR(MU_ANSI_TERM__name, _fg, _bg) _fg,
static const uint8_t s_fg_colormap[] = { DEFINE_ANSI_TERM_COLORS };

#undef ANSI_TERM_COLOR
#define ANSI_TERM_COLOR(MU_ANSI_TERM__name, _fg, _bg) _bg,
static const uint8_t s_bg_colormap[] = { DEFINE_ANSI_TERM_COLORS };

static key_poll_ctx_t mu_ansi_term_key_poll_ctx; // if mu_begin_polling_for_keypress() is called, key_poll_ctx.key_char will contain most recent user key press

// =============================================================================
// Local (forward) declarations

static char getint(uint8_t *val);
static uint8_t map_fg_color(mu_ansi_term_color_t color);
static uint8_t map_bg_color(mu_ansi_term_color_t color);
static void poll_keypress_fn(void *ctx, void *arg);


typedef struct {
      uint8_t count;
      char bytes[];
    } mu_flag_store;

// #define mu_flag_store_intializer_of_size(n) {.count = n,  .bytes = char b[16]}
// #define mu_flag_store_intializer_of_size(n) {.count = n,  .bytes = [0 ... 16] = 0}
// #define mu_flag_store_intializer_of_size(n) {.count = n,  .bytes = [0 ... ((n >> 3) + 1)] = 0}
#define mu_flag_store_intializer_of_size(n) {.count = n }


// =============================================================================
// Public code
// void got_sigwinch() {
//   printf("Got it\n");
// }

void mu_ansi_term_init(void) {
  // static mu_flag_store chosen_flags = mu_flag_store_intializer_of_size(100);
  // printf("store %d\n",chosen_flags.count);

  mu_ansi_term_set_colors(MU_ANSI_TERM_DEFAULT_COLOR, MU_ANSI_TERM_DEFAULT_COLOR);
  mu_ansi_term_get_terminal_attributes(&saved_attributes); // so we can restore later
  _has_saved_attributes = true;


// #ifdef TIOCGSIZE
//     struct ttysize ts;
//     ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
//     mu_ansi_cols = ts.ts_cols;
//     mu_ansi_rows = ts.ts_lines;
// #elif defined(TIOCGWINSZ)
//     struct winsize ts;
//     ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
//     mu_ansi_cols = ts.ws_col;
//     mu_ansi_rows = ts.ws_row;
// #endif /* TIOCGSIZE */
//     printf("Terminal is %dx%d\n", mu_ansi_cols, mu_ansi_rows);
//     signal(SIGWINCH, got_sigwinch);


}


void mu_ansi_term_terminal_bell() {
  printf("\a"); // ansi terminal bell / flash
}

void mu_ansi_term_reset() {
  printf( "%s%s", MU_ANSI_TERM_ESC, MU_ANSI_TERM_RESET); // undo any color settings
}

void mu_ansi_term_set_cursor_visible(bool isVisible) {
  printf( "%s%s", MU_ANSI_TERM_ESC, (isVisible ? MU_ANSI_SHOW_CURSOR : MU_ANSI_HIDE_CURSOR)); 
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
    ungetc(ch, 0);
    return false;
  }
  ch = getchar();
  if (ch != '[') {
    ungetc(ch, 0);
    return false;
  }
  ch = getint(&temp_row);
  if (ch != ';') {
    ungetc(ch, 0);
    return false;
  }
  ch = getint(&temp_col);
  if (ch != 'R') {
    ungetc(ch, 0);
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

/**
 * @brief Assuming our stdin terminal is in non-canonical mode (set via mu_set_terminal_attributes)
 * this function checks stdin for the next user-entered character, but won't hang if there isn't one.
 * If wait_for_newlines was turned off then there will also be no waiting for newline
 * 
 */

int mu_ansi_term_get_key_press(void) {
    int ch;
    ch = getchar();
    if (ch < 0) {
        if (ferror(stdin)) { /* there was an error... */ }
        clearerr(stdin);
        /* there was no keypress */
        return 0;
    }
    if(ch == 3) exit(0); // ctrl-c special case
    return ch;
}

/**
 * @brief Change attributes on the terminal.
*/

void mu_ansi_term_set_terminal_attributes(struct termios *terminal_attributes) {
    tcsetattr(STDIN_FILENO, TCSANOW, terminal_attributes);
}
void mu_ansi_term_get_terminal_attributes(struct termios *terminal_attributes) {
      tcgetattr(STDIN_FILENO, terminal_attributes);      
}


int mu_ansi_term_ncols() {
  return mu_ansi_cols;
}
int mu_ansi_term_nrows() {
  return mu_ansi_rows;
}


void mu_ansi_term_enter_noncanonical_mode() {
  struct termios info;
  mu_ansi_term_get_terminal_attributes(&saved_attributes); // so we can restore later
  _has_saved_attributes = true;
 /* get current terminal attirbutes */
  bool canonical = false, echo_input = false, wait_for_newlines = false; // these could be passed in...
  tcgetattr(STDIN_FILENO, &info);          
  info.c_lflag &= ((ICANON && canonical) | (ECHO && echo_input)); // TODO make sure we're toggling these bits correctly
  info.c_cc[VMIN] = wait_for_newlines ? 1 : 0;
  info.c_cc[VTIME] = 0;         /* no timeout */
  tcsetattr(STDIN_FILENO, TCSANOW, &info);
}

void mu_ansi_term_exit_noncanonical_mode() {
  if(_has_saved_attributes)
    mu_ansi_term_set_terminal_attributes(&saved_attributes);
  printf( "%s%s\n", MU_ANSI_TERM_ESC, MU_ANSI_TERM_RESET); // undo any color settings
  mu_ansi_term_set_cursor_visible(true);
}

void mu_begin_polling_for_keypress() {
  mu_ansi_term_key_poll_ctx.key_char = 0;
  mu_task_init(&mu_ansi_term_key_poll_ctx.task, poll_keypress_fn, &mu_ansi_term_key_poll_ctx, "key_poll");
  mu_ansi_term_enter_noncanonical_mode();
  mu_sched_task_now(&mu_ansi_term_key_poll_ctx.task);
  atexit(mu_ansi_term_exit_noncanonical_mode); // restores terminal attributes
}

static void poll_keypress_fn(void *ctx, void *arg) {
  //key_poll_ctx_t *self = (key_poll_ctx_t *)ctx;
  (void)arg;  // unused
  int ch = mu_ansi_term_get_key_press();
  if(ch) {
    mu_ansi_term_key_poll_ctx.key_char = ch;
  }
  mu_duration_t delay = MU_TIME_MS_TO_DURATION(KEY_POLL_INTERVAL_MS);
  mu_sched_task_in(&mu_ansi_term_key_poll_ctx.task, delay);
}

unsigned char mu_term_get_current_keypress() {
  unsigned char k = mu_ansi_term_key_poll_ctx.key_char;
  mu_ansi_term_key_poll_ctx.key_char = 0;
  return k;
}



