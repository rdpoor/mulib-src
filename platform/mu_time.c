/**
 * A POSIX-compliant implementation for mu_time.c
 *
 * Note that this implementation represents time in seconds in a double.
 *
 * To compile and run the in-file unit tests, make sure that the mulib directory
 * is available and at the same level as mulib-test.  In a terminal wndow, type:
 *
 *  cc -Wall -g -I.. -I../../../mulib/src/platform -o mu_time mu_time.c && ./mu_time && rm ./mu_time
 *
 * R. D. Poor <rdpoor@gmail.com>
 */

#include "mu_config.h"     // must come first
#include "mu_time.h"       // included from mulib/src/platform/
#include "time.h"          // posix time functions

/**
 * @brief Initialize the time system.  Must be called before any other time
 * functions are called.
 */
void mu_time_init(void) {
  // no initialization required
}

/**
 * @brief Get the current system time.
 *
 * @return A value representing the current time.
 */
mu_time_t mu_time_now() {
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return now.tv_sec + now.tv_nsec / 1000000000.0;
}

/**
 * @brief Add a time and a duration.
 *
 * `mu_time_offset` adds a time and a duration to produce a new time object.
 *
 * @param t1 a time object
 * @param dt a duration object
 * @return t1 offset by dt
 */
mu_time_t mu_time_offset(mu_time_t t1, mu_time_dt dt) {
  return t1 + dt;
}

/**
 * @brief Take the difference between two time objects
 *
 * `mu_time_difference` subtracts t2 from t1 to produce a duration object.
 *
 * @param t1 A time object
 * @param t2 A time object
 * @return (t1-t2) as a duration object
 */
mu_time_dt mu_time_difference(mu_time_t t1, mu_time_t t2) {
  return t1 - t2;
}

/**
 * @brief Return true if t1 is strictly before t2
 *
 * Note that if you want to know if t1 is before or equal to t2, you can use the
 * construct `!mu_time_follows(t2, t1)``
 *
 * @param t1 A time object
 * @param t2 A time object
 * @return true if t1 is strictly before t2, false otherwise.
 */
bool mu_time_precedes(mu_time_t t1, mu_time_t t2) {
  return t1 < t2;
}

/**
 * @brief Return true if t1 is equal to t2
 *
 * @param t1 A time object
 * @param t2 A time object
 * @return true if t1 equals t2, false otherwise.
 */
bool mu_time_equals(mu_time_t t1, mu_time_t t2) {
  return t1 == t2;
}

/**
 * @brief Return true if t1 is strictly after t2
 *
 * Note that if you want to know if t1 is equal to or after t2, you can use the
 * construct `!mu_time_precedes(t2, t1)``
 *
 * @param t1 A time object
 * @param t2 A time object
 * @return true if t1 is strictly after t2, false otherwise.
 */
bool mu_time_follows(mu_time_t t1, mu_time_t t2) {
  return t1 > t2;
}

/**
 * @brief Convert a duration to milliseconds.
 *
 * @param dt A duration object
 * @return The duration in seconds
 */
mu_time_ms_dt mu_time_duration_to_ms(mu_time_dt dt) {
  return dt * 1000.0;
}

/**
 * @brief Convert milliseconds to a duration
 *
 * @param ms The duration in milliseconds
 * @return A duration object
 */
mu_time_dt mu_time_ms_to_duration(mu_time_ms_dt ms) {
  return ms / 1000.0;
}

#ifdef MU_FLOAT
/**
 * @brief Convert a duration to seconds.
 *
 * @param dt A duration object
 * @return The duration in seconds
 */
MU_FLOAT mu_time_duration_to_s(mu_time_dt dt) {
  return dt;
}

/**
 * @brief Convert seconds to a duration.
 *
 * @param s The duration in seconds
 * @return A duration object
 */
mu_time_dt mu_time_s_to_duration(MU_FLOAT s) {
  return s;
}

#endif

//#define MU_TIME_TEST
#ifdef MU_TIME_TEST // rest of file

#include <stdio.h>
#include <assert.h>

int main(void) {
  mu_time_t t1, t2;
  mu_time_dt dt;
  mu_time_ms_dt ms;
  mu_float_t s;

  printf("Starting mu_time unit tests...");

  mu_time_init();
  t1 = mu_time_now();
  t2 = mu_time_offset(t1, 1);
  assert(mu_time_difference(t2, t1) == 1);

  assert(mu_time_precedes(t1, t2) == true);
  assert(mu_time_equals(t1, t2) == false);
  assert(mu_time_follows(t1, t2) == false);

  assert(mu_time_precedes(t2, t1) == false);
  assert(mu_time_equals(t2, t1) == false);
  assert(mu_time_follows(t2, t1) == true);

  assert(mu_time_equals(t1, t1) == true);

  dt = mu_time_ms_to_duration(1000);
  t2 = mu_time_offset(t1, dt);
  ms = mu_time_duration_to_ms(mu_time_difference(t2, t1));
  assert(ms == 1000);

  dt = mu_time_s_to_duration(1.0);
  t2 = mu_time_offset(t1, dt);
  s = mu_time_duration_to_s(mu_time_difference(t2, t1));
  assert(s == 1.0);

  printf("done.\r\n");

  return 0;
}



#endif // #ifdef MU_TIME_TEST
