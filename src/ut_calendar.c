/**
 * @file ut_calendar.c
 * @brief Implementation of calendar conversion functions.
 */


#include "universal_timestamp.h"
#include "core/ut_internal.h"

#define THAI_OFFSET    543
#define DANGI_OFFSET   2333
#define MINGUO_OFFSET  1911

/**
 * @brief Convert Gregorian year to Thai Buddhist Era year.
 */

int ut_gregorian_to_thai(int gregorian_year) {
    return gregorian_year + THAI_OFFSET;
}

/**
 * @brief Convert Thai Buddhist Era year to Gregorian year.
 */

int ut_thai_to_gregorian(int thai_year) {
    return thai_year - THAI_OFFSET;
}

/**
 * @brief Convert Gregorian year to Korean Dangi year.
 */

int ut_gregorian_to_dangi(int gregorian_year) {
    return gregorian_year + DANGI_OFFSET;
}

/**
 * @brief Convert Korean Dangi year to Gregorian year.
 */

int ut_dangi_to_gregorian(int dangi_year) {
    return dangi_year - DANGI_OFFSET;
}

/**
 * @brief Convert Gregorian year to Minguo (ROC) year.
 */

int ut_gregorian_to_minguo(int gregorian_year) {
    return gregorian_year - MINGUO_OFFSET;
}

/**
 * @brief Convert Minguo (ROC) year to Gregorian year.
 */

int ut_minguo_to_gregorian(int minguo_year) {
    return minguo_year + MINGUO_OFFSET;
}

static const struct {
    ut_japanese_era_t era;
    int start_year;
    int start_month;
    int start_day;
    const char *name;
} JAPANESE_ERAS[] = {
    {UT_ERA_REIWA,  2019, 5,  1,  "Reiwa"},
    {UT_ERA_HEISEI, 1989, 1,  8,  "Heisei"},
    {UT_ERA_SHOWA,  1926, 12, 25, "Showa"},
    {UT_ERA_TAISHO, 1912, 7,  30, "Taisho"},
    {UT_ERA_MEIJI,  1868, 1,  25, "Meiji"}
};

#define NUM_ERAS (sizeof(JAPANESE_ERAS) / sizeof(JAPANESE_ERAS[0]))

/**
 * @brief Get Japanese era and year for a given timestamp.
 */

ut_error_t ut_to_japanese_era(ut_timestamp_t ts, ut_japanese_era_t *era, int *era_year) {
    if (era == NULL || era_year == NULL) {
        return UT_ERR_NULL_POINTER;
    }
    
    int year, month, day, hour, minute, second, frac;
    ut_internal_from_nanos(ts.nanos, &year, &month, &day, &hour, &minute, &second, &frac);
    
    for (size_t i = 0; i < NUM_ERAS; i++) {
        int sy = JAPANESE_ERAS[i].start_year;
        int sm = JAPANESE_ERAS[i].start_month;
        int sd = JAPANESE_ERAS[i].start_day;
        
        bool after_start = (year > sy) ||
                          (year == sy && month > sm) ||
                          (year == sy && month == sm && day >= sd);
        
        if (after_start) {
            *era = JAPANESE_ERAS[i].era;
            *era_year = year - sy + 1;
            return UT_OK;
        }
    }
    
    return UT_ERR_OUT_OF_RANGE;
}

/**
 * @brief Get the name of a Japanese era.
 */

const char *ut_japanese_era_name(ut_japanese_era_t era) {
    for (size_t i = 0; i < NUM_ERAS; i++) {
        if (JAPANESE_ERAS[i].era == era) {
            return JAPANESE_ERAS[i].name;
        }
    }
    return "Unknown";
}

/* Calculates day of week (0=Monday, 6=Sunday) from timestamp. */
static int day_of_week_from_nanos(int64_t nanos) {
    int64_t days = nanos / (86400LL * 1000000000LL);
    int dow = (int)((days + 3) % 7);
    if (dow < 0) dow += 7;
    return dow;
}

/**
 * @brief Get ISO week date components from a timestamp.
 */

void ut_to_iso_week(ut_timestamp_t ts, int *year, int *week, int *day) {
    if (year == NULL || week == NULL || day == NULL) {
        return;
    }
    
    int y, m, d, hour, minute, second, frac;
    ut_internal_from_nanos(ts.nanos, &y, &m, &d, &hour, &minute, &second, &frac);
    
    int dow = day_of_week_from_nanos(ts.nanos);
    *day = dow + 1;
    
    int day_of_year = 0;
    for (int i = 1; i < m; i++) {
        day_of_year += ut_internal_days_in_month(y, i);
    }
    day_of_year += d;
    
    int thursday_doy = day_of_year + (3 - dow);
    
    int iso_year = y;
    if (thursday_doy < 1) {
        iso_year = y - 1;
        int prev_year_days = ut_internal_is_leap_year(iso_year) ? 366 : 365;
        thursday_doy += prev_year_days;
    } else {
        int this_year_days = ut_internal_is_leap_year(y) ? 366 : 365;
        if (thursday_doy > this_year_days) {
            iso_year = y + 1;
            thursday_doy -= this_year_days;
        }
    }
    
    *year = iso_year;
    *week = (thursday_doy + 6) / 7;
}
