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

#ifndef _MU_TASK_H_
#define _MU_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include "mu_list.h"
#include "mu_thunk.h"
#include "mu_time.h"

// =============================================================================
// types and definitions

/**
 * A `mu_task` is a mu_thunk (deferrable function) with a time and a link field
 * ddded, primarily for the benefit of the scheduler.
 */

typedef struct _mu_task {
  mu_list_t link;   // next (older) event in the schedule
  mu_time_t time;   // time at which this task fires
  mu_thunk_t thunk; // function to be scheduled
#if (MU_TASK_PROFILING)
  const char *name;        // user defined task name
  unsigned int call_count; // # of times task is called
  mu_time_dt runtime;      // accumulated time spent running the task
  mu_time_dt max_duration; // max time spend running the task
#endif
} mu_task_t;

mu_task_t *mu_task_init(mu_task_t *task, mu_thunk_fn fn, void *ctx, const char *name);

mu_list_t mu_task_link(mu_task_t *task);

mu_time_t mu_task_get_time(mu_task_t *task);

void mu_task_set_time(mu_task_t *task, mu_time_t time);

mu_thunk_fn mu_task_get_fn(mu_task_t *task);

void *mu_task_get_context(mu_task_t *task);

const char *mu_task_name(mu_task_t *task);

void *mu_task_call(mu_task_t *task, void *arg);

#if (MU_TASK_PROFILING)

unsigned int mu_task_call_count(mu_task_t *task);

mu_time_ms_dt mu_task_runtime_ms(mu_task_t *task);

mu_time_ms_dt mu_task_max_duration_ms(mu_task_t *task);

#ifdef MU_FLOAT
mu_time_s_dt mu_task_runtime_s(mu_task_t *task);

mu_time_s_dt mu_task_max_duration_s(mu_task_t *task);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif // #ifndef _MU_TASK_H_
