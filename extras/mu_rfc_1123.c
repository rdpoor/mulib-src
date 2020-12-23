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

// =============================================================================
// Includes

#include "mu_rfc_1123.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

// =============================================================================
// Local types and definitions

// =============================================================================
// Local (forward) declarations

// tokens is a denseely packed string of tokens, where each token is token_len
// characters long.  If s equals the Nth token, set dst to N (by reference)
// and return s incremented by token_len.  else return NULL
static const char *parse_tokens(const char *s, int *dst, const char *tokens, int token_len);

// parse N digits as a base-10 number and return it by reference in dst, then
// return s incremented by n_digits.  Return NULL if s does not contain digits.
static const char *parse_int(const char *s, int *dst, int n_digits);

// Skip over the given literal, return s incremented by strlen(literal), else
// return NULL if s does not match literal.
static const char *skip_literal(const char *s, const char *literal);

// =============================================================================
// Local storage

static const char *const s_months = "JanFebMarAprMayJunJulAugSepOctNovDec";
static const char *const s_days = "SunMonTueWedThuFriSat";

// =============================================================================
// Public code

const char *mu_rfc_1123_str_to_tm(const char *s, struct tm *tm) {
  memset(tm, 0, sizeof(struct tm));
  if (!(s = parse_tokens(s, &tm->tm_wday, s_days, 3))) return NULL;
  if (!(s = skip_literal(s, ", "))) return NULL;
  if (!(s = parse_int(s, &tm->tm_mday, 2))) return NULL;
  if (!(s = skip_literal(s, " "))) return NULL;
  if (!(s = parse_tokens(s, &tm->tm_mon, s_months, 3))) return NULL;
  if (!(s = skip_literal(s, " "))) return NULL;
  if (!(s = parse_int(s, &tm->tm_year, 4))) return NULL;
  // want year - 1900
  tm->tm_year -= 1900;
  if (!(s = skip_literal(s, " "))) return NULL;
  if (!(s = parse_int(s, &tm->tm_hour, 2))) return NULL;
  if (!(s = skip_literal(s, ":"))) return NULL;
  if (!(s = parse_int(s, &tm->tm_min, 2))) return NULL;
  if (!(s = skip_literal(s, ":"))) return NULL;
  if (!(s = parse_int(s, &tm->tm_sec, 2))) return NULL;
  if (!(s = skip_literal(s, " GMT"))) return NULL;
  return s;
}

char *mu_rfc_1123_tm_to_str(const struct tm *tm, char *s, int maxlen) {
  snprintf(s,
           maxlen,
           "%.3s, %02d %.3s %04d %02d:%02d:%02d GMT",
           &s_days[tm->tm_wday * 3],
           tm->tm_mday,
           &s_months[tm->tm_mon * 3],
           tm->tm_year + 1900,
           tm->tm_hour,
           tm->tm_min,
           tm->tm_sec);
  return s;
}

// =============================================================================
// Local (static) code

static const char *parse_tokens(const char *s, int *dst, const char *tokens, int token_len) {
  size_t tokens_length = strlen(tokens);
  for (size_t i = 0; i < tokens_length; i += token_len) {
    if (strncmp(s, &tokens[i], token_len) == 0) {
      *dst = i / token_len;
      return s + token_len;
    }
  }
  // ran off end without a match
  return NULL;
}

static const char *parse_int(const char *s, int *dst, int n_digits) {
  *dst = 0;

  for (int i = 0; i < n_digits; i++) {
    char ch = s[i];
    if ((ch >= '0') && (ch <= '9')) {
      *dst = (*dst * 10) + (ch - '0');
    } else {
      return NULL;
    }
  }
  return s + n_digits;
}

static const char *skip_literal(const char *s, const char *literal) {
  int literal_len = strlen(literal);
  if (strncmp(s, literal, literal_len) == 0) {
    return s + literal_len;
  } else {
    return NULL;
  }
}

// =============================================================================
// cc -g -Wall -Wextra -Werror -DTEST_MU_RFC_1123 -o test_mu_rfc_1123 mu_rfc_1123.c
//   ./test_mu_rfc_1123
// or
//   gdb test_mu_rfc_1123
// then
//   rm ./test_mu_rfc_1123

#ifdef TEST_MU_RFC_1123

#include <assert.h>
#define ASSERT assert

int main(void) {
  struct tm tm1;
  const char *s;
  char s1[MU_RFC_1123_MAX_LEN];
  const char *s2 = "Tue, 18 Jun 2019 16:06:21 GMT";

  ASSERT((s = mu_rfc_1123_str_to_tm(s2, &tm1)) != NULL);
  ASSERT(s == s2 + strlen(s2));
  ASSERT(tm1.tm_sec == 21);
  ASSERT(tm1.tm_min == 6);
  ASSERT(tm1.tm_hour == 16);
  ASSERT(tm1.tm_mday == 18);
  ASSERT(tm1.tm_mon == 5);
  ASSERT(tm1.tm_year == 2019);
  ASSERT(tm1.tm_wday == 2);

  ASSERT(mu_rfc_1123_str_to_tm("Tux, 18 Jun 2019 16:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tux, 1x Jun 2019 16:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tux, 18 Jux 2019 16:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tux, 18 Jun 201x 16:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tux, 18 Jun 2019 1x:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tux, 18 Jun 2019 16:0x:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tux, 18 Jun 2019 16:06:2x GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tux, 18 Jun 2019 16:06:21 GMx", &tm1) == NULL);

  ASSERT((s = mu_rfc_1123_tm_to_str(&tm1, s1, MU_RFC_1123_MAX_LEN)) != NULL);
  ASSERT(strcmp(s, s1) == 0);
}

#endif
