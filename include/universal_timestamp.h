/**
 * @file universal_timestamp.h
 * @brief Universal Timestamp Library - Deterministic, cross-platform timestamps.
 * @version 1.0.0
 *
 * This library provides deterministic timestamp generation and parsing
 * conforming to the Universal Timestamp Specification (UTS).
 *
 * Features:
 * - Always UTC, always ISO-8601
 * - Nanosecond precision
 * - Strict and lenient parsing modes
 * - Monotonic timestamp generation
 * - Zero external dependencies
 */

#ifndef UNIVERSAL_TIMESTAMP_H
#define UNIVERSAL_TIMESTAMP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum length of a formatted timestamp string.
 *
 * Format: YYYY-MM-DDTHH:MM:SS.nnnnnnnnnZ + null terminator
 *         = 4+1+2+1+2+1+2+1+2+1+2+1+9+1+1 = 31 bytes
 */

#define UT_MAX_STRING_LEN 32

/**
 * @brief Error codes for parsing and validation operations.
 */

typedef enum {
    UT_OK = 0,                    /**< Success */
    UT_ERR_INVALID_FORMAT,        /**< String doesn't match expected pattern */
    UT_ERR_INVALID_DATE,          /**< Calendar date doesn't exist */
    UT_ERR_OUT_OF_RANGE,          /**< Component value outside valid range */
    UT_ERR_UNSUPPORTED_OFFSET,    /**< Non-zero timezone offset in strict mode */
    UT_ERR_FRACTION_TOO_LONG,     /**< More than 9 fractional digits */
    UT_ERR_LEAP_SECOND,           /**< Leap second (SS=60) not supported */
    UT_ERR_NULL_POINTER           /**< Null pointer argument */
} ut_error_t;

/**
 * @brief Calendar type for formatting and parsing.
 *
 * All calendars share the same internal representation (nanoseconds since epoch).
 * Calendar type only affects how dates are displayed and parsed.
 */

typedef enum {
    UT_CALENDAR_GREGORIAN = 0,    /**< Proleptic Gregorian calendar (default, ISO-8601) */
    UT_CALENDAR_THAI,             /**< Thai Solar Calendar (Buddhist Era, +543 years) */
    UT_CALENDAR_DANGI,            /**< Korean Dangi Calendar (+2333 years) */
    UT_CALENDAR_MINGUO,           /**< Minguo/ROC Calendar (Taiwan, -1911 years) */
    UT_CALENDAR_JAPANESE,         /**< Japanese Era Calendar (Gengō) */
    UT_CALENDAR_ISO_WEEK          /**< ISO week-date format (YYYY-Www-D) */
} ut_calendar_t;

/**
 * @brief Japanese era identifier for era-based year formatting.
 */

typedef enum {
    UT_ERA_REIWA = 0,             /**< Reiwa era (2019-05-01 onwards) */
    UT_ERA_HEISEI,                /**< Heisei era (1989-01-08 to 2019-04-30) */
    UT_ERA_SHOWA,                 /**< Shōwa era (1926-12-25 to 1989-01-07) */
    UT_ERA_TAISHO,                /**< Taishō era (1912-07-30 to 1926-12-24) */
    UT_ERA_MEIJI                  /**< Meiji era (1868-01-25 to 1912-07-29) */
} ut_japanese_era_t;

/**
 * @brief Clock precision levels detected at runtime.
 *
 * Lower values indicate higher precision. Negative values indicate errors.
 */

typedef enum {
    UT_PRECISION_NANOSECOND = 0,  /**< Full nanosecond precision */
    UT_PRECISION_MICROSECOND = 1, /**< Microsecond precision (last 3 digits always 0) */
    UT_PRECISION_MILLISECOND = 2, /**< Millisecond precision (last 6 digits always 0) */
    UT_PRECISION_SECOND = 3,      /**< Second precision only */
    UT_PRECISION_ERROR = -1       /**< Unable to determine precision */
} ut_precision_t;

/**
 * @brief Timestamp structure storing nanoseconds since Unix epoch.
 *
 * The internal representation is a signed 64-bit integer representing
 * nanoseconds since 1970-01-01T00:00:00Z. This allows representation
 * of dates from approximately 1677 to 2262.
 */

typedef struct {
    int64_t nanos;  /**< Nanoseconds since Unix epoch (1970-01-01T00:00:00Z) */
} ut_timestamp_t;

/**
 * @brief Callback type for clock regression detection.
 *
 * Called when the monotonic generator detects that the system clock
 * has moved backwards.
 *
 * @param expected  The expected minimum timestamp (last + 1ns)
 * @param actual    The actual system time observed
 * @param adjusted  The adjusted timestamp that will be returned
 */

typedef void (*ut_regression_callback_t)(
    ut_timestamp_t expected,
    ut_timestamp_t actual,
    ut_timestamp_t adjusted
);

/**
 * @brief Get the current UTC timestamp.
 *
 * Returns the current system time as a UTC timestamp with nanosecond
 * precision (where available). This function does not provide monotonic
 * guarantees - use ut_now_monotonic() if ordering is required.
 *
 * @return Current UTC timestamp.
 *
 * @code
 * ut_timestamp_t ts = ut_now();
 * char buf[UT_MAX_STRING_LEN];
 * ut_format(ts, buf, sizeof(buf), true);
 * printf("Current time: %s\n", buf);
 * @endcode
 */

ut_timestamp_t ut_now(void);

/**
 * @brief Get the current UTC timestamp with monotonic guarantee.
 *
 * Returns a timestamp that is guaranteed to be strictly greater than
 * any previously returned timestamp from this function. If the system
 * clock moves backwards, the timestamp is synthesized by incrementing
 * the last known timestamp by 1 nanosecond.
 *
 * Thread-safe: Uses atomic operations for the internal counter.
 *
 * @return Monotonically increasing UTC timestamp.
 *
 * @note The monotonic state resets when the process restarts.
 *
 * @code
 * ut_timestamp_t t1 = ut_now_monotonic();
 * ut_timestamp_t t2 = ut_now_monotonic();
 * assert(t2.nanos > t1.nanos);  // Always true
 * @endcode
 */

ut_timestamp_t ut_now_monotonic(void);

/**
 * @brief Set a callback for clock regression events.
 *
 * Registers a callback function that will be invoked whenever the
 * monotonic generator detects clock regression. Pass NULL to disable.
 *
 * @param callback  Function to call on clock regression, or NULL.
 *
 * @code
 * void my_handler(ut_timestamp_t exp, ut_timestamp_t act, ut_timestamp_t adj) {
 *     fprintf(stderr, "Clock went backwards!\n");
 * }
 * ut_set_regression_callback(my_handler);
 * @endcode
 */

void ut_set_regression_callback(ut_regression_callback_t callback);

/**
 * @brief Format a timestamp to an ISO-8601 string.
 *
 * Formats the timestamp to the canonical UTS format:
 * - Without nanoseconds: YYYY-MM-DDTHH:MM:SSZ
 * - With nanoseconds: YYYY-MM-DDTHH:MM:SS.nnnnnnnnnZ
 *
 * Nanoseconds are included only if non-zero and include_nanos is true.
 * Trailing zeros in fractional seconds are omitted.
 *
 * @param ts            Timestamp to format.
 * @param buf           Output buffer for the formatted string.
 * @param buf_size      Size of the output buffer (minimum UT_MAX_STRING_LEN).
 * @param include_nanos If true, include fractional seconds when non-zero.
 * @return Number of characters written (excluding null), or -1 on error.
 *
 * @code
 * ut_timestamp_t ts = ut_from_unix_nanos(1734147201123456789);
 * char buf[UT_MAX_STRING_LEN];
 * ut_format(ts, buf, sizeof(buf), true);
 * // buf now contains "2024-12-14T03:13:21.123456789Z"
 * @endcode
 */

int ut_format(ut_timestamp_t ts, char *buf, size_t buf_size, bool include_nanos);

/**
 * @brief Parse a timestamp string in strict mode.
 *
 * Strict parsing requires:
 * - Exact format: YYYY-MM-DDTHH:MM:SS[.nnnnnnnnn]Z
 * - Uppercase 'T' and 'Z'
 * - Valid calendar date
 * - No timezone offsets
 * - Maximum 9 fractional digits
 *
 * @param str    Null-terminated timestamp string to parse.
 * @param out    Pointer to store the parsed timestamp.
 * @return UT_OK on success, error code on failure.
 *
 * @code
 * ut_timestamp_t ts;
 * ut_error_t err = ut_parse_strict("2024-12-14T03:13:21Z", &ts);
 * if (err != UT_OK) {
 *     // Handle parse error
 * }
 * @endcode
 */
 
ut_error_t ut_parse_strict(const char *str, ut_timestamp_t *out);

/**
 * @brief Parse a timestamp string in lenient mode.
 *
 * Lenient parsing accepts:
 * - Missing 'Z' suffix (assumes UTC)
 * - Lowercase 'z'
 * - Zero offsets (+00:00, -00:00)
 * - Optional fractional seconds
 *
 * @param str    Null-terminated timestamp string to parse.
 * @param out    Pointer to store the parsed timestamp.
 * @return UT_OK on success, error code on failure.
 *
 * @code
 * ut_timestamp_t ts;
 * ut_error_t err = ut_parse_lenient("2024-12-14T03:13:21", &ts);
 * // Succeeds - missing Z is allowed in lenient mode
 * @endcode
 */

ut_error_t ut_parse_lenient(const char *str, ut_timestamp_t *out);

/**
 * @brief Create a timestamp from Unix nanoseconds.
 *
 * Converts a signed 64-bit integer representing nanoseconds since
 * the Unix epoch (1970-01-01T00:00:00Z) to a timestamp.
 *
 * @param nanos  Nanoseconds since Unix epoch.
 * @return Timestamp representing the given instant.
 *
 * @code
 * ut_timestamp_t epoch = ut_from_unix_nanos(0);
 * // Represents 1970-01-01T00:00:00Z
 * @endcode
 */

ut_timestamp_t ut_from_unix_nanos(int64_t nanos);

/**
 * @brief Convert a timestamp to Unix nanoseconds.
 *
 * Returns the number of nanoseconds since the Unix epoch
 * (1970-01-01T00:00:00Z) for the given timestamp.
 *
 * @param ts  Timestamp to convert.
 * @return Nanoseconds since Unix epoch.
 *
 * @code
 * ut_timestamp_t ts = ut_now();
 * int64_t nanos = ut_to_unix_nanos(ts);
 * @endcode
 */

int64_t ut_to_unix_nanos(ut_timestamp_t ts);

/**
 * @brief Get a human-readable error message.
 *
 * Returns a static string describing the given error code.
 *
 * @param err  Error code to describe.
 * @return Null-terminated error message string.
 */

const char *ut_error_string(ut_error_t err);

/**
 * @brief Get the calendar system used for date calculations.
 *
 * Returns the default calendar type. Internal calculations always use
 * Gregorian, but formatting/parsing can use other calendar types.
 *
 * @return The default calendar type (UT_CALENDAR_GREGORIAN).
 */

ut_calendar_t ut_get_calendar(void);

/**
 * @brief Detect the clock precision available on the current hardware.
 *
 * Samples the system clock multiple times to determine the actual
 * precision available. Lower return values indicate higher precision.
 *
 * @return Precision level:
 *         - UT_PRECISION_NANOSECOND (0): Full nanosecond precision
 *         - UT_PRECISION_MICROSECOND (1): Microsecond precision
 *         - UT_PRECISION_MILLISECOND (2): Millisecond precision
 *         - UT_PRECISION_SECOND (3): Second precision only
 *         - UT_PRECISION_ERROR (-1): Unable to determine
 *
 * @code
 * ut_precision_t prec = ut_get_clock_precision();
 * if (prec == UT_PRECISION_NANOSECOND) {
 *     printf("Full nanosecond precision available\n");
 * }
 * @endcode
 */

ut_precision_t ut_get_clock_precision(void);

/**
 * @brief Convert Gregorian year to Thai Buddhist Era year.
 *
 * Thai year = Gregorian year + 543
 *
 * @param gregorian_year  Gregorian year.
 * @return Thai Buddhist Era year.
 *
 * @code
 * int thai = ut_gregorian_to_thai(2024);  // Returns 2567
 * @endcode
 */

int ut_gregorian_to_thai(int gregorian_year);

/**
 * @brief Convert Thai Buddhist Era year to Gregorian year.
 *
 * Gregorian year = Thai year - 543
 *
 * @param thai_year  Thai Buddhist Era year.
 * @return Gregorian year.
 */

int ut_thai_to_gregorian(int thai_year);

/**
 * @brief Convert Gregorian year to Korean Dangi year.
 *
 * Dangi year = Gregorian year + 2333
 *
 * @param gregorian_year  Gregorian year.
 * @return Korean Dangi year.
 *
 * @code
 * int dangi = ut_gregorian_to_dangi(2024);  // Returns 4357
 * @endcode
 */

int ut_gregorian_to_dangi(int gregorian_year);

/**
 * @brief Convert Korean Dangi year to Gregorian year.
 *
 * Gregorian year = Dangi year - 2333
 *
 * @param dangi_year  Korean Dangi year.
 * @return Gregorian year.
 */

int ut_dangi_to_gregorian(int dangi_year);

/**
 * @brief Convert Gregorian year to Minguo (ROC) year.
 *
 * Minguo year = Gregorian year - 1911
 *
 * @param gregorian_year  Gregorian year.
 * @return Minguo (Republic of China) year.
 *
 * @code
 * int minguo = ut_gregorian_to_minguo(2024);  // Returns 113
 * @endcode
 */

int ut_gregorian_to_minguo(int gregorian_year);

/**
 * @brief Convert Minguo (ROC) year to Gregorian year.
 *
 * Gregorian year = Minguo year + 1911
 *
 * @param minguo_year  Minguo (Republic of China) year.
 * @return Gregorian year.
 */

int ut_minguo_to_gregorian(int minguo_year);

/**
 * @brief Get Japanese era and year for a given timestamp.
 *
 * Returns the Japanese era and year within that era for the given timestamp.
 *
 * @param ts        Timestamp to convert.
 * @param era       Pointer to store the era identifier.
 * @param era_year  Pointer to store the year within the era.
 * @return UT_OK on success, error code if date is before Meiji era.
 *
 * @code
 * ut_japanese_era_t era;
 * int year;
 * ut_to_japanese_era(ts, &era, &year);  // e.g., Reiwa 6
 * @endcode
 */

ut_error_t ut_to_japanese_era(ut_timestamp_t ts, ut_japanese_era_t *era, int *era_year);

/**
 * @brief Get the name of a Japanese era.
 *
 * @param era  Era identifier.
 * @return Era name in romaji (e.g., "Reiwa", "Heisei").
 */

const char *ut_japanese_era_name(ut_japanese_era_t era);

/**
 * @brief Get ISO week date components from a timestamp.
 *
 * Calculates the ISO week-numbering year, week number (1-53),
 * and day of week (1-7, Monday = 1) for the given timestamp.
 *
 * @param ts    Timestamp to convert.
 * @param year  Pointer to store ISO week-numbering year.
 * @param week  Pointer to store week number (1-53).
 * @param day   Pointer to store day of week (1-7).
 *
 * @code
 * int y, w, d;
 * ut_to_iso_week(ts, &y, &w, &d);  // e.g., 2024, 50, 6
 * @endcode
 */

void ut_to_iso_week(ut_timestamp_t ts, int *year, int *week, int *day);

#ifdef __cplusplus
}
#endif

#endif /* UNIVERSAL_TIMESTAMP_H */
