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

#include "mu_timer.h"
#include "mu_task.h"
#include "mu_time.h"
#include "mu_sched.h"

// =============================================================================
// private types and definitions

// =============================================================================
// private declarations

/**
 * Implementation note: since we anticipate that timers will be frequently used,
 * we want to minimize the size of the timer context.  We do this by providing
 * two functions: one_shot_fn and periodic_fn in order to eliminate a boolean in
 * the context.
 */
static void one_shot_fn(void *ctx, void *arg);
static void periodic_fn(void *ctx, void *arg);
static void trigger_aux(void *ctx, void *arg, bool repeat);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_timer_t *mu_timer_one_shot(mu_timer_t *timer, mu_task_t *target_task) {
  mu_task_init(&timer->timer_task, one_shot_fn, timer, "One Shot");
  timer->target_task = target_task;
  return timer;
}

mu_timer_t *mu_timer_periodic(mu_timer_t *timer, mu_task_t *target_task) {
  mu_task_init(&timer->timer_task, periodic_fn, timer, "Periodic");
  timer->target_task = target_task;
  return timer;
}

mu_timer_t *mu_timer_start(mu_timer_t *timer, mu_duration_t interval) {
  timer->interval = interval;
  mu_sched_task_in(&timer->timer_task, interval);
  return timer;
}

mu_timer_t *mu_timer_stop(mu_timer_t *timer) {
  mu_sched_remove_task(&timer->timer_task);
  return timer;
}

bool mu_timer_is_running(mu_timer_t *timer) {
  return mu_sched_get_task_status(&timer->timer_task) !=
         MU_SCHED_TASK_STATUS_IDLE;
}

// =============================================================================
// static (local) code

static void one_shot_fn(void *ctx, void *arg) {
  trigger_aux(ctx, arg, false);
}

static void periodic_fn(void *ctx, void *arg) {
  trigger_aux(ctx, arg, true);
}

static void trigger_aux(void *ctx, void *arg, bool repeat) {
  mu_timer_t *timer = (mu_timer_t *)ctx;
  (void)(arg);

  if (repeat) {
    mu_sched_reschedule_in(timer->interval);
  }
  mu_task_call(timer->target_task, timer);
}
