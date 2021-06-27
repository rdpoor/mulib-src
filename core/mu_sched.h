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

 /**
Implmeentation notes:

mu_sched implements a discrete time, run-to-completion scheduler.  A mu_task can
be scheduled to run at some point in the future through the following calls:

    mu_sched_err_t mu_sched_task_now(mu_task_t *task);
    mu_sched_err_t mu_sched_task_at(mu_task_t *task, mu_time_t at);
    mu_sched_err_t mu_sched_task_in(mu_task_t *task, mu_duration_t in);
    mu_sched_err_t mu_sched_reschedule_now(void);
    mu_sched_err_t mu_sched_reschedule_in(mu_duration_t in);

Each of these functions add a task to the scheduler's queue, implemented as a
doubly linked list.  In every case, if the task is already present in the queue,
it is removed prior to scheduling.  This prevents "runaway" scheduling.

The following functions support scheduling events from interrupt level:

    mu_sched_err_t mu_sched_isr_task_now(mu_task_t *task);
    mu_sched_err_t mu_sched_isr_task_at(mu_task_t *task, mu_time_t at);
    mu_sched_err_t mu_sched_isr_task_in(mu_task_t *task, mu_duration_t in);

The function

    mu_sched_err_t mu_sched_step(void);

is where all the magic happens.  The scheduler examines the first task in the
queue, and if its start time has arrived, the task is removed from the queue and
is called.

## Implementation of the scheduler queue

mu_sched makes an conscious design choice that each task may only appear once
in the schedule.  This means that at each call to mu_sched_task_xxx(task), the
scheduler must check to see if the task is already in the queue, and if so, to
remove it.

To support fast check and removal of a task, the scheduler queue is implemented
as a doubly linked list: removing a task from the queue merely requires a pair
of pointer operations.

This approach also means that the task can encapsulate the `prev` and `next`
link fields requires to insert it into the queue, so no additional storage is
required.

## Implementation of the ISR queue

mu_sched supports scheduling tasks from interrupt level via the
`mu_sched_isr_task_xxx()` functions.  Unlike their foreground counterparts,
these functions do not add tasks directly to the scheduler queue.  Instead,
tasks are added to a "single produce, single consumer" isr queue, which is
guaranteed to be interrupt safe.

At foreground level, at the next call to mu_sched_step(), any tasks on the isr
queue are transferred from the isr queue to the regular scheduler queue.
*/

#ifndef _MU_SCHED_H_
#define _MU_SCHED_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include "mu_task.h"
#include "mu_platform/mu_time.h"
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

/**
 * @brief Signature for a function passed to mu_sched_traverse.
 *
 * @param task a pointer to a task..
 * @param arg A user-supplied argument.
 * @return A NULL value to continue traversing, a non-null value to stop.
 */
typedef mu_task_t *(*mu_sched_traverse_fn)(mu_task_t *task, void *arg);

// =============================================================================
// declarations

/**
 * @brief initialize the schedule module.  Not interrupt safe.
 */
 void mu_sched_init(void);

/**
 * @brief  Remove all scheduled items from the schedule.  Not interrupt safe.
 */
void mu_sched_reset(void);

/**
 * @brief Process the next runnable task or -- if none are runnable -- the idle
 * task.
 */
mu_sched_err_t mu_sched_step(void);

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
mu_sched_err_t mu_sched_task_in(mu_task_t *task, mu_duration_t in);

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
mu_sched_err_t mu_sched_reschedule_in(mu_duration_t in);

/**
 * @brief Schedule a task from interrupt level.
 *
 * Note: At the next call to mu_sched_step(), if the task is currently in the
 * schedule, it will be removed and re-scheduled.
 *
 * @param task The task to be scheduled.
 * @return MU_SCHED_ERR_NONE on no error, MU_SCHED_ERR_FULL if the interrupt
 *         queue is full.
 */
mu_sched_err_t mu_sched_isr_task_now(mu_task_t *task);

/**
 * @brief Schedule a task from interrupt level to be run at a particular time.
 *
 * Note: At the next call to mu_sched_step(), if the task is currently in the
 * schedule, it will be removed and re-scheduled.
 *
 * @param task The task to be scheduled.
 * @param at The time at which to run the task.
 * @return MU_SCHED_ERR_NONE on no error, MU_SCHED_ERR_FULL if the interrupt
 *         queue is full.
 */
mu_sched_err_t mu_sched_isr_task_at(mu_task_t *task, mu_time_t at);

/**
 * @brief Schedule a task from interrupt level to be run after a given interval.
 *
 * Note: At the next call to mu_sched_step(), if the task is currently in the
 * schedule, it will be removed and re-scheduled.
 *
 * @param task The task to be scheduled.
 * @param in The interval after which to run the task.
 * @return MU_SCHED_ERR_NONE on no error, MU_SCHED_ERR_FULL if the interrupt
 *         queue is full.
 */
mu_sched_err_t mu_sched_isr_task_in(mu_task_t *task, mu_duration_t in);

/**
 * @brief Return the status of a task.
 *
 * Task status may be one of
 * * MU_SCHED_TASK_STATUS_IDLE -- task is not scheduled
 * * MU_SCHED_TASK_STATUS_SCHEDULED --  task is scheduled but not yet runnable
 * * MU_SCHED_TASK_STATUS_RUNNABLE -- task is ready to run
 * * MU_SCHED_TASK_STATUS_ACTIVE -- task is currently running
 *
 * @param task The task being queried.
 * @return task status.
 */
mu_sched_task_status_t mu_sched_get_task_status(mu_task_t *task);

/**
 * @brief Call a user-supplied function for each task in the schedule.
 *
 * The user-supplied function has the signature:
 *
 *    mu_task_t *user_fn(mu_task_t *task, void *arg)
 *
 * Traversing the list continues until the end of the list is reached, or the
 * user function returns a non-null value.
 *
 * @param user_fn The function to call with each element of the schedule.
 * @param arg The value supplies as the second argument to the user fun.
 * @return A non-null value returned by the user function, or NULL if the end of
 *         the list is reached.
 */
mu_task_t *mu_sched_traverse(mu_sched_traverse_fn user_fn, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_SCHED_H_ */
