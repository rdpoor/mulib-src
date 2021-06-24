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

// About mulib.h:
//
// If you are using the "smorgasborg" approach, simply add "#include mulib.h"
// near the top of any file that uses mulib functions, and be sure to call
// mulib_init() after performing any processor and board-level initialization.
//
// If you are using the "ala carte" approach, copy the selected #include entries
// from this file into your code.  You may need to call `xxx_init()` (where xxx
// is the name of a module) prior to using module xxx -- consult the docs for
// each module to see what initialization is required, if any.

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

// You must provide platform-specific declarations and implementation for the
// following modules in your project's mu_platform directory
//
#include "mu_platform/mu_config.h"
#include "mu_platform/mu_button_io.h"
#include "mu_platform/mu_led_io.h"
#include "mu_platform/mu_time.h"

// Core mulib module declarations.
//
// #include "core/mu_bvec.h"
// #include "core/mu_cirq.h"
#include "core/mu_dlist.h"
// #include "core/mu_fsm.h"
// #include "core/mu_list.h"
// #include "core/mu_log.h"
// #include "core/mu_pstore.h"
// #include "core/mu_queue.h"
#include "core/mu_sched.h"
#include "core/mu_spsc.h"
#include "core/mu_str.h"
#include "core/mu_strbuf.h"
#include "core/mu_task.h"
#include "core/mu_thunk.h"
// #include "core/mu_timer.h"
// #include "core/mu_vect.h"
#include "core/mu_version.h"

// Extra mulib module declarations.
//
#include "extras/mu_rfc_1123.h"
#include "extras/mu_random.h"
#include "extras/mu_ansi_term.h"

// =============================================================================
// types and definitions

// =============================================================================
// declarations

/**
 * @brief Initialize the mulib system.
 *
 * This function should be called once at startup before calling any mulib
 * functions.
 */
void mulib_init(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MULIB_H_ */
