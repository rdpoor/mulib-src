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

#ifndef _MU_PRINTER_TASK_H_
#define _MU_PRINTER_TASK _H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include "mulib.h"
#include <stdbool.h>
#include <stdint.h>

// =============================================================================
// types and definitions

typedef struct {
  mu_task_t printer_task;
  mu_task_t *on_completion;
  uint8_t const *data;
  size_t data_len;
} mu_printer_task_ctx;

// =============================================================================
// declarations

/**
 * @brief Start a mu_task that prints a string to the mu_vm_serial port.
 */
mu_printer_task_ctx *mu_printer_task_init(mu_printer_task_ctx *ctx,
                                          mu_task_t *on_completion,
                                          mu_sched_t *scheduler);

mu_printer_task_ctx *mu_printer_task_print(mu_printer_task_ctx *ctx,
                                           uint8_t *data,
                                           size_t data_len);

bool mu_printer_task_is_active(mu_printer_task_ctx *ctx);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MU_PRINTER_TASK_H_ */
