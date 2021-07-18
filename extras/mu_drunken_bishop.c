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

#include "mu_drunken_bishop.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// Local types and definitions

/*
 * Field sizes for the random art have to be odd, so that the starting point
 * can be in the exact middle of the picture, and FLDBASE should be >=8 .
 * Else pictures would be too dense to be useful
 */
#define MAX_FLDBASE   40
#define MAX_FLDSIZE_X (MAX_FLDBASE * 2 + 1)
#define MAX_FLDSIZE_Y (MAX_FLDBASE + 1)

#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})


// =============================================================================
// Local storage
static char field[MAX_FLDSIZE_X][MAX_FLDSIZE_Y];


// =============================================================================
// Local (forward) declarations

// =============================================================================
// Public code


void print_random_art_from_string(char *seed_string, int column_width)
{
  // make sure both dimensions are odd and fit within our static storage
  int fldsize_x = min(MAX_FLDSIZE_X, ((column_width >> 1) * 2) + 1);
  int fldsize_y = (fldsize_x >> 1) + 1;

  //These characters represent the bishop's path, each self-crossing points farther into this string
  char  *worm_string = " .o+=*BOX@%&#/^SE";
  uint32_t  i, b;
  int  x, y;
  size_t   len = strlen(worm_string) - 1;
  int seed_string_length = strlen(seed_string);

  /* initialize field */
  memset(field, 0, fldsize_x * fldsize_y * sizeof(char));

  // start the bishop in the center
  x = fldsize_x / 2;
  y = fldsize_y / 2;

  for (i = 0; i < seed_string_length; i++) {
    int input;
    /* each byte conveys four 2-bit move commands */
    input = seed_string[i];
    for (b = 0; b < 4; b++) {
      /* evaluate 2 bit, rest is shifted later */
      x += (input & 0x1) ? 1 : -1;
      y += (input & 0x2) ? 1 : -1;

      /* assure we are still in bounds */
      x = max(x, 0);
      y = max(y, 0);
      x = min(x, fldsize_x - 1);
      y = min(y, fldsize_y - 1);

      /* augment the field  -- if we are alread at len -1, maybe we shold wrap around? */
      if (field[x][y] < len - 2)
        field[x][y]++;
      input = input >> 2;
    }
  }

  /* mark starting point and end point*/
  field[fldsize_x / 2][fldsize_y / 2] = len - 1;
  field[x][y] = len;

  /* upper border */
  printf("+");
  for (i = 0; i < fldsize_x; i++)
    fputc('-', stdout);
  printf("+\n");

  /* bishop's path */
  for (y = 0; y < fldsize_y; y++) {
    fputc('|', stdout); // left edge
    for (x = 0; x < fldsize_x; x++) {
      fputc(worm_string[min(field[x][y], len)], stdout);
    }
    printf("|\n"); // right edge
  }

  /* lower border */
  fputc('+', stdout);
  for (i = 0; i < fldsize_x; i++)
    fputc('-', stdout);
  printf("+\n");
}

// =============================================================================
// Private code

