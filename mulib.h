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

#ifndef _MULIB_H_
#define _MULIB_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include "mu_config.h" // must come first!
#include "mu_time.h"

#include "core/mu_bitvec.h"
#include "core/mu_cirq.h"
#include "core/mu_list.h"
#include "core/mu_log.h"
#include "core/mu_pstore.h"
#include "core/mu_queue.h"
#include "core/mu_sched.h"
#include "core/mu_spscq.h"
#include "core/mu_str.h"
#include "core/mu_strbuf.h"
#include "core/mu_task.h"
#include "core/mu_thunk.h"
#include "core/mu_timer.h"
#include "core/mu_vect.h"
#include "core/mu_version.h"

#include "extras/mu_rfc_1123.h"

// =============================================================================
// types and definitions

// =============================================================================
// declarations

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MULIB_H_ */
