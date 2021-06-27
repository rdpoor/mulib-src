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

#include "mu_sched.h"

#include "mu_time.h"
#include "mu_rtc.h"
#include "mu_spsc.h"
#include "mu_task.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h> // memmove

// =============================================================================
// local types and definitions

typedef struct {
  mu_dlist_t task_list;     // time ordered list of tasks (soonest first)
  mu_clock_fn clock_fn;     // function to call to get the current time
  mu_task_t *idle_task;     // the idle task
  mu_task_t *current_task;  // the task currently being processed
  mu_spsc_t irq_task_queue; // Tasks queued at interrupt level
  mu_spsc_item_t irq_task_queue_store[MU_IRQ_TASK_QUEUE_SIZE];
} mu_sched_t;

// =============================================================================
// local (forward) declarations

static void default_idle_fn(void *self, void *arg);

static mu_task_t *peek_next_task(void);

static mu_task_t *get_runnable_task(mu_time_t now);

static mu_sched_err_t queue_task(mu_task_t *task);

static mu_sched_err_t queue_isr_task(mu_task_t *task);

static mu_dlist_t *find_insertion_point(mu_dlist_t *head, mu_time_t time);

// =============================================================================
// local storage

// Singleton instance of the scheduler
static mu_sched_t s_sched;

static mu_task_t s_default_idle_task;

// =============================================================================
// public code

void mu_sched_init() {
  s_sched.clock_fn = mu_rtc_now;
  s_sched.idle_task = &s_default_idle_task;
  mu_task_init(&s_default_idle_task, default_idle_fn, NULL, "Idle");

  mu_spsc_init(&s_sched.irq_task_queue, s_sched.irq_task_queue_store,
               MU_IRQ_TASK_QUEUE_SIZE);
  mu_dlist_init(&s_sched.task_list);
  mu_sched_reset();
}

void mu_sched_reset(void) {
  mu_spsc_reset(&s_sched.irq_task_queue);

  while (!mu_dlist_is_empty(&s_sched.task_list)) {
    mu_dlist_pop(&s_sched.task_list);
  }
  s_sched.current_task = NULL;
}

mu_sched_err_t mu_sched_step(void) {
  mu_time_t now = mu_sched_get_current_time();
  mu_task_t *irq_task;

  // Transfer any pending tasks from the interrupt queue to the main queue
  while (mu_spsc_get(&s_sched.irq_task_queue, (mu_spsc_item_t *)(&irq_task)) ==
         MU_SPSC_ERR_NONE) {
    queue_task(irq_task);
  }

  // Process one task in the main queue (or idle task if none are runnable)
  s_sched.current_task = get_runnable_task(now);
  mu_task_call(s_sched.current_task, NULL);
  s_sched.current_task = NULL;

  return MU_SCHED_ERR_NONE;
}

mu_task_t *mu_sched_get_idle_task(void) { return s_sched.idle_task; }

mu_task_t *mu_sched_get_default_idle_task(void) { return &s_default_idle_task; }

void mu_sched_set_idle_task(mu_task_t *task) { s_sched.idle_task = task; }

mu_clock_fn mu_sched_get_clock_source(void) { return s_sched.clock_fn; }

void mu_sched_set_clock_source(mu_clock_fn clock_fn) {
  s_sched.clock_fn = clock_fn;
}

mu_time_t mu_sched_get_current_time(void) { return s_sched.clock_fn(); }

int mu_sched_task_count(void) { return mu_dlist_length(&s_sched.task_list); }

bool mu_sched_is_empty(void) { return mu_dlist_is_empty(&s_sched.task_list); }

mu_task_t *mu_sched_get_current_task(void) { return s_sched.current_task; }

mu_task_t *mu_sched_get_next_task(void) { return peek_next_task(); }

mu_task_t *mu_sched_remove_task(mu_task_t *task) {
  if (mu_dlist_unlink(mu_task_link(task)) == NULL) {
    task = NULL;
  }
  return task;
}

mu_sched_err_t mu_sched_task_now(mu_task_t *task) {
  mu_task_set_time(task, mu_sched_get_current_time());
  return queue_task(task);
}

mu_sched_err_t mu_sched_task_at(mu_task_t *task, mu_time_t at) {
  mu_task_set_time(task, at);
  return queue_task(task);
}

mu_sched_err_t mu_sched_task_in(mu_task_t *task, mu_duration_t in) {
  mu_task_set_time(task, mu_time_offset(mu_sched_get_current_time(), in));
  return queue_task(task);
}

mu_sched_err_t mu_sched_reschedule_now(void) {
  mu_task_t *task = mu_sched_get_current_task();
  return mu_sched_task_now(task);
}

mu_sched_err_t mu_sched_reschedule_in(mu_duration_t in) {
  mu_task_t *task = mu_sched_get_current_task();
  mu_task_set_time(task, mu_time_offset(mu_task_get_time(task), in));
  return queue_task(task);
}

mu_sched_err_t mu_sched_isr_task_now(mu_task_t *task) {
  mu_task_set_time(task, mu_sched_get_current_time());
  return queue_isr_task(task);
}

mu_sched_err_t mu_sched_isr_task_at(mu_task_t *task, mu_time_t at) {
  mu_task_set_time(task, at);
  return queue_isr_task(task);
}

mu_sched_err_t mu_sched_isr_task_in(mu_task_t *task, mu_duration_t in) {
  mu_task_set_time(task, mu_time_offset(mu_sched_get_current_time(), in));
  return queue_isr_task(task);
}

mu_sched_task_status_t mu_sched_get_task_status(mu_task_t *task) {
  if (mu_sched_get_current_task() == task) {
    // task is the current task
    return MU_SCHED_TASK_STATUS_ACTIVE;
  }
  if (!mu_task_is_scheduled(task)) {
    return MU_SCHED_TASK_STATUS_IDLE;
  }

  mu_time_t now = mu_sched_get_current_time();
  if (!mu_time_follows(mu_task_get_time(task), now)) {
    // task's time has arrived, but it's not yet running
    return MU_SCHED_TASK_STATUS_RUNNABLE;

  } else {
    // task is scheduled for some point in the future
    return MU_SCHED_TASK_STATUS_SCHEDULED;
  }
}

mu_task_t *mu_sched_traverse(mu_sched_traverse_fn user_fn, void *arg) {
  // TODO: Implement me.
  return NULL;
}

// =============================================================================
// local (static) code

static void default_idle_fn(void *self, void *arg) {
  (void)(self);
  (void)(arg);
  // the default idle task doesn't do much...
}

static mu_task_t *peek_next_task(void) {
  mu_dlist_t *link = mu_dlist_first(&s_sched.task_list);
  if (link != NULL) {
    return MU_DLIST_CONTAINER(link, mu_task_t, link);
  } else {
    return NULL;
  }
}

static mu_task_t *get_runnable_task(mu_time_t now) {
  mu_task_t *task = NULL;

  task = peek_next_task(); // peek at next task.
  if ((task != NULL) && !mu_time_follows(mu_task_get_time(task), now)) {
    // time to run the task: pop from queue
    mu_dlist_pop(&s_sched.task_list);
  } else {
    // no runnable task in the queue: use the idle task.
    task = mu_sched_get_idle_task();
  }
  return task;
}

static mu_sched_err_t queue_task(mu_task_t *task) {
  mu_time_t time = mu_task_get_time(task);
  if (mu_dlist_unlink(mu_task_link(task)) != NULL) {
    // here if a task was already scheduled - useful for debugging
    asm("nop");
  }
  mu_dlist_t *list = find_insertion_point(&s_sched.task_list, time);
  mu_dlist_insert_prev(list, mu_task_link(task));
  // mu_sched_print_state();  // ###
  return MU_SCHED_ERR_NONE;
}

static mu_sched_err_t queue_isr_task(mu_task_t *task) {
  // Add task to the ISR task queue.  This can be safely called from interrupt
  // level because irq_task_queue is a single producer / single consumer queue
  // designed for this purpose.
  if (mu_spsc_put(&s_sched.irq_task_queue, task) != MU_SPSC_ERR_NONE) {
    return MU_SCHED_ERR_FULL;
  } else {
    return MU_SCHED_ERR_NONE;
  }
}

/**
 * @brief Return the list element that "is older" than the given time.
 *
 * The list head is considered older than all times, so the task can
 * always be inserted before it.  And because a dlist is essentially
 * circular, this function is guaranteed to always return a non-null
 * list element.
 */
static mu_dlist_t *find_insertion_point(mu_dlist_t *head, mu_time_t time) {
  if (mu_dlist_is_empty(head)) {
    return head;
  } else {
    mu_dlist_t *list = mu_dlist_next(head);
    while (list != head) {
      mu_task_t *incumbent = MU_DLIST_CONTAINER(list, mu_task_t, link);
      if (mu_time_precedes(time, mu_task_get_time(incumbent))) {
        break;
      }
      list = mu_dlist_next(list);
    }
    return list;
  }
}
