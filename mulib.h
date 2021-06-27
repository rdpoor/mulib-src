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
// There are two build strategies, referred to as the "prix fixe" approach and
// the "ala carte" approach.
//
// The _prix fixe_ approach is the easiest to implement: just `#include mulib.h`
// in any file that requires mulib functionality and call `mulib_init()` when
// the system first starts, and everything just works.
//
// But if you want the smallest possible code impage, you'll want to use the
// _ala carte_ approach.  In this scheme, you `#include` only the mulib modules
// that your application needs.  In addition, for each module named `xxx`, you
// may need to call `xxx_init()` if that module requires it -- consult the docs
// see which modules require initialization.

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

//
//
#include "mu_platform.h"

// Core mulib module declarations.
//
#include "core/mu_array.h"
#include "core/mu_bvec.h"
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
