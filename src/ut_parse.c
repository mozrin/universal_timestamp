/**
 * @file ut_parse.c
 * @brief Implementation of ut_parse_strict() and ut_parse_lenient() functions.
 */


#include "universal_timestamp.h"
#include "core/ut_internal.h"
#include <string.h>

static ut_error_t parse_timestamp(const char *str, ut_timestamp_t *out, bool strict) {
    if (str == NULL || out == NULL) {
        return UT_ERR_NULL_POINTER;
    }
    
    size_t len = strlen(str);
    if (len < 19) {
        return UT_ERR_INVALID_FORMAT;
    }
    
    if (str[4] != '-' || str[7] != '-' || str[10] != 'T' ||
        str[13] != ':' || str[16] != ':') {
        return UT_ERR_INVALID_FORMAT;
    }
    
    int year = ut_internal_parse_int(str, 4);
    int month = ut_internal_parse_int(str + 5, 2);
    int day = ut_internal_parse_int(str + 8, 2);
    int hour = ut_internal_parse_int(str + 11, 2);
    int minute = ut_internal_parse_int(str + 14, 2);
    int second = ut_internal_parse_int(str + 17, 2);
    
    if (year < 0 || month < 0 || day < 0 || 
        hour < 0 || minute < 0 || second < 0) {
        return UT_ERR_INVALID_FORMAT;
    }
    
    if (hour > 23 || minute > 59 || second > 59) {
        return UT_ERR_OUT_OF_RANGE;
    }
    
    if (second == 60) {
        return UT_ERR_LEAP_SECOND;
    }
    
    if (!ut_internal_validate_date(year, month, day)) {
        return UT_ERR_INVALID_DATE;
    }
    
    int64_t frac_nanos = 0;
    size_t pos = 19;
    
    if (pos < len && str[pos] == '.') {
        pos++;
        size_t frac_start = pos;
        while (pos < len && str[pos] >= '0' && str[pos] <= '9') {
            pos++;
        }
        int frac_len = (int)(pos - frac_start);
        
        if (frac_len == 0) {
            return UT_ERR_INVALID_FORMAT;
        }
        
        if (frac_len > 9) {
            if (strict) {
                return UT_ERR_FRACTION_TOO_LONG;
            }
            frac_len = 9;
        }
        
        frac_nanos = ut_internal_parse_fraction(str + frac_start, frac_len);
        if (frac_nanos < 0) {
            return UT_ERR_INVALID_FORMAT;
        }
    }
    
    if (pos < len) {
        char suffix = str[pos];
        
        if (suffix == 'Z') {
            pos++;
        } else if (suffix == 'z') {
            if (strict) {
                return UT_ERR_INVALID_FORMAT;
            }
            pos++;
        } else if (suffix == '+' || suffix == '-') {
            if (len - pos < 6) {
                return UT_ERR_INVALID_FORMAT;
            }
            if (str[pos + 3] != ':') {
                return UT_ERR_INVALID_FORMAT;
            }
            
            int off_hour = ut_internal_parse_int(str + pos + 1, 2);
            int off_min = ut_internal_parse_int(str + pos + 4, 2);
            
            if (off_hour < 0 || off_min < 0) {
                return UT_ERR_INVALID_FORMAT;
            }
            
            if (off_hour != 0 || off_min != 0) {
                return UT_ERR_UNSUPPORTED_OFFSET;
            }
            
            if (strict) {
                return UT_ERR_UNSUPPORTED_OFFSET;
            }
            
            pos += 6;
        } else {
            if (strict) {
                return UT_ERR_INVALID_FORMAT;
            }
        }
    } else {
        if (strict) {
            return UT_ERR_INVALID_FORMAT;
        }
    }
    
    if (pos != len) {
        return UT_ERR_INVALID_FORMAT;
    }
    
    out->nanos = ut_internal_to_nanos(year, month, day, hour, minute, second, frac_nanos);
    return UT_OK;
}

/**
 * @brief Parse a timestamp string in strict mode.
 */

ut_error_t ut_parse_strict(const char *str, ut_timestamp_t *out) {
    return parse_timestamp(str, out, true);
}

/**
 * @brief Parse a timestamp string in lenient mode.
 */

ut_error_t ut_parse_lenient(const char *str, ut_timestamp_t *out) {
    return parse_timestamp(str, out, false);
}
