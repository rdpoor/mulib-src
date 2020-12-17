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

#include "mu_list.h"
#include "mu_sched.h"
#include "mu_spscq.h"
#include "mu_task.h"
#include "mu_time.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h> // memmove

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

static void *default_idle_fn(void *self, void *arg);

static mu_sched_err_t
sched_task_at(mu_sched_t *sched, mu_task_t *task, mu_time_t time);

static void *sched_task_at_aux(mu_list_t *ref, void *arg);

// =============================================================================
// local storage

static mu_task_t s_default_idle_task;

// =============================================================================
// public code

mu_sched_t *mu_sched_init(mu_sched_t *sched, mu_spscq_t *isr_queue) {
  sched->isr_queue = isr_queue;
  sched->clock_fn = mu_time_now;
  sched->idle_task = &s_default_idle_task;
  mu_task_init(&s_default_idle_task, default_idle_fn, NULL, "Idle");

  return mu_sched_reset(sched);
}

mu_sched_t *mu_sched_reset(mu_sched_t *sched) {
  mu_spscq_reset(sched->isr_queue);
  sched->current_task = NULL;
  sched->task_list.next = NULL;
  return sched;
}

mu_sched_err_t mu_sched_step(mu_sched_t *sched) {
  mu_task_t *task;
  mu_time_t now = mu_sched_get_current_time(sched);

  // first, transfer any tasks in the isr queue into the main queue
  while (mu_spscq_get(sched->isr_queue, (void **)&task) !=
         MU_CQUEUE_ERR_EMPTY) {
    mu_sched_err_t err = mu_sched_task_at(sched, task, now);
    if (err != MU_SCHED_ERR_NONE) {
      return err;
    }
  }

  // process one task in the main queue
  task = mu_sched_get_next_task(sched);  // peek at next task.
  if (task != NULL) {
    if (!mu_time_follows(mu_task_get_time(task), now)) {
      // time to run the task: pop from queue, make current, run it...
      sched->current_task = MU_LIST_CONTAINER(
          mu_list_pop(&sched->task_list), mu_task_t, link);

      mu_task_call(sched->current_task, sched);
      // set current task to null to signify "not running task"
      sched->current_task = NULL;
      return MU_SCHED_ERR_NONE;
    }
  }

  // arrive here if there was nothing to run
  mu_task_call(sched->idle_task, sched);
  return MU_SCHED_ERR_NONE;
}

mu_list_t mu_sched_task_list(mu_sched_t *sched) { return sched->task_list; }

mu_spscq_t *mu_sched_isr_queue(mu_sched_t *sched) { return sched->isr_queue; }

mu_task_t *mu_sched_get_idle_task(mu_sched_t *sched) {
  return sched->idle_task;
}

mu_task_t *mu_sched_get_default_idle_task(mu_sched_t *sched) {
  return &s_default_idle_task;
}

mu_sched_t *mu_sched_set_idle_task(mu_sched_t *sched, mu_task_t *task) {
  sched->idle_task = task;
  return sched;
}

mu_clock_fn mu_sched_get_clock_source(mu_sched_t *sched) {
  return sched->clock_fn;
}

mu_sched_t *mu_sched_set_clock_source(mu_sched_t *sched, mu_clock_fn clock_fn) {
  sched->clock_fn = clock_fn;
  return sched;
}

mu_time_t mu_sched_get_current_time(mu_sched_t *sched) {
  return sched->clock_fn();
}

int mu_sched_task_count(mu_sched_t *sched) {
  return mu_list_length(&sched->task_list);
}

bool mu_sched_is_empty(mu_sched_t *sched) { return mu_list_is_empty(&sched->task_list); }

mu_task_t *mu_sched_get_current_task(mu_sched_t *sched) {
  return sched->current_task;
}

mu_task_t *mu_sched_get_next_task(mu_sched_t *sched) {
  if (sched->task_list.next == NULL) {
    return NULL;
  }
  return MU_LIST_CONTAINER(sched->task_list.next, mu_task_t, link);
}

mu_task_t *mu_sched_remove_task(mu_sched_t *sched, mu_task_t *task) {
  mu_list_t *link = mu_list_delete(&sched->task_list, MU_LIST_REF(task, link));
  if (link == NULL) {
    return NULL;
  }
  return MU_LIST_CONTAINER(link, mu_task_t, link);
}

mu_sched_err_t mu_sched_task_now(mu_sched_t *sched, mu_task_t *task) {
  mu_sched_remove_task(sched, task);
  return sched_task_at(sched, task, mu_sched_get_current_time(sched));
}

mu_sched_err_t
mu_sched_task_at(mu_sched_t *sched, mu_task_t *task, mu_time_t at) {
  mu_sched_remove_task(sched, task);
  return sched_task_at(sched, task, at);
}

mu_sched_err_t
mu_sched_task_in(mu_sched_t *sched, mu_task_t *task, mu_time_dt in) {
  mu_sched_remove_task(sched, task);
  return sched_task_at(
      sched, task, mu_time_offset(mu_sched_get_current_time(sched), in));
}

mu_sched_err_t mu_sched_reschedule_in(mu_sched_t *sched, mu_time_dt in) {
  mu_task_t *task = mu_sched_get_current_task(sched);
  if (!task) {
    return MU_SCHED_ERR_NOT_FOUND;
  }
  return sched_task_at(sched, task, mu_time_offset(mu_task_get_time(task), in));
}

mu_sched_err_t mu_sched_reschedule_now(mu_sched_t *sched) {
  mu_task_t *task = mu_sched_get_current_task(sched);
  if (!task) {
    return MU_SCHED_ERR_NOT_FOUND;
  }
  return mu_sched_task_now(sched, task);
}

mu_sched_err_t mu_sched_task_from_isr(mu_sched_t *sched, mu_task_t *task) {
  if (mu_spscq_put(sched->isr_queue, task) == MU_CQUEUE_ERR_FULL) {
    return MU_SCHED_ERR_FULL;
  } else {
    return MU_SCHED_ERR_NONE;
  }
}

mu_sched_task_status_t mu_sched_get_task_status(mu_sched_t *sched,
                                                mu_task_t *task) {
  if (mu_sched_get_current_task(sched) == task) {
    // task is the current task
    return MU_SCHED_TASK_STATUS_ACTIVE;
  }
  if (!mu_list_contains(&sched->task_list, MU_LIST_REF(task, link))) {
    // task is not in the schedule at all
    return MU_SCHED_TASK_STATUS_IDLE;
  }

  mu_time_t now = mu_sched_get_current_time(sched);
  if (!mu_time_follows(mu_task_get_time(task), now)) {
    // task's time has arrived, but it's not yet running
    return MU_SCHED_TASK_STATUS_RUNNABLE;

  } else {
    // task is scheduled for some pont in the future
    return MU_SCHED_TASK_STATUS_SCHEDULED;
  }
}

// =============================================================================
// local (static) code

static void *default_idle_fn(void *self, void *arg) {
  // the default idle task doesn't do much...
  return self;
}

static mu_sched_err_t
sched_task_at(mu_sched_t *sched, mu_task_t *task, mu_time_t time) {
  mu_task_set_time(task, time);
  mu_list_traverse(&sched->task_list, sched_task_at_aux, (void *)task);
  return MU_SCHED_ERR_NONE;
}

static void *sched_task_at_aux(mu_list_t *ref, void *arg) {
  mu_task_t *task = (mu_task_t *)arg;
  mu_time_t time1 = mu_task_get_time(task);

  // If at end of list, insert task here.
  if (ref->next == NULL) {
    return mu_list_push(ref, MU_LIST_REF(task, link));
  }
  // Check relative times of new task and incumbent task.
  mu_task_t *task2 = MU_LIST_CONTAINER(ref->next, mu_task_t, link);
  mu_time_t time2 = mu_task_get_time(task2);
  if (mu_time_precedes(time1, time2)) {
    // task happens sooner than task2: insert on list
    return mu_list_push(ref, MU_LIST_REF(task, link));
  }
  // keep searching ...
  return NULL;
}
