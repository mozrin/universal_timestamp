/**
 * @file ut_now.c
 * @brief Implementation of ut_now() and ut_now_monotonic() functions.
 */

#include "universal_timestamp.h"
#include <time.h>
#include <stdatomic.h>

#if defined(_WIN32) || defined(_WIN64)
#define UT_WINDOWS 1
#include <windows.h>
#endif

static atomic_int_fast64_t g_last_monotonic = ATOMIC_VAR_INIT(0);
static ut_regression_callback_t g_regression_callback = NULL;

/* Windows epoch (1601-01-01) to Unix epoch (1970-01-01) in 100-nanosecond intervals */
static const int64_t WINDOWS_TICK = 10000000LL;
static const int64_t WINDOWS_TO_UNIX_EPOCH = 11644473600LL;

/**
 * @brief Get the current UTC timestamp.
 */
ut_timestamp_t ut_now(void) {
    ut_timestamp_t ts;

#if defined(UT_WINDOWS)
    FILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);
    int64_t wintime = ((int64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    int64_t unix_100ns = wintime - (WINDOWS_TO_UNIX_EPOCH * WINDOWS_TICK);
    ts.nanos = unix_100ns * 100LL;
#elif defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    ts.nanos = (int64_t)spec.tv_sec * 1000000000LL + spec.tv_nsec;
#else
    ts.nanos = (int64_t)time(NULL) * 1000000000LL;
#endif

    return ts;
}

/**
 * @brief Get the current UTC timestamp with monotonic guarantee.
 */
ut_timestamp_t ut_now_monotonic(void) {
    ut_timestamp_t current = ut_now();

    int64_t last = atomic_load(&g_last_monotonic);
    int64_t new_val;

    do {
        if (current.nanos <= last) {
            new_val = last + 1;

            if (g_regression_callback != NULL) {
                ut_timestamp_t expected = {last + 1};
                ut_timestamp_t adjusted = {new_val};
                g_regression_callback(expected, current, adjusted);
            }
        } else {
            new_val = current.nanos;
        }
    } while (!atomic_compare_exchange_weak(&g_last_monotonic, &last, new_val));

    ut_timestamp_t result = {new_val};
    return result;
}

/**
 * @brief Set a callback for clock regression events.
 */
void ut_set_regression_callback(ut_regression_callback_t callback) {
    g_regression_callback = callback;
}

/**
 * @brief Create a timestamp from Unix nanoseconds.
 */
ut_timestamp_t ut_from_unix_nanos(int64_t nanos) {
    ut_timestamp_t ts = {nanos};
    return ts;
}

/**
 * @brief Convert a timestamp to Unix nanoseconds.
 */
int64_t ut_to_unix_nanos(ut_timestamp_t ts) {
    return ts.nanos;
}

/**
 * @brief Get a human-readable error message.
 */
const char *ut_error_string(ut_error_t err) {
    switch (err) {
        case UT_OK:                   return "Success";
        case UT_ERR_INVALID_FORMAT:   return "Invalid format";
        case UT_ERR_INVALID_DATE:     return "Invalid date";
        case UT_ERR_OUT_OF_RANGE:     return "Value out of range";
        case UT_ERR_UNSUPPORTED_OFFSET: return "Unsupported timezone offset";
        case UT_ERR_FRACTION_TOO_LONG: return "Fractional seconds too long";
        case UT_ERR_LEAP_SECOND:      return "Leap second not supported";
        case UT_ERR_NULL_POINTER:     return "Null pointer";
        default:                      return "Unknown error";
    }
}

/**
 * @brief Get the calendar system used for date calculations.
 */
ut_calendar_t ut_get_calendar(void) {
    return UT_CALENDAR_GREGORIAN;
}

/**
 * @brief Detect the clock precision available on the current hardware.
 */
ut_precision_t ut_get_clock_precision(void) {
    const int NUM_SAMPLES = 100;
    int64_t samples[100];
    
    for (int i = 0; i < NUM_SAMPLES; i++) {
        ut_timestamp_t ts = ut_now();
        samples[i] = ts.nanos;
    }
    
    int has_nanos = 0;
    int has_micros = 0;
    int has_millis = 0;
    
    for (int i = 0; i < NUM_SAMPLES; i++) {
        int64_t n = samples[i];
        if (n % 1000 != 0) has_nanos = 1;
        if (n % 1000000 != 0) has_micros = 1;
        if (n % 1000000000 != 0) has_millis = 1;
    }
    
    if (has_nanos) return UT_PRECISION_NANOSECOND;
    if (has_micros) return UT_PRECISION_MICROSECOND;
    if (has_millis) return UT_PRECISION_MILLISECOND;
    return UT_PRECISION_SECOND;
}
