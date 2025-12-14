/**
 * @file universal_timestamp.hpp
 * @brief C++ wrapper for Universal Timestamp Library.
 * @version 0.9.0
 *
 * Provides idiomatic C++ classes wrapping the C API.
 */

#ifndef UNIVERSAL_TIMESTAMP_HPP
#define UNIVERSAL_TIMESTAMP_HPP

#include <string>
#include <stdexcept>
#include <cstdint>

extern "C" {
#include "universal_timestamp.h"
}

namespace uts {

/**
 * @brief Exception thrown for timestamp parsing/validation errors.
 */

class Error : public std::runtime_error {
public:
    explicit Error(ut_error_t code)
        : std::runtime_error(ut_error_string(code)), code_(code) {}

    ut_error_t code() const noexcept { return code_; }

private:
    ut_error_t code_;
};

/**
 * @brief Represents a UTC timestamp with nanosecond precision.
 */

class Timestamp {
public:

    /**
     * @brief Construct from Unix nanoseconds.
     */

    explicit Timestamp(int64_t nanos = 0) : ts_{nanos} {}

    /**
     * @brief Construct from the C struct.
     */

    explicit Timestamp(ut_timestamp_t ts) : ts_(ts) {}

    /**
     * @brief Get current UTC time.
     */

    static Timestamp now() {

        return Timestamp(ut_now());
    }

    /**
     * @brief Get current UTC time with monotonic guarantee.
     */

    static Timestamp now_monotonic() {

        return Timestamp(ut_now_monotonic());
    }

    /**
     * @brief Parse from ISO-8601 string (strict mode).
     * @throws Error on parse failure.
     */

    static Timestamp parse(const std::string& str) {

        ut_timestamp_t ts;
        ut_error_t err = ut_parse_strict(str.c_str(), &ts);

        if (err != UT_OK) {
            throw Error(err);
        }

        return Timestamp(ts);
    }

    /**
     * @brief Parse from ISO-8601 string (lenient mode).
     * @throws Error on parse failure.
     */

    static Timestamp parse_lenient(const std::string& str) {

        ut_timestamp_t ts;
        ut_error_t err = ut_parse_lenient(str.c_str(), &ts);

        if (err != UT_OK) {
            throw Error(err);
        }

        return Timestamp(ts);
    }

    /**
     * @brief Format to ISO-8601 string.
     */

    std::string format(bool include_nanos = true) const {
        char buf[UT_MAX_STRING_LEN];
        ut_format(ts_, buf, sizeof(buf), include_nanos);
        return std::string(buf);
    }

    /**
     * @brief Get underlying nanoseconds since epoch.
     */

    int64_t nanos() const noexcept { return ts_.nanos; }

    /**
     * @brief Get underlying C struct.
     */

    ut_timestamp_t raw() const noexcept { return ts_; }

    /**
     * @brief Convert to string (alias for format()).
     */

    std::string to_string() const { return format(true); }

    bool operator==(const Timestamp& other) const noexcept {
        return ts_.nanos == other.ts_.nanos;
    }

    bool operator!=(const Timestamp& other) const noexcept {
        return ts_.nanos != other.ts_.nanos;
    }

    bool operator<(const Timestamp& other) const noexcept {
        return ts_.nanos < other.ts_.nanos;
    }

    bool operator<=(const Timestamp& other) const noexcept {
        return ts_.nanos <= other.ts_.nanos;
    }

    bool operator>(const Timestamp& other) const noexcept {
        return ts_.nanos > other.ts_.nanos;
    }

    bool operator>=(const Timestamp& other) const noexcept {
        return ts_.nanos >= other.ts_.nanos;
    }

private:
    ut_timestamp_t ts_;
};

/**
 * @brief Calendar conversion utilities.
 */

namespace calendar {

inline int gregorian_to_thai(int year) { return ut_gregorian_to_thai(year); }
inline int thai_to_gregorian(int year) { return ut_thai_to_gregorian(year); }
inline int gregorian_to_dangi(int year) { return ut_gregorian_to_dangi(year); }
inline int dangi_to_gregorian(int year) { return ut_dangi_to_gregorian(year); }
inline int gregorian_to_minguo(int year) { return ut_gregorian_to_minguo(year); }
inline int minguo_to_gregorian(int year) { return ut_minguo_to_gregorian(year); }

struct JapaneseEra {
    ut_japanese_era_t era;
    int year;
    std::string name() const { return ut_japanese_era_name(era); }
};

inline JapaneseEra to_japanese_era(const Timestamp& ts) {
    JapaneseEra result;
    ut_error_t err = ut_to_japanese_era(ts.raw(), &result.era, &result.year);
    if (err != UT_OK) {
        throw Error(err);
    }
    return result;
}

struct IsoWeek {
    int year;
    int week;
    int day;
};

inline IsoWeek to_iso_week(const Timestamp& ts) {
    IsoWeek result;
    ut_to_iso_week(ts.raw(), &result.year, &result.week, &result.day);
    return result;
}

}  /* namespace calendar */

/**
 * @brief Get clock precision available on this hardware.
 */

inline ut_precision_t get_clock_precision() {
    return ut_get_clock_precision();
}

}  /* namespace uts */

#endif /* UNIVERSAL_TIMESTAMP_HPP */
