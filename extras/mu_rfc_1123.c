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
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

// =============================================================================
// Local types and definitions

#define TM_YEAR_OFFSET 1900

// Most ports define a tm struct using all ints:
// struct tm {
//     int tm_sec;    /* Seconds (0-60) */
//     int tm_min;    /* Minutes (0-59) */
//     int tm_hour;   /* Hours (0-23) */
//     int tm_mday;   /* Day of the month (1-31) */
//     int tm_mon;    /* Month (0-11) */
//     int tm_year;   /* Year - 1900 */
//     int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
//     int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
//     int tm_isdst;  /* Daylight saving time */
// };
//
// However, the AVR128U1U (and likely others) define the tm struct using
// 8- and 16- bit fields:
//
// struct tm {
//     int8_t          tm_sec; /**< seconds after the minute - [ 0 to 59 ] */
//     int8_t          tm_min; /**< minutes after the hour - [ 0 to 59 ] */
//     int8_t          tm_hour; /**< hours since midnight - [ 0 to 23 ] */
//     int8_t          tm_mday; /**< day of the month - [ 1 to 31 ] */
//     int8_t          tm_wday; /**< days since Sunday - [ 0 to 6 ] */
//     int8_t          tm_mon; /**< months since January - [ 0 to 11 ] */
//     int16_t         tm_year; /**< years since 1900 */
//     int16_t         tm_yday; /**< days since January 1 - [ 0 to 365 ] */
//     int16_t         tm_isdst; /**< Daylight Saving Time flag */
// };
//
// These defines make it possible to assign values directly into the struct:
//
#ifndef __AVR_ATxmega128A1U__
#define TM_FIELD_SMALL int
#define TM_FIELD_LARGE int
#else
#define TM_FIELD_SMALL int8_t
#define TM_FIELD_LARGE int16_t
#endif

// =============================================================================
// Local (forward) declarations

// tokens is a denseely packed string of tokens, where each token is token_len
// characters long.  If s equals the Nth token, set dst to N (by reference)
// and return s incremented by token_len.  else return NULL
static const char *parse_tokens(const char *s, TM_FIELD_SMALL *dst, const char *tokens, int token_len);

// parse N digits as a base-10 number and return it by reference in dst, then
// return s incremented by n_digits.  Return NULL if s does not contain digits.
static const char *parse_int_small(const char *s, TM_FIELD_SMALL *dst, int n_digits);

// parse N digits as a base-10 number and return it by reference in dst, then
// return s incremented by n_digits.  Return NULL if s does not contain digits.
static const char *parse_int_large(const char *s, TM_FIELD_LARGE *dst, int n_digits);

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
  if (!(s = parse_int_small(s, &tm->tm_mday, 2))) return NULL;
  if (!(s = skip_literal(s, " "))) return NULL;
  if (!(s = parse_tokens(s, &tm->tm_mon, s_months, 3))) return NULL;
  if (!(s = skip_literal(s, " "))) return NULL;
  if (!(s = parse_int_large(s, &tm->tm_year, 4))) return NULL;
  // want year - 1900
  tm->tm_year -= TM_YEAR_OFFSET;
  if (!(s = skip_literal(s, " "))) return NULL;
  if (!(s = parse_int_small(s, &tm->tm_hour, 2))) return NULL;
  if (!(s = skip_literal(s, ":"))) return NULL;
  if (!(s = parse_int_small(s, &tm->tm_min, 2))) return NULL;
  if (!(s = skip_literal(s, ":"))) return NULL;
  if (!(s = parse_int_small(s, &tm->tm_sec, 2))) return NULL;
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
           tm->tm_year + TM_YEAR_OFFSET,
           tm->tm_hour,
           tm->tm_min,
           tm->tm_sec);
  return s;
}

// =============================================================================
// Local (static) code

static const char *parse_tokens(const char *s, TM_FIELD_SMALL *dst, const char *tokens, int token_len) {
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

static const char *parse_int_small(const char *s, TM_FIELD_SMALL *dst, int n_digits) {
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

static const char *parse_int_large(const char *s, TM_FIELD_LARGE *dst, int n_digits) {
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

  printf("mu_rfc_1123 unit test starting...\n");
  ASSERT((s = mu_rfc_1123_str_to_tm(s2, &tm1)) != NULL);
  ASSERT(s == s2 + strlen(s2));
  ASSERT(tm1.tm_sec == 21);
  ASSERT(tm1.tm_min == 6);
  ASSERT(tm1.tm_hour == 16);
  ASSERT(tm1.tm_mday == 18);
  ASSERT(tm1.tm_mon == 5);
  ASSERT(tm1.tm_year + TM_YEAR_OFFSET == 2019);
  ASSERT(tm1.tm_wday == 2);

  ASSERT(mu_rfc_1123_str_to_tm("Tux, 18 Jun 2019 16:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tue, 1x Jun 2019 16:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tue, 18 Jux 2019 16:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tue, 18 Jun 201x 16:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tue, 18 Jun 2019 1x:06:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tue, 18 Jun 2019 16:0x:21 GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tue, 18 Jun 2019 16:06:2x GMT", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tue, 18 Jun 2019 16:06:21 GMx", &tm1) == NULL);
  ASSERT(mu_rfc_1123_str_to_tm("Tue, 18 Jun 2019 16x06:21 GMT", &tm1) == NULL);

  ASSERT((s = mu_rfc_1123_str_to_tm(s2, &tm1)) != NULL); // reset tm1
  ASSERT((s = mu_rfc_1123_tm_to_str(&tm1, s1, MU_RFC_1123_MAX_LEN)) != NULL);
  ASSERT(strcmp(s, s2) == 0);
  printf("... mu_rfc_1123 unit test complete.\n");
}

#endif
