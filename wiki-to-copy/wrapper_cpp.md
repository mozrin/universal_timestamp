# C++ Wrapper

Header-only C++ wrapper providing idiomatic C++ classes for the Universal Timestamp Library.

## Installation

Copy `wrappers/cpp/universal_timestamp.hpp` to your include path.

## Requirements

- C++11 or later
- Universal Timestamp C library

## Usage

```cpp
#include "universal_timestamp.hpp"
#include <iostream>

int main() {
    // Get current time
    uts::Timestamp now = uts::Timestamp::now();
    std::cout << now.format() << std::endl;

    // Parse a timestamp
    uts::Timestamp ts = uts::Timestamp::parse("2024-12-14T03:13:21Z");

    // Comparison
    if (now > ts) {
        std::cout << "now is after ts" << std::endl;
    }

    return 0;
}
```

## Building

```bash
g++ -std=c++11 myapp.cpp -I/path/to/include -I/path/to/wrappers/cpp \
    -L/path/to/lib -l:libuniversal_timestamp.a -o myapp
```

## API Reference

### `uts::Timestamp`

| Method | Description |
|--------|-------------|
| `Timestamp(int64_t nanos)` | Construct from nanoseconds |
| `static now()` | Get current UTC time |
| `static now_monotonic()` | Get monotonic timestamp |
| `static parse(string)` | Parse ISO-8601 (strict) |
| `static parse_lenient(string)` | Parse ISO-8601 (lenient) |
| `format(bool nanos)` | Format to ISO-8601 string |
| `nanos()` | Get underlying nanoseconds |
| `to_string()` | Alias for `format(true)` |

Comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`

### `uts::Error`

Exception class thrown on parse/validation errors.

| Method | Description |
|--------|-------------|
| `code()` | Get `ut_error_t` code |
| `what()` | Get error message |

### `uts::calendar`

| Function | Description |
|----------|-------------|
| `gregorian_to_thai(year)` | Convert to Thai year |
| `thai_to_gregorian(year)` | Convert from Thai year |
| `gregorian_to_dangi(year)` | Convert to Dangi year |
| `dangi_to_gregorian(year)` | Convert from Dangi year |
| `gregorian_to_minguo(year)` | Convert to Minguo year |
| `minguo_to_gregorian(year)` | Convert from Minguo year |
| `to_japanese_era(ts)` | Get `JapaneseEra` struct |
| `to_iso_week(ts)` | Get `IsoWeek` struct |

### `uts::get_clock_precision()`

Returns hardware clock precision level (0=ns, 1=µs, 2=ms, 3=s).
