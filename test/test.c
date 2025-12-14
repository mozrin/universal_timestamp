#include "universal_timestamp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT(msg, expr) do { \
    tests_run++; \
    if (!(expr)) { \
        tests_failed++; \
        printf("[FAIL] %s (line %d)\n", msg, __LINE__); \
    } else { \
        printf("[PASS] %s\n", msg); \
    } \
} while (0)

#define ASSERT_EQ_INT(msg, a, b) ASSERT(msg, (a) == (b))
#define ASSERT_EQ_STR(msg, a, b) ASSERT(msg, strcmp((a), (b)) == 0)

static void test_now_basic(void) {
    printf("\n--- test_now_basic ---\n");
    
    ut_timestamp_t ts = ut_now();
    ASSERT("nanos > 0", ts.nanos > 0);
    
    char buf[UT_MAX_STRING_LEN];
    int len = ut_format(ts, buf, sizeof(buf), false);
    ASSERT("format succeeds", len > 0);
    printf("  Current time: %s\n", buf);
}

static void test_format_basic(void) {
    printf("\n--- test_format_basic ---\n");
    
    char buf[UT_MAX_STRING_LEN];
    
    ut_timestamp_t epoch = ut_from_unix_nanos(0);
    ut_format(epoch, buf, sizeof(buf), false);
    ASSERT_EQ_STR("epoch formats correctly", buf, "1970-01-01T00:00:00Z");
    
    ut_timestamp_t ts1 = ut_from_unix_nanos(1000000000000000000LL);
    ut_format(ts1, buf, sizeof(buf), false);
    ASSERT_EQ_STR("2001-09-09 formats correctly", buf, "2001-09-09T01:46:40Z");
    
    ut_timestamp_t ts2 = ut_from_unix_nanos(1734146001123456789LL);
    ut_format(ts2, buf, sizeof(buf), true);
    ASSERT_EQ_STR("nanos format correctly", buf, "2024-12-14T03:13:21.123456789Z");
}

static void test_parse_strict(void) {
    printf("\n--- test_parse_strict ---\n");
    
    ut_timestamp_t ts;
    ut_error_t err;
    
    err = ut_parse_strict("1970-01-01T00:00:00Z", &ts);
    ASSERT("parse epoch succeeds", err == UT_OK);
    ASSERT_EQ_INT("epoch nanos = 0", ts.nanos, 0);
    
    err = ut_parse_strict("2024-12-14T03:13:21Z", &ts);
    ASSERT("parse timestamp succeeds", err == UT_OK);
    ASSERT_EQ_INT("parsed nanos correct", ts.nanos, 1734146001000000000LL);
    
    err = ut_parse_strict("2024-12-14T03:13:21.5Z", &ts);
    ASSERT("parse with fraction succeeds", err == UT_OK);
    ASSERT_EQ_INT("fractional nanos correct", ts.nanos, 1734146001500000000LL);
    
    err = ut_parse_strict("2024-12-14T03:13:21", &ts);
    ASSERT("missing Z rejected", err == UT_ERR_INVALID_FORMAT);
    
    err = ut_parse_strict("2024-02-30T00:00:00Z", &ts);
    ASSERT("invalid date rejected", err == UT_ERR_INVALID_DATE);
    
    err = ut_parse_strict("2024-12-14T03:13:21z", &ts);
    ASSERT("lowercase z rejected", err == UT_ERR_INVALID_FORMAT);
    
    err = ut_parse_strict("2024-12-14T03:13:21+00:00", &ts);
    ASSERT("offset rejected in strict", err == UT_ERR_UNSUPPORTED_OFFSET);
}

static void test_parse_lenient(void) {
    printf("\n--- test_parse_lenient ---\n");
    
    ut_timestamp_t ts;
    ut_error_t err;
    
    err = ut_parse_lenient("2024-12-14T03:13:21", &ts);
    ASSERT("missing Z allowed", err == UT_OK);
    ASSERT_EQ_INT("parsed nanos correct", ts.nanos, 1734146001000000000LL);
    
    err = ut_parse_lenient("2024-12-14T03:13:21+00:00", &ts);
    ASSERT("+00:00 allowed", err == UT_OK);
    
    err = ut_parse_lenient("2024-02-30T00:00:00Z", &ts);
    ASSERT("invalid date still rejected", err == UT_ERR_INVALID_DATE);
}

static void test_round_trip(void) {
    printf("\n--- test_round_trip ---\n");
    
    ut_timestamp_t original = ut_now();
    
    char buf[UT_MAX_STRING_LEN];
    ut_format(original, buf, sizeof(buf), true);
    
    ut_timestamp_t parsed;
    ut_error_t err = ut_parse_strict(buf, &parsed);
    
    ASSERT("round-trip parse succeeds", err == UT_OK);
    ASSERT_EQ_INT("round-trip preserves nanos", original.nanos, parsed.nanos);
}

static void test_monotonic(void) {
    printf("\n--- test_monotonic ---\n");
    
    ut_timestamp_t t1 = ut_now_monotonic();
    ut_timestamp_t t2 = ut_now_monotonic();
    ut_timestamp_t t3 = ut_now_monotonic();
    
    ASSERT("t2 > t1", t2.nanos > t1.nanos);
    ASSERT("t3 > t2", t3.nanos > t2.nanos);
}

static void test_conversions(void) {
    printf("\n--- test_conversions ---\n");
    
    int64_t nanos = 1734146001123456789LL;
    ut_timestamp_t ts = ut_from_unix_nanos(nanos);
    ASSERT_EQ_INT("from_unix_nanos", ts.nanos, nanos);
    
    int64_t back = ut_to_unix_nanos(ts);
    ASSERT_EQ_INT("to_unix_nanos", back, nanos);
}

static void test_error_strings(void) {
    printf("\n--- test_error_strings ---\n");
    
    ASSERT("OK string", strcmp(ut_error_string(UT_OK), "Success") == 0);
    ASSERT("INVALID_FORMAT string", strlen(ut_error_string(UT_ERR_INVALID_FORMAT)) > 0);
    ASSERT("INVALID_DATE string", strlen(ut_error_string(UT_ERR_INVALID_DATE)) > 0);
}

static void test_calendar(void) {
    printf("\n--- test_calendar ---\n");
    
    ASSERT("calendar is gregorian", ut_get_calendar() == UT_CALENDAR_GREGORIAN);
    
    ut_timestamp_t ts;
    ut_error_t err;
    
    err = ut_parse_strict("2000-02-29T00:00:00Z", &ts);
    ASSERT("2000 leap year valid", err == UT_OK);
    
    err = ut_parse_strict("1900-02-29T00:00:00Z", &ts);
    ASSERT("1900 not leap year", err == UT_ERR_INVALID_DATE);
    
    err = ut_parse_strict("2024-02-29T00:00:00Z", &ts);
    ASSERT("2024 leap year valid", err == UT_OK);
    
    err = ut_parse_strict("2023-02-29T00:00:00Z", &ts);
    ASSERT("2023 not leap year", err == UT_ERR_INVALID_DATE);
    
    ASSERT_EQ_INT("thai 2024", ut_gregorian_to_thai(2024), 2567);
    ASSERT_EQ_INT("thai inverse", ut_thai_to_gregorian(2567), 2024);
    
    ASSERT_EQ_INT("dangi 2024", ut_gregorian_to_dangi(2024), 4357);
    ASSERT_EQ_INT("dangi inverse", ut_dangi_to_gregorian(4357), 2024);
    
    ASSERT_EQ_INT("minguo 2024", ut_gregorian_to_minguo(2024), 113);
    ASSERT_EQ_INT("minguo inverse", ut_minguo_to_gregorian(113), 2024);
    
    ut_timestamp_t ts_2024 = ut_from_unix_nanos(1734146001000000000LL);
    ut_japanese_era_t era;
    int era_year;
    err = ut_to_japanese_era(ts_2024, &era, &era_year);
    ASSERT("japanese era ok", err == UT_OK);
    ASSERT("japanese era reiwa", era == UT_ERA_REIWA);
    ASSERT_EQ_INT("japanese era year 6", era_year, 6);
    ASSERT_EQ_STR("era name reiwa", ut_japanese_era_name(UT_ERA_REIWA), "Reiwa");
    
    int iso_y, iso_w, iso_d;
    ut_to_iso_week(ts_2024, &iso_y, &iso_w, &iso_d);
    ASSERT_EQ_INT("iso week year", iso_y, 2024);
    ASSERT_EQ_INT("iso week num", iso_w, 50);
    ASSERT_EQ_INT("iso week day", iso_d, 6);
}

static void test_precision(void) {
    printf("\n--- test_precision ---\n");
    
    ut_precision_t prec = ut_get_clock_precision();
    ASSERT("precision valid", prec >= UT_PRECISION_ERROR && prec <= UT_PRECISION_SECOND);
    ASSERT("precision not error", prec != UT_PRECISION_ERROR);
    
    printf("  Detected precision: %d (0=ns, 1=us, 2=ms, 3=s)\n", prec);
}

int main(void) {
    printf("Running universal_timestamp tests...\n");
    printf("=====================================\n");

    test_now_basic();
    test_format_basic();
    test_parse_strict();
    test_parse_lenient();
    test_round_trip();
    test_monotonic();
    test_conversions();
    test_error_strings();
    test_calendar();
    test_precision();

    printf("\n=====================================\n");
    printf("Tests run: %d\n", tests_run);
    printf("Failures : %d\n", tests_failed);

    if (tests_failed == 0) {
        printf("\n✅ All tests passed.\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed.\n");
        return 1;
    }
}
