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

// =============================================================================
// Includes

#include "mu_fsm.h"

// =============================================================================
// Private types and definitions

// =============================================================================
// Private declarations

// =============================================================================
// Local storage

// =============================================================================
// Public code

void mu_fsm_init(mu_fsm_t *fsm,
                 mu_fsm_state_fn_t fns[],
                 const char *names[],
                 size_t n_states) {
  fsm->fns = fns;
  fsm->names = names;
  fsm->state = 0; // by convention
  fsm->n_states = n_states;
}

void mu_fsm_dispatch(mu_fsm_t *fsm, void *receiver, void *sender) {
  if (fsm->state < fsm->n_states) {
    mu_fsm_state_fn_t fn = fsm->fns[fsm->state];
    fn(receiver, sender);
  }
}

void mu_fsm_advance(mu_fsm_t *fsm, int state) { fsm->state = state; }

const char *mu_fsm_state_name(mu_fsm_t *fsm, int state) {
  if (fsm->names) {
    if (state < fsm->n_states) {
      // valid state
      return fsm->names[state];
    } else {
      // state is out of range.
      return "unknown state";
    }
  }
  // state names not provided
  return "";
}

// =============================================================================
// Private functions
