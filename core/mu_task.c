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

#include "mulib.h"
#include "mu_task.h"
#include "mu_thunk.h"
#include <stddef.h>

// =============================================================================
// private types and definitions

// =============================================================================
// private declarations

// =============================================================================
// local storage

// =============================================================================
// public code

mu_task_t *mu_task_init(mu_task_t *task,
                        mu_thunk_fn fn,
                        void *ctx,
                        const char *name) {
  task->link.next = NULL;
  task->time = 0;
  mu_thunk_init(&task->thunk, fn, ctx);
#if (MU_TASK_PROFILING)
  task->name = name;
  task->call_count = 0;
  task->runtime = 0;
  task->max_duration = 0;
#endif
  return task;
}

mu_list_t mu_task_link(mu_task_t *task) {
  return task->link;
}

mu_time_t mu_task_get_time(mu_task_t *task) {
  return task->time;
}

void mu_task_set_time(mu_task_t *task, mu_time_t time) {
  task->time = time;
}

mu_thunk_fn mu_task_get_fn(mu_task_t *task) {
  return mu_thunk_get_fn(&task->thunk);
}

void *mu_task_get_context(mu_task_t *task) {
  return mu_thunk_get_ctx(&task->thunk);
}

const char *mu_task_name(mu_task_t *task) {
#if (MU_TASK_PROFILING)
  return task->name;
#else
  return "";
#endif
}

void *mu_task_call(mu_task_t *task, void *arg) {

#if (MU_TASK_PROFILING)
  mu_time_t called_at = mu_time_now();
#endif
  void *result = mu_thunk_call(&task->thunk, arg);
#if (MU_TASK_PROFILING)
  task->call_count += 1;
  mu_time_dt duration = mu_time_difference(mu_time_now(), called_at);
  task->runtime += duration;
  if (duration > task->max_duration) task->max_duration = duration;
#endif
  return result;
}

#if (MU_TASK_PROFILING)

unsigned int mu_task_call_count(mu_task_t *task) {
  return task->call_count;
}

mu_time_ms_dt mu_task_runtime_ms(mu_task_t *task) {
  return mu_time_duration_to_ms(task->runtime);
}

mu_time_ms_dt mu_task_max_duration_ms(mu_task_t *task) {
  return mu_time_duration_to_ms(task->max_duration);
}

#ifdef MU_FLOAT
mu_time_s_dt mu_task_runtime_s(mu_task_t *task) {
  return mu_time_duration_to_s(task->runtime);
}

mu_time_s_dt mu_task_max_duration_s(mu_task_t *task) {
  return mu_time_duration_to_s(task->max_duration);
}
#endif

#endif

// =============================================================================
// private functions
