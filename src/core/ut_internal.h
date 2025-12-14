/**
 * Internal header for Universal Timestamp private functions.
 */

#ifndef UT_INTERNAL_H
#define UT_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>

/* Converts broken-down time to nanoseconds since epoch. */
int64_t ut_internal_to_nanos(int year, int month, int day,
                              int hour, int minute, int second,
                              int64_t frac_nanos);

/* Converts nanoseconds since epoch to broken-down time. */
void ut_internal_from_nanos(int64_t nanos,
                             int *year, int *month, int *day,
                             int *hour, int *minute, int *second,
                             int *frac_nanos);

/* Validates a calendar date. Returns true if valid. */
bool ut_internal_validate_date(int year, int month, int day);

/* Returns true if year is a leap year. */
bool ut_internal_is_leap_year(int year);

/* Returns days in month for given year. */
int ut_internal_days_in_month(int year, int month);

/* Parses an integer of exactly n digits. Returns -1 on error. */
int ut_internal_parse_int(const char *str, int n);

/* Parses fractional seconds. Returns nanoseconds or -1 on error. */
int64_t ut_internal_parse_fraction(const char *str, int len);

#endif /* UT_INTERNAL_H */
