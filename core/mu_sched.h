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

#ifndef _MU_SCHED_H_
#define _MU_SCHED_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

//#include "mulib.h"


#include "mu_config.h"
#include "mu_spscq.h"
#include "mu_task.h"
#include "mu_time.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// =============================================================================
// types and definitions

typedef enum {
  MU_SCHED_ERR_NONE,
  MU_SCHED_ERR_EMPTY,
  MU_SCHED_ERR_FULL,
  MU_SCHED_ERR_NOT_FOUND,
  MU_SCHED_ERR_NULL_TASK,  // return value for scheduling null task.
} mu_sched_err_t;

typedef enum {
  MU_SCHED_TASK_STATUS_IDLE,      // task is not scheduled
  MU_SCHED_TASK_STATUS_SCHEDULED, // task is in the schedule, not yet runnable
  MU_SCHED_TASK_STATUS_RUNNABLE,  // task is in the schedule, ready to run
  MU_SCHED_TASK_STATUS_ACTIVE,    // task is running
} mu_sched_task_status_t;

// Signature for clock source function.  Returns the current time.
typedef mu_time_t (*mu_clock_fn)(void);

// =============================================================================
// declarations

/**
 * \brief initialize the schedule module.  Not interrupt safe.
 *
 * TODO: Error return if isq_queue_capacity is not a power of 2.
 */
 void mu_sched_init(mu_spscq_item_t *isr_queue_store, uint16_t isr_queue_capacity);

/**
 * \brief  Remove all scheduled items from the schedule.  Not interrupt safe.
 */
void mu_sched_reset(void);

/**
 * @brief Process the next runnable task or -- if none are runnable -- the idle
 * task.
 */
mu_sched_err_t mu_sched_step(void);

/**
 * @brief Return the list of tasks in the schedule.
 *
 * @deprecated This function exposes the internal implementation of the schduler
 * and is subject to change.  A better approach would be to provide an iterator
 * that traverses the elements of the schedule.
 */
mu_list_t mu_sched_task_list(void);

/**
 * @brief Return the single-producer, single-consumer interrupt quee.
 */
mu_spscq_t *mu_sched_isr_queue(void);

/**
 * @brief Return the default idle task (which does nothing but return).
 */
mu_task_t *mu_sched_get_default_idle_task(void);

/**
 * @brief Return the idle task.
 */
mu_task_t *mu_sched_get_idle_task(void);

/**
 * @brief Set the task to be invoked when there are no tasks to run.
 */
void mu_sched_set_idle_task(mu_task_t *task);

/**
 * @brief Return the current clock souce.
 */
mu_clock_fn mu_sched_get_clock_source(void);

/**
 * @brief Set the clock source for the scheduler.
 *
 * @param sched The scheduler object.
 * @param clock_fn A function that returns the current time.
 */
void mu_sched_set_clock_source(mu_clock_fn clock_fn);

/**
 * @brief Return the current time.
 */
mu_time_t mu_sched_get_current_time(void);

/**
 * @brief Return the number of tasks currently scheduled.
 */
int mu_sched_task_count(void);

/**
 * @brief Return true if there are no items in the schedule.
 */
bool mu_sched_is_empty(void);

/**
 * @brief Return the task currently being run, or NULL if none.
 */
mu_task_t *mu_sched_get_current_task(void);

/**
 * @brief Return the next task to be run or NULL if thre are none to run.
 */
mu_task_t *mu_sched_get_next_task(void);

/**
 * @brief Remove a scheduled task.
 *
 * @param sched The scheduler object.
 * @param task The task to be removed.
 * @return The task that was removed, or NULL if it was not scheduled.
 */
mu_task_t *mu_sched_remove_task(mu_task_t *task);

/**
 * @brief Schedule (or reschedule) a task to be run as soon as possible.
 *
 * Note: If there are other runnable tasks, the new task will be scheduled after
 * those have run.
 *
 * Note: If the task is currently in the schedule, it will be removed and
 * re-scheduled.
 *
 * @param task The task to be scheduled.
 * @return MU_SCHED_ERR_NONE.  (Other error returns may be added in the future.)
 */
mu_sched_err_t mu_sched_task_now(mu_task_t *task);

/**
 * @brief Schedule a task to be run at a particular time.
 *
 * Note: If the task is currently in the schedule, it will be removed and
 * re-scheduled.
 *
 * @param task The task to be scheduled.
 * @param at The time at which to run the task.
 * @return MU_SCHED_ERR_NONE.  (Other error returns may be added in the future.)
 */
mu_sched_err_t mu_sched_task_at(mu_task_t *task, mu_time_t at);

/**
 * @brief Schedule a task to be run after a given interval.
 *
 * Note: If the task is currently in the schedule, it will be removed and
 * re-scheduled.
 *
 * @param task The task to be scheduled.
 * @param in The interval after which to run the task.
 * @return MU_SCHED_ERR_NONE.  (Other error returns may be added in the future.)
 */
mu_sched_err_t mu_sched_task_in(mu_task_t *task, mu_time_dt in);

/**
 * @brief Reschedule the current task to run as soon as possible.
 *
 * Note: If there are other runnable tasks, the task will be scheduled after
 * they have run: they get a chance to run first.
 *
 * @return MU_SCHED_ERR_NOT_FOUND if there is no current task, MU_SCHED_ERR_NONE
 * otherwise.
 */
mu_sched_err_t mu_sched_reschedule_now(void);

/**
 * @brief Reschedule the current task after the given interval.
 *
 * Note that to avoid drift, this increments the task's time rather than
 * the current time.
 *
 * @param in The interval after which to run the task.
 * @return MU_SCHED_ERR_NOT_FOUND if there is no current task, MU_SCHED_ERR_NONE
 * otherwise.
 */
mu_sched_err_t mu_sched_reschedule_in(mu_time_dt in);

/**
 * @brief Schedule a task from interrupt level.
 *
 * Note: The task will be safely queued and will be scheduled to run at the next
 * call to sched_step().
 *
 * @param task The task to be scheduled.
 * @return MU_SCHED_ERR_NONE.  (Other error returns may be added in the future.)
 */
mu_sched_err_t mu_sched_task_from_isr(mu_task_t *task);

/**
 * @brief Return the status of a task.
 *
 * Task status may be one of
 * * MU_SCHED_TASK_STATUS_IDLE -- task is not scheduled
 * * MU_SCHED_TASK_STATUS_SCHEDULED --  task is scheduled but not yet runnable
 * * MU_SCHED_TASK_STATUS_RUNNABLE -- task is ready to run
 * * MU_SCHED_TASK_STATUS_ACTIVE -- task is currently running
 *
 * @param sched The scheduler object.
 * @param task The task being queried.
 * @return task status.
 */
mu_sched_task_status_t mu_sched_get_task_status(mu_task_t *task);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_SCHED_H_ */
