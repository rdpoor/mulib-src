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

#ifndef _MU_TIMER_H_
#define _MU_TIMER_H_

#ifdef __cplusplus
extern "C";
#endif

// =============================================================================
// includes

#include "mu_config.h"
#include "mu_task.h"
#include <stdbool.h>

// =============================================================================
// types and definitions

typedef struct {
  mu_task_t timer_task;
  mu_task_t *target_task;
  mu_time_dt interval;
  bool does_repeat;
  bool is_running;
} mu_timer_t;

// =============================================================================
// declarations

mu_timer_t *mu_timer_init(mu_timer_t *timer, mu_task_t *target_task);

mu_timer_t *mu_timer_start(mu_timer_t *timer, mu_time_dt interval, bool repeat);

mu_timer_t *mu_timer_stop(mu_timer_t *timer);

bool mu_timer_is_running(mu_timer_t *timer);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _MU_TIMER_H_
