/**
 * @file ut_now.c
 * @brief Implementation of ut_now() and ut_now_monotonic() functions.
 */

#include "universal_timestamp.h"
#include <time.h>
#include <stdatomic.h>

/* Platform detection */
#if defined(_WIN32) || defined(_WIN64)
    #define UT_PLATFORM_WINDOWS 1
    #include <windows.h>
#elif defined(__EMSCRIPTEN__)
    #define UT_PLATFORM_EMSCRIPTEN 1
    #include <emscripten.h>
#elif defined(__APPLE__) && defined(__MACH__)
    #define UT_PLATFORM_APPLE 1
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define UT_PLATFORM_IOS 1
    #else
        #define UT_PLATFORM_MACOS 1
    #endif
#elif defined(__ANDROID__)
    #define UT_PLATFORM_ANDROID 1
#elif defined(__linux__)
    #define UT_PLATFORM_LINUX 1
#elif defined(__FreeBSD__)
    #define UT_PLATFORM_FREEBSD 1
#elif defined(__OpenBSD__)
    #define UT_PLATFORM_OPENBSD 1
#elif defined(__NetBSD__)
    #define UT_PLATFORM_NETBSD 1
#elif defined(__DragonFly__)
    #define UT_PLATFORM_DRAGONFLY 1
#elif defined(__sun) && defined(__SVR4)
    #define UT_PLATFORM_SOLARIS 1
#elif defined(_AIX)
    #define UT_PLATFORM_AIX 1
#elif defined(__hpux)
    #define UT_PLATFORM_HPUX 1
#elif defined(__QNX__) || defined(__QNXNTO__)
    #define UT_PLATFORM_QNX 1
#elif defined(__HAIKU__)
    #define UT_PLATFORM_HAIKU 1
#elif defined(__CYGWIN__)
    #define UT_PLATFORM_CYGWIN 1
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #define UT_PLATFORM_MINGW 1
#elif defined(__vxworks)
    #define UT_PLATFORM_VXWORKS 1
#elif defined(__Fuchsia__)
    #define UT_PLATFORM_FUCHSIA 1
#endif

/* POSIX availability check */
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L
    #define UT_HAS_POSIX_CLOCK 1
#elif defined(UT_PLATFORM_APPLE) || defined(UT_PLATFORM_LINUX) || \
      defined(UT_PLATFORM_FREEBSD) || defined(UT_PLATFORM_OPENBSD) || \
      defined(UT_PLATFORM_NETBSD) || defined(UT_PLATFORM_DRAGONFLY) || \
      defined(UT_PLATFORM_ANDROID) || defined(UT_PLATFORM_SOLARIS) || \
      defined(UT_PLATFORM_AIX) || defined(UT_PLATFORM_HPUX) || \
      defined(UT_PLATFORM_QNX) || defined(UT_PLATFORM_HAIKU) || \
      defined(UT_PLATFORM_CYGWIN) || defined(UT_PLATFORM_VXWORKS) || \
      defined(UT_PLATFORM_FUCHSIA)
    #define UT_HAS_POSIX_CLOCK 1
#endif

static atomic_int_fast64_t g_last_monotonic = ATOMIC_VAR_INIT(0);
static ut_regression_callback_t g_regression_callback = NULL;

#if defined(UT_PLATFORM_WINDOWS)
/* Windows epoch (1601-01-01) to Unix epoch (1970-01-01) in 100-nanosecond intervals */
static const int64_t WINDOWS_TICK = 10000000LL;
static const int64_t WINDOWS_TO_UNIX_EPOCH = 11644473600LL;
#endif

/**
 * @brief Get the current UTC timestamp.
 */
ut_timestamp_t ut_now(void) {
    ut_timestamp_t ts;

#if defined(UT_PLATFORM_WINDOWS)
    FILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);
    int64_t wintime = ((int64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    int64_t unix_100ns = wintime - (WINDOWS_TO_UNIX_EPOCH * WINDOWS_TICK);
    ts.nanos = unix_100ns * 100LL;

#elif defined(UT_PLATFORM_EMSCRIPTEN)
    double ms = emscripten_get_now();
    ts.nanos = (int64_t)(ms * 1000000.0);

#elif defined(UT_HAS_POSIX_CLOCK)
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
