/**
 * MIT License
 *
 * Copyright (c) 2020 R. D. Poor <rdpoor@gmail.com>
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

#ifndef _MU_FSM_H_
#define _MU_FSM_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stddef.h>

// =============================================================================
// types and definitions

// The signature of an FSM state function.
typedef void (*mu_fsm_state_fn_t)(void *receiver, void *sender);

typedef struct {
  mu_fsm_state_fn_t *fns; // table of function pointers
  const char **names;     // may be NULL
  int state;              // the current state
  size_t n_states;        // size of fns[]
} mu_fsm_t;

// =============================================================================
// Declarations

void mu_fsm_init(mu_fsm_t *fsm,
                 mu_fsm_state_fn_t fns[],
                 const char *names[],
                 size_t n_states);

void mu_fsm_dispatch(mu_fsm_t *fsm, void *receiver, void *sender);

void mu_fsm_advance(mu_fsm_t *fsm, int state);

const char *mu_fsm_state_name(mu_fsm_t *fsm, int state);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _MU_FSM_H_
