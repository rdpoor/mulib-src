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

// =============================================================================
// includes


#include "mulib.h"
#include "atmel_start.h"

// =============================================================================
// private types and definitions

// =============================================================================
// private declarations

static void rtc_cb_trampoline(struct calendar_dev *const dev);

static int quo_rounded(int x, int y);

// =============================================================================
// local storage

#ifdef MU_FLOAT
MU_FLOAT s_rtc_period;
#endif

// =============================================================================
// public code

void mu_time_init(void) {
  // Initialize the RTC.  Use CALENDAR_0 since that's the only published
  // interface for interacting with the underlying RTC.
  calendar_enable(&CALENDAR_0); // start RTC
  _calendar_register_callback(&CALENDAR_0.device, rtc_cb_trampoline);

#ifdef MU_FLOAT
  s_rtc_period = 1.0 / (MU_FLOAT)RTC_FREQUENCY;
#endif
}

mu_time_t mu_time_now(void) {
  return hri_rtcmode0_read_COUNT_COUNT_bf(CALENDAR_0.device.hw);
}

mu_time_t mu_time_offset(mu_time_t t, mu_time_dt dt) {
  return t + dt;
}

mu_time_dt mu_time_difference(mu_time_t t1, mu_time_t t2) {
  return t1 - t2;
}

bool mu_time_precedes(mu_time_t t1, mu_time_t t2) {
  volatile mu_time_dt dt = mu_time_difference(t1, t2);
  return dt < 0;
}

bool mu_time_equals(mu_time_t t1, mu_time_t t2) {
  return t1 == t2;
}

bool mu_time_follows(mu_time_t t1, mu_time_t t2) {
  mu_time_dt dt = mu_time_difference(t2, t1);
  return dt < 0;
}

mu_time_dt mu_time_ms_to_duration(mu_time_ms_dt ms) {
  return quo_rounded(ms * RTC_FREQUENCY, 1000);
}

mu_time_ms_dt mu_time_duration_to_ms(mu_time_dt dt) {
  return quo_rounded(dt * 1000, RTC_FREQUENCY);
}

#ifdef MU_FLOAT

MU_FLOAT mu_time_duration_to_s(mu_time_dt dt) {
  return dt * s_rtc_period;
}

mu_time_dt mu_time_s_to_duration(MU_FLOAT seconds) {
  return seconds / s_rtc_period;
}

#endif

// =============================================================================
// local (static) code

static void rtc_cb_trampoline(struct calendar_dev *const dev) {
  // Arrive here when the RTC count register matches the RTC compare register.
  // This will wake the processor from sleep...
}

// See https://stackoverflow.com/a/18067292/558639
//
static int quo_rounded(int x, int y) {
  // What does it all mean?
  //   (x < 0) is false (zero) if x is non-negative
  //   (y < 0) is false (zero) if x is non-negative
  //   (x < 0) ^ (y < 0) is true if x and y have opposite signs
  //   x/y would be the quotient, but it is truncated towards zero.  To round:
  //   (x + y/2)/y is the rounded quotient when x and y have the same sign
  //   (x - y/2)/y is the rounded quotient when x and y have opposite signs
  return ((x < 0) ^ (y < 0)) ? ((x - y/2)/y) : ((x + y/2)/y);
}
