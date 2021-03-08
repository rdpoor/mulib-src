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

#include "mulib.h" // must come first

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h> // memmove

// =============================================================================
// local types and definitions

typedef struct {
  mu_dlist_t task_list;    // time ordered list of tasks (soonest first)
  mu_clock_fn clock_fn;    // function to call to get the current time
  mu_task_t *idle_task;    // the idle task
  mu_task_t *current_task; // the task currently being processed
} mu_sched_t;

// =============================================================================
// local (forward) declarations

static void default_idle_fn(void *self, void *arg);

static mu_task_t *get_next_task(void);

static mu_task_t *get_runnable_task(mu_time_t now);

static mu_sched_err_t sched_task_at(mu_task_t *task, mu_time_t time);

static mu_dlist_t *find_insertion_point(mu_dlist_t *head, mu_time_t time);

// =============================================================================
// local storage

// Singleton instance of the scheduler
static mu_sched_t s_sched;

static mu_task_t s_default_idle_task;

// =============================================================================
// public code

void mu_sched_init() {
  s_sched.clock_fn = mu_time_now;
  s_sched.idle_task = &s_default_idle_task;
  mu_task_init(&s_default_idle_task, default_idle_fn, NULL, "Idle");

  mu_dlist_init(&s_sched.task_list);
  mu_sched_reset();
}

void mu_sched_reset(void) {
  while (!mu_dlist_is_empty(&s_sched.task_list)) {
    mu_dlist_pop(&s_sched.task_list);
  }
  s_sched.current_task = NULL;
}

mu_sched_err_t mu_sched_step(void) {
  mu_time_t now = mu_sched_get_current_time();

  // process one task in the main queue
  if ((s_sched.current_task = get_runnable_task(now)) != NULL) {
    mu_task_call(s_sched.current_task, NULL);
    s_sched.current_task = NULL;
  } else {
    mu_task_call(s_sched.idle_task, NULL);
  }
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

int mu_sched_task_count(void) {
  // TODO: needs interrupt protection?
  return mu_dlist_length(&s_sched.task_list);
}

bool mu_sched_is_empty(void) {
  // TODO: needs interrupt protection?
  return mu_dlist_is_empty(&s_sched.task_list);
}

mu_task_t *mu_sched_get_current_task(void) { return s_sched.current_task; }

mu_task_t *mu_sched_get_next_task(void) {
  mu_task_t *task = NULL;

  MU_WITH_INTERRUPTS_DISABLED(
    task = get_next_task();
  )

  return task;
}

mu_task_t *mu_sched_remove_task(mu_task_t *task) {
  MU_WITH_INTERRUPTS_DISABLED(
    if (mu_dlist_unlink(mu_task_link(task)) == NULL) {
      task = NULL;
    }
  )
  return task;
}

mu_sched_err_t mu_sched_task_now(mu_task_t *task) {
  return sched_task_at(task, mu_sched_get_current_time());
}

mu_sched_err_t mu_sched_task_at(mu_task_t *task, mu_time_t at) {
  return sched_task_at(task, at);
}

mu_sched_err_t mu_sched_task_in(mu_task_t *task, mu_time_dt in) {
  return sched_task_at(task, mu_time_offset(mu_sched_get_current_time(), in));
}

mu_sched_err_t mu_sched_reschedule_now(void) {
  mu_task_t *task = mu_sched_get_current_task();
  return mu_sched_task_now(task);
}

mu_sched_err_t mu_sched_reschedule_in(mu_time_dt in) {
  mu_task_t *task = mu_sched_get_current_task();
  return sched_task_at(task, mu_time_offset(mu_task_get_time(task), in));
}

mu_sched_err_t mu_sched_task_from_isr(mu_task_t *task) {
  // TODO: no longer needed?
  return sched_task_at(task, mu_sched_get_current_time());
}

mu_sched_task_status_t mu_sched_get_task_status(mu_task_t *task) {
  // TODO: Needs interrupt protection?
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

// =============================================================================
// local (static) code

static void default_idle_fn(void *self, void *arg) {
  (void)(self);
  (void)(arg);
  // the default idle task doesn't do much...
}

static mu_task_t *get_next_task(void) {
  mu_dlist_t *link = mu_dlist_first(&s_sched.task_list);
  if (link != NULL) {
    return MU_DLIST_CONTAINER(link, mu_task_t, link);
  } else {
    return NULL;
  }
}

static mu_task_t *get_runnable_task(mu_time_t now) {
  mu_task_t *task = NULL;

  MU_WITH_INTERRUPTS_DISABLED(
    task = get_next_task(); // peek at next task.
    if ((task != NULL) && !mu_time_follows(mu_task_get_time(task), now)) {
      // time to run the task: pop from queue
      mu_dlist_pop(&s_sched.task_list);
    } else {
      task = NULL;
    }
  );
  return task;
}

static mu_sched_err_t sched_task_at(mu_task_t *task, mu_time_t time) {
  MU_WITH_INTERRUPTS_DISABLED(
    mu_task_set_time(task, time);
    mu_dlist_unlink(mu_task_link(task));
    mu_dlist_t *list = find_insertion_point(&s_sched.task_list, time);
    mu_dlist_insert_prev(list, mu_task_link(task));
  );
  return MU_SCHED_ERR_NONE;
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
