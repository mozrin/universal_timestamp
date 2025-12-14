/**
 * Core date/time calculation utilities.
 */

#include "ut_internal.h"

static const int DAYS_IN_MONTH[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const int64_t NANOS_PER_SECOND = 1000000000LL;
static const int64_t SECONDS_PER_MINUTE = 60LL;
static const int64_t SECONDS_PER_HOUR = 3600LL;
static const int64_t SECONDS_PER_DAY = 86400LL;

/* Returns true if year is a leap year. */
bool ut_internal_is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/* Returns days in month for given year. */
int ut_internal_days_in_month(int year, int month) {
    if (month < 1 || month > 12) return 0;
    if (month == 2 && ut_internal_is_leap_year(year)) return 29;
    return DAYS_IN_MONTH[month];
}

/* Validates a calendar date. Returns true if valid. */
bool ut_internal_validate_date(int year, int month, int day) {
    if (year < 0 || year > 9999) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > ut_internal_days_in_month(year, month)) return false;
    return true;
}

/* Days from epoch (1970-01-01) to given date. */
static int64_t days_from_epoch(int year, int month, int day) {
    int64_t days = 0;
    
    if (year >= 1970) {
        for (int y = 1970; y < year; y++) {
            days += ut_internal_is_leap_year(y) ? 366 : 365;
        }
    } else {
        for (int y = year; y < 1970; y++) {
            days -= ut_internal_is_leap_year(y) ? 366 : 365;
        }
    }
    
    for (int m = 1; m < month; m++) {
        days += ut_internal_days_in_month(year, m);
    }
    
    days += day - 1;
    return days;
}

/* Converts broken-down time to nanoseconds since epoch. */
int64_t ut_internal_to_nanos(int year, int month, int day,
                              int hour, int minute, int second,
                              int64_t frac_nanos) {
    int64_t days = days_from_epoch(year, month, day);
    int64_t seconds = days * SECONDS_PER_DAY
                      + (int64_t)hour * SECONDS_PER_HOUR
                      + (int64_t)minute * SECONDS_PER_MINUTE
                      + second;
    return seconds * NANOS_PER_SECOND + frac_nanos;
}

/* Converts nanoseconds since epoch to broken-down time. */
void ut_internal_from_nanos(int64_t nanos,
                             int *year, int *month, int *day,
                             int *hour, int *minute, int *second,
                             int *frac_nanos) {
    int64_t total_seconds = nanos / NANOS_PER_SECOND;
    int64_t remaining_nanos = nanos % NANOS_PER_SECOND;
    
    if (remaining_nanos < 0) {
        remaining_nanos += NANOS_PER_SECOND;
        total_seconds--;
    }
    
    *frac_nanos = (int)remaining_nanos;
    
    int64_t days = total_seconds / SECONDS_PER_DAY;
    int64_t day_seconds = total_seconds % SECONDS_PER_DAY;
    
    if (day_seconds < 0) {
        day_seconds += SECONDS_PER_DAY;
        days--;
    }
    
    *hour = (int)(day_seconds / SECONDS_PER_HOUR);
    *minute = (int)((day_seconds % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE);
    *second = (int)(day_seconds % SECONDS_PER_MINUTE);
    
    int y = 1970;
    if (days >= 0) {
        while (1) {
            int days_in_year = ut_internal_is_leap_year(y) ? 366 : 365;
            if (days < days_in_year) break;
            days -= days_in_year;
            y++;
        }
    } else {
        while (days < 0) {
            y--;
            int days_in_year = ut_internal_is_leap_year(y) ? 366 : 365;
            days += days_in_year;
        }
    }
    *year = y;
    
    int m = 1;
    while (1) {
        int days_in_month = ut_internal_days_in_month(y, m);
        if (days < days_in_month) break;
        days -= days_in_month;
        m++;
    }
    *month = m;
    *day = (int)days + 1;
}

/* Parses an integer of exactly n digits. Returns -1 on error. */
int ut_internal_parse_int(const char *str, int n) {
    int val = 0;
    for (int i = 0; i < n; i++) {
        char c = str[i];
        if (c < '0' || c > '9') return -1;
        val = val * 10 + (c - '0');
    }
    return val;
}

/* Parses fractional seconds. Returns nanoseconds or -1 on error. */
int64_t ut_internal_parse_fraction(const char *str, int len) {
    if (len < 1 || len > 9) return -1;
    
    int64_t val = 0;
    for (int i = 0; i < len; i++) {
        char c = str[i];
        if (c < '0' || c > '9') return -1;
        val = val * 10 + (c - '0');
    }
    
    static const int64_t multipliers[] = {
        100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1
    };
    return val * multipliers[len - 1];
}
