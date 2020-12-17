/**
 * MIT License
 *
 * Copyright (c) 2020 R. Dunbar Poor <rdpoor@gmail.com>
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
// includes

#include "mu_strbuf.h"
#include "mu_substr.h"
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

// #include <stdio.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

static size_t min_size(size_t a, size_t b);
static bool index_is_valid(mu_substr_t *s, size_t index);
static mu_strbuf_data_t *ref_offset(mu_substr_t *s, size_t offset);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_substr_t *mu_substr_init(mu_substr_t *s, mu_strbuf_t *str) {
  s->str = str;
  return mu_substr_reset(s);
}

mu_substr_t *mu_substr_duplicate(mu_substr_t *dst, mu_substr_t *src) {
  if (src != dst) {
    memcpy(dst, src, sizeof(mu_substr_t));
  }
  return dst;
}

mu_substr_t *mu_substr_reset(mu_substr_t *s) {
  s->start = 0;
  s->end = mu_strbuf_capacity(s->str);
  return s;
}

mu_substr_t *mu_substr_clear(mu_substr_t *s) {
  s->start = 0;
  s->end = 0;
  return s;
}

mu_strbuf_t *mu_substr_str(mu_substr_t *s) {
  return s->str;
}

size_t mu_substr_start(mu_substr_t *s) {
  return s->start;
}

size_t mu_substr_end(mu_substr_t *s) {
  return s->end;
}

size_t mu_substr_length(mu_substr_t *s) {
  return s->end - s->start;
}

size_t mu_substr_remaining(mu_substr_t *s) {
  return mu_strbuf_capacity(s->str) - s->end;
}

mu_substr_err_t mu_substr_ref(mu_substr_t *s, size_t index, mu_strbuf_data_t **p) {
  if (index_is_valid(s, index)) {
    *p = ref_offset(s, index);
    return MU_SUBSTR_ERR_NONE;
  } else {
    *p = NULL;
    return MU_SUBSTR_ERR_INDEX;
  }
}

mu_substr_err_t mu_substr_get(mu_substr_t *s, size_t index, mu_strbuf_data_t *d) {
  if (index_is_valid(s, index)) {
    *d = *ref_offset(s, index);
    return MU_SUBSTR_ERR_NONE;
  } else {
    *d = '\0';
    return MU_SUBSTR_ERR_INDEX;
  }
}

mu_substr_err_t mu_substr_put(mu_substr_t *s, size_t index, mu_strbuf_data_t d) {
  if (index_is_valid(s, index)) {
    *ref_offset(s, index) = d;
    return MU_SUBSTR_ERR_NONE;
  } else {
    return MU_SUBSTR_ERR_INDEX;
  }
}

mu_substr_err_t mu_substr_slice_str(mu_substr_t *s, int start, int end) {
  size_t capacity = mu_strbuf_capacity(mu_substr_str(s));
  int s1, e1;

  if (start >= 0) {
    s1 = start;
  } else {
    s1 = capacity + start;
  }
  if (end >= 0) {
    e1 = end;
  } else {
    e1 = capacity + end;
  }

  if ((s1 < 0) || (s1 > capacity)) {
    return MU_SUBSTR_ERR_INDEX;
  } else if ((e1 < 0) || (e1 > capacity)) {
    return MU_SUBSTR_ERR_INDEX;
  } else if (e1 < s1) {
    return MU_SUBSTR_ERR_INDEX;
  }
  s->start = s1;
  s->end = e1;
  return MU_SUBSTR_ERR_NONE;
}

mu_substr_t *mu_substr_copy(mu_substr_t *dst, mu_substr_t *src) {
  size_t dst_len = mu_substr_length(dst);
  size_t src_len = mu_substr_length(src);
  size_t to_copy = min_size(dst_len, src_len);
  if (to_copy > 0) {
    memmove(ref_offset(dst, 0), ref_offset(src, 0), to_copy);
  }
  dst->end = dst->start + to_copy;
  return dst;
}

int mu_substr_cmp(mu_substr_t *s1, mu_substr_t *s2) {
  size_t len1 = mu_substr_length(s1);
  size_t len2 = mu_substr_length(s2);
  mu_strbuf_data_t *p1 = ref_offset(s1, 0);
  mu_strbuf_data_t *p2 = ref_offset(s2, 0);
  mu_strbuf_data_t c1, c2;

  c1 = *p1;
  c2 = *p2;
  // loop structure could be better...
  while (len1 > 0 && len2 > 0) {
    c1 = *p1++;
    c2 = *p2++;
    if (c1 != c2) {
      return c1 - c2;
    }
    len1--;
    len2--;
  };

  // ran off the end of one or the other...
  if (len1 == 0 && len2 == 0) {
    return 0;
  } else if (len1 == 0) {
    return -c2;
  } else {
    return c1;
  }
}

bool mu_substr_equals(mu_substr_t *s1, mu_substr_t *s2) {
  return mu_substr_cmp(s1, s2) == 0;
}

mu_substr_t *mu_substr_append(mu_substr_t *dst, mu_substr_t *src) {
  size_t to_copy = min_size(mu_substr_length(src), mu_substr_remaining(dst));
  mu_strbuf_data_t *sp = ref_offset(src, 0);
  mu_strbuf_data_t *dp = ref_offset(dst, dst->end);
  memcpy(dp, sp, to_copy);
  dst->end += to_copy;
  return dst;
}

// Search for a needle in a haystack.
//
// Implementation: dst is initialize to a substring having the same lemgth as
// needle, but starting at index = 0.  At each iteration dst->start and dst->end
// is incremented by one until needle matches it or it runs into the end of
// haystack.
mu_substr_t *mu_substr_strstr(mu_substr_t *dst, mu_substr_t *needle, mu_substr_t *haystack) {
  size_t needle_len = mu_substr_length(needle);
  mu_substr_duplicate(dst, haystack);

  if (needle_len > mu_substr_length(haystack)) {
    return mu_substr_clear(dst);
  }

  if (mu_substr_slice_str(dst, 0, needle_len) != MU_SUBSTR_ERR_NONE) {
    return mu_substr_clear(dst);
  }

  while(true) {
    if (mu_substr_cmp(dst, needle) == 0) {
      // dst matches needle -- return it
      return dst;
    }
    // advance dst by 1 character
    if (mu_substr_slice_str(dst,
                            mu_substr_start(dst) + 1,
                            mu_substr_end(dst) + 1) != MU_SUBSTR_ERR_NONE) {
      // ran into end of haystack -- cannot match
      return mu_substr_clear(dst);
    }
  }
}

mu_substr_t *mu_substr_to_cstr(mu_substr_t *s, char *cstr, size_t cstr_length) {
  // always leave one byte for null terminator
  size_t to_copy = min_size(mu_substr_length(s), cstr_length-1);
  memmove(cstr, ref_offset(s, 0), to_copy);
  cstr[to_copy] = '\0';  // null terminate
  return s;
}


mu_substr_t *mu_substr_printf(mu_substr_t *s, const char *fmt, ...) {
  size_t available = mu_substr_remaining(s);

  if (available > 0) {
    size_t printed;
    va_list ap;
    va_start(ap, fmt);
    printed = vsnprintf(&s->str->data[s->end], mu_substr_remaining(s), fmt, ap);
    va_end(ap);
    // Note: count is the number of bytes that *would* have been printed if
    // there was enough room, NOT the number of bytes actually printed.
    if (printed > available) {
      printed = available;
    }
    s->end += printed;
  }
  return s;
}

mu_substr_t *mu_substr_puts(mu_substr_t *s, const char *src) {
  size_t available = mu_substr_remaining(s);

  if (available > 0) {
    size_t copied = strlen(src);  // does not include null terminator
    if (copied > available) {
      copied = available;
    }
    memcpy(&s->str->data[s->end], src, copied);
    s->end += copied;
  }
  return s;
}

// =============================================================================
// local (static) code

static size_t min_size(size_t a, size_t b) {
  return (a < b) ? a : b;
}

static bool index_is_valid(mu_substr_t *s, size_t index) {
  size_t abs_i = index + s->start;
  return (s->start <= abs_i) && (abs_i < s->end);
}

static mu_strbuf_data_t *ref_offset(mu_substr_t *s, size_t offset) {
  // Violate mu_strbuf's data abstraction in the name of efficiency...
  return &s->str->data[s->start + offset];
}
