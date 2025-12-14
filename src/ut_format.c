/**
 * @file ut_format.c
 * @brief Implementation of ut_format() function.
 */


#include "universal_timestamp.h"
#include "core/ut_internal.h"
#include <stdio.h>

/**
 * @brief Format a timestamp to an ISO-8601 string.
 */

 
int ut_format(ut_timestamp_t ts, char *buf, size_t buf_size, bool include_nanos) {
    if (buf == NULL || buf_size < UT_MAX_STRING_LEN) {
        return -1;
    }
    
    int year, month, day, hour, minute, second, frac_nanos;
    ut_internal_from_nanos(ts.nanos, &year, &month, &day, 
                           &hour, &minute, &second, &frac_nanos);
    
    int len;
    
    if (include_nanos && frac_nanos > 0) {
        char frac_buf[10];
        int frac_temp = frac_nanos;
        for (int i = 8; i >= 0; i--) {
            frac_buf[i] = '0' + (frac_temp % 10);
            frac_temp /= 10;
        }
        frac_buf[9] = '\0';
        
        int digits = 9;
        while (digits > 1 && frac_buf[digits - 1] == '0') {
            digits--;
        }
        frac_buf[digits] = '\0';
        
        len = snprintf(buf, buf_size, "%04d-%02d-%02dT%02d:%02d:%02d.%sZ",
                       year, month, day, hour, minute, second, frac_buf);
    } else {
        len = snprintf(buf, buf_size, "%04d-%02d-%02dT%02d:%02d:%02dZ",
                       year, month, day, hour, minute, second);
    }
    
    return len;
}
