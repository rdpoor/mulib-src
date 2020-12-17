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
#include <stddef.h>
#include <stdio.h>

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

static void *periodic_task_fn(void *a1, void *a2);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_task_t *mu_periodic_task_init(mu_task_t *periodic_task,
                                 mu_periodic_task_ctx *ctx,
                                 mu_task_t *triggered_task,
                                 mu_sched_t *scheduler,
                                 mu_vm_time_ms_dt interval_ms,
                                 const char *task_name) {
  ctx->triggered_task = triggered_task;
  ctx->sched = scheduler;
  ctx->interval_ms = interval_ms;
  mu_task_init(periodic_task, periodic_task_fn, ctx, task_name);
  return periodic_task;
}

void mu_periodic_task_start(mu_task_t *periodic_task) {
  mu_periodic_task_ctx *ctx = (mu_periodic_task_ctx *)periodic_task->ctx;
  mu_sched_t *sched = ctx->sched;

  mu_periodic_task_stop(periodic_task);  // make sure it's not already in sched
  mu_sched_task_now(sched, periodic_task);
}

void mu_periodic_task_stop(mu_task_t *periodic_task) {
  mu_periodic_task_ctx *ctx = (mu_periodic_task_ctx *)periodic_task->ctx;
  mu_sched_t *sched = ctx->sched;

  mu_sched_remove_task(sched, periodic_task);
}

// =============================================================================
// local (static) code

/**
 * @brief Periodically run the target task.
 */
static void *periodic_task_fn(void *a1, void *a2) {
  // periodic_task context is passed as a1, scheduler as a2
  mu_periodic_task_ctx *ctx = (mu_periodic_task_ctx *)a1;

  // trigger the given task...
  mu_sched_task_now(ctx->sched, ctx->triggered_task);

  // ...and repeat after the given interval
  mu_sched_reschedule_in(ctx->sched, mu_time_ms_to_duration(ctx->interval_ms));

  return NULL;
}
