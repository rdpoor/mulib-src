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

#include "mu_log.h"
#include "mu_vect.h"

#ifdef MU_LOG_ENABLED // rest of file...

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

// =============================================================================
// private types and definitions

typedef struct {
  mu_log_function_t fn;
  mu_log_level_t threshold;
} subscriber_t;

// =============================================================================
// private declarations

/**
 * @brief Return non-null value if a->fn == b->fn.  Assumes that both a and b
 * are non-NULL and of type subscriber_t *.
 */
static void *function_matches(void *a, void *b);

/**
 * @brief Call subscriber's function with a logging message.
 *
 * If the severity equals or exceeds the subscriber's threshold, call the
 * subscriber's function with s_message.
 *
 * @param subscriber Pointer to a subscriber object.
 * @param arg Pointer to the severity of the current log message.
 * @return Returns NULL in order for mu_vect_traverse to visit all elements.
 */
static void *broadcast(void *subscriber, void *arg);

// =============================================================================
// local storage

static mu_vect_t s_subscribers;
static subscriber_t s_subscribers_store[MU_LOG_MAX_SUBSCRIBERS];

static char s_message[MU_LOG_MAX_MESSAGE_LENGTH];

#undef DEFINE_MU_LOG_LEVEL
#define DEFINE_MU_LOG_LEVEL(level, name) name,
const char * const s_level_names[] = {
  EXPAND_MU_LOG_LEVELS
};


// =============================================================================
// user-visible code

void mu_log_init() {
  mu_vect_init(&s_subscribers, s_subscribers_store, MU_LOG_MAX_SUBSCRIBERS, sizeof(subscriber_t));
}

// install or update a subscriber
mu_log_err_t mu_log_subscribe(mu_log_function_t fn, mu_log_level_t threshold) {
  subscriber_t s = {.fn = fn, .threshold = threshold};
  // remove any existing subscription, then add a new subscription at the end
  // of the list.
  mu_log_unsubscribe(fn);
  if (mu_vect_push(&s_subscribers, &s) == MU_VECT_ERR_FULL) {
    return MU_LOG_ERR_SUBSCRIBERS_EXCEEDED;
  } else {
    return MU_LOG_ERR_NONE;
  }
}

mu_log_err_t mu_log_unsubscribe(mu_log_function_t fn) {
  subscriber_t s = {.fn = fn};
  int index = mu_vect_find_index(&s_subscribers, function_matches, &s);
  // No need to check for negative index: mu_vect_delete_at does that.
  mu_vect_delete_at(&s_subscribers, index, NULL);
  return MU_LOG_ERR_NONE;
}

const char *mu_log_level_name(mu_log_level_t severity) {
  if (severity >= sizeof(s_level_names)/sizeof(const char *)) {
    return "UNKNOWN";
  } else {
    return s_level_names[severity];
  }
}

void mu_log_message(mu_log_level_t severity, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(s_message, MU_LOG_MAX_MESSAGE_LENGTH, fmt, ap);
  va_end(ap);

  mu_vect_traverse(&s_subscribers, broadcast, &severity);
}

// =============================================================================
// private code

static void *function_matches(void *a, void *b) {
  subscriber_t *as = (subscriber_t *)a;
  subscriber_t *bs = (subscriber_t *)b;
  return as->fn == bs->fn ? a : NULL;
}

static void *broadcast(void *subscriber, void *arg) {
  subscriber_t *s = (subscriber_t *)subscriber;
  mu_log_level_t *severity = (mu_log_level_t *)arg;

  if (*severity >= s->threshold) {
    s->fn(*severity, s_message);
  }
  return NULL;
}

#endif // #ifdef MU_LOG_ENABLED
