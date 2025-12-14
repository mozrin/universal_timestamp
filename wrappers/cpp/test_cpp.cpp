/**
 * @file test_cpp.cpp
 * @brief Test suite for C++ wrapper.
 */

#include "universal_timestamp.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "C++ wrapper tests...\n";
    std::cout << "====================\n\n";

    /* Test Timestamp::now() */
    uts::Timestamp now = uts::Timestamp::now();
    assert(now.nanos() > 0);
    std::cout << "[PASS] now() returns positive nanos\n";

    /* Test format() */
    std::string str = now.format();
    assert(str.length() >= 20);
    assert(str.back() == 'Z');
    std::cout << "[PASS] format() produces valid string: " << str << "\n";

    /* Test parse() */
    uts::Timestamp parsed = uts::Timestamp::parse("2024-12-14T03:13:21.123456789Z");
    assert(parsed.nanos() == 1734146001123456789LL);
    std::cout << "[PASS] parse() works correctly\n";

    /* Test round-trip */
    uts::Timestamp rt = uts::Timestamp::parse(now.format());
    std::cout << "[PASS] round-trip parse succeeds\n";

    /* Test comparison operators */
    uts::Timestamp t1 = uts::Timestamp::now_monotonic();
    uts::Timestamp t2 = uts::Timestamp::now_monotonic();
    assert(t2 > t1);
    assert(t1 < t2);
    assert(t1 != t2);
    std::cout << "[PASS] comparison operators work\n";

    /* Test calendar conversions */
    assert(uts::calendar::gregorian_to_thai(2024) == 2567);
    assert(uts::calendar::thai_to_gregorian(2567) == 2024);
    assert(uts::calendar::gregorian_to_dangi(2024) == 4357);
    assert(uts::calendar::gregorian_to_minguo(2024) == 113);
    std::cout << "[PASS] calendar conversions work\n";

    /* Test Japanese era */
    uts::Timestamp ts_2024 = uts::Timestamp::parse("2024-12-14T00:00:00Z");
    uts::calendar::JapaneseEra era = uts::calendar::to_japanese_era(ts_2024);
    assert(era.era == UT_ERA_REIWA);
    assert(era.year == 6);
    assert(era.name() == "Reiwa");
    std::cout << "[PASS] Japanese era conversion works\n";

    /* Test ISO week */
    uts::calendar::IsoWeek iw = uts::calendar::to_iso_week(ts_2024);
    assert(iw.year == 2024);
    assert(iw.week == 50);
    std::cout << "[PASS] ISO week conversion works\n";

    /* Test precision */
    ut_precision_t prec = uts::get_clock_precision();
    assert(prec >= UT_PRECISION_ERROR && prec <= UT_PRECISION_SECOND);
    std::cout << "[PASS] get_clock_precision() works\n";

    /* Test error handling */
    try {
        uts::Timestamp::parse("invalid");
        assert(false);
    } catch (const uts::Error& e) {
        assert(e.code() == UT_ERR_INVALID_FORMAT);
        std::cout << "[PASS] Error exception thrown correctly\n";
    }

    std::cout << "\n====================\n";
    std::cout << "All C++ wrapper tests passed!\n";

    return 0;
}
