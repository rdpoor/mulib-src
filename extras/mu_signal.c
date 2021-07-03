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

#include "mu_signal.h"

// =============================================================================
// Local types and definitions

// =============================================================================
// Local storage

// =============================================================================
// Local (forward) declarations
/**
 * @brief Intercept CTRL-C (SIGINT) and shunt it to exit(0) so that any functions registered with atexit() will run before we quit.  
 * mu_kbd_io uses this to restore termios settings, for example.
 * 
 */

static void signal_handling_init();

// =============================================================================
// Public code

void mu_signal_init(void) {
    signal_handling_init();
}
// =============================================================================
// Local (static) code

#ifdef HAS_SIGNAL
  #include <signal.h>
  #include <stdio.h>
  #include <stdlib.h>
  static void handle_signint(int s) {
    exit(0);
  }
  static void signal_handling_init(void) {
    if(signal(SIGINT, SIG_IGN) != SIG_IGN) 
      signal(SIGINT, handle_signint);
  }
#else
  static void signal_handling_init() {}
#endif