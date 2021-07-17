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
 * Field sizes for the random art.  Have to be odd, so the starting point
 * can be in the exact middle of the picture, and FLDBASE should be >=8 .
 * Else pictures would be too dense, and drawing the frame would
 * fail, too, because the key type would not fit in anymore.
 */
#define FLDBASE   8
#define FLDSIZE_Y (FLDBASE + 1)
#define FLDSIZE_X (FLDBASE * 2 + 1)

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

// =============================================================================
// Local (forward) declarations

// =============================================================================
// Public code


void print_randomart(char *aString)
{
  /*
   * Chars to be used after each other every time the worm
   * intersects with itself.  Matter of taste.
   */
  char  *augmentation_string = " .o+=*BOX@%&#/^SE";
  char   field[FLDSIZE_X][FLDSIZE_Y];
  uint32_t  i, b;
  int  x, y;
  size_t   len = strlen(augmentation_string) - 1;
  int aString_length = strlen(aString);

  /* initialize field */
  memset(field, 0, FLDSIZE_X * FLDSIZE_Y * sizeof(char));
  x = FLDSIZE_X / 2;
  y = FLDSIZE_Y / 2;

  /* process raw key */
  for (i = 0; i < aString_length; i++) {
    int input;
    /* each byte conveys four 2-bit move commands */
    input = aString[i];
    for (b = 0; b < 4; b++) {
      /* evaluate 2 bit, rest is shifted later */
      x += (input & 0x1) ? 1 : -1;
      y += (input & 0x2) ? 1 : -1;

      /* assure we are still in bounds */
      x = max(x, 0);
      y = max(y, 0);
      x = min(x, FLDSIZE_X - 1);
      y = min(y, FLDSIZE_Y - 1);

      /* augment the field  -- if we are alread at len -1, maybe we shold wrap around? */
      if (field[x][y] < len - 2)
        field[x][y]++;
      input = input >> 2;
    }
  }

  /* mark starting point and end point*/
  field[FLDSIZE_X / 2][FLDSIZE_Y / 2] = len - 1;
  field[x][y] = len;

  /* output upper border */
  for (i = 0; i < FLDSIZE_X; i++)
    fputc('-', stdout);
  printf("+\n");

  /* output content */
  for (y = 0; y < FLDSIZE_Y; y++) {
    fputc('|', stdout);
    for (x = 0; x < FLDSIZE_X; x++) {
      fputc(augmentation_string[min(field[x][y], len)], stdout);
    }
    printf("|\n");
  }

  /* output lower border */
  fputc('+', stdout);
  for (i = 0; i < FLDSIZE_X; i++)
    fputc('-', stdout);
  printf("+\n");
}

// =============================================================================
// Private code

