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

// =============================================================================
// private types and definitions

// =============================================================================
// private declarations

static void *timer_fn(void *ctx, void *arg);

// =============================================================================
// local storage

// =============================================================================
// public code

mu_timer_t *mu_timer_init(mu_timer_t *timer, mu_task_t *target_task) {
  mu_task_init(&timer->timer_task, timer_fn, timer, "TimerTask");
  timer->target_task = target_task;
  timer->is_running = false;
  return timer;
}

mu_timer_t *
mu_timer_start(mu_timer_t *timer, mu_time_dt interval, bool repeat) {
  mu_timer_stop(timer);
  timer->interval = interval;
  timer->does_repeat = repeat;
  timer->is_running = true;
  mu_sched_task_in(&timer->timer_task, interval);
  return timer;
}

mu_timer_t *mu_timer_stop(mu_timer_t *timer) {
  if (timer->is_running) {
    mu_sched_remove_task(&timer->timer_task);
  }
  timer->is_running = false;
  return timer;
}

bool mu_timer_is_running(mu_timer_t *timer) { return timer->is_running; }

// =============================================================================
// static (local) code

static void *timer_fn(void *ctx, void *arg) {
  (void)(arg);
  mu_timer_t *timer = (mu_timer_t *)ctx;
  if (!timer->is_running) {
    // timer was previously stopped: don't trigger target_task
    return NULL;
  } else if (timer->does_repeat) {
    // repeat is enabled: schedule next time
    mu_sched_reschedule_in(timer->interval);
  } else {
    // repeat is disabled: stop now
    timer->is_running = false;
  }
  // trigger the target task.  By convention, sched is user argument.
  return mu_task_call(timer->target_task, NULL);
}
