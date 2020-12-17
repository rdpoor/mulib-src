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

#ifndef _MU_PERIODIC_TASK_H_
#define _MU_PERIODIC_TASK _H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include "mulib.h"
#include <stdbool.h>

// =============================================================================
// types and definitions

typedef struct {
  mu_task_t *triggered_task;
  mu_sched_t *sched;
  mu_vm_time_ms_dt interval_ms;
} mu_periodic_task_ctx;

// =============================================================================
// declarations

mu_task_t *mu_periodic_task_init(mu_task_t *periodic_task,
                                 mu_periodic_task_ctx *ctx,
                                 mu_task_t *triggered_task,
                                 mu_sched_t *scheduler,
                                 mu_vm_time_ms_dt interval,
                                 const char *task_name);

bool mu_periodic_task_is_active(void);

void mu_periodic_task_start(mu_task_t *periodic_task);

void mu_periodic_task_stop(mu_task_t *periodic_task);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_PERIODIC_TASK _H_ */
