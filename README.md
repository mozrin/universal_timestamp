# Universal Timestamp

[![C11](https://img.shields.io/badge/C-C11-blue.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

A **deterministic, cross-platform timestamp library** for C with nanosecond precision.

## Features

- ✅ **Always UTC** — No timezone ambiguity
- ✅ **Always ISO-8601** — `YYYY-MM-DDTHH:MM:SS.nnnnnnnnnZ`
- ✅ **Nanosecond precision** — Full 9-digit fractional seconds
- ✅ **Monotonic mode** — Timestamps never go backwards
- ✅ **Strict & lenient parsing** — Validate or be flexible
- ✅ **Zero dependencies** — No locale, no timezone files
- ✅ **6 calendar systems** — Gregorian, Thai, Dangi, Minguo, Japanese, ISO week
- ✅ **20+ platforms** — Windows (PowerShell), macOS, Linux, BSD, Solaris, AIX, QNX, WASM, and more
- ✅ **Wrappers** — C, C++, Python, Rust, Go, Bash

## Quick Start

```c
#include "universal_timestamp.h"
#include <stdio.h>

int main(void) {
    /* Get current UTC time */
    ut_timestamp_t now = ut_now();

    /* Format to ISO-8601 */
    char buf[UT_MAX_STRING_LEN];
    ut_format(now, buf, sizeof(buf), true);
    printf("Now: %s\n", buf);

    /* Parse a timestamp */
    ut_timestamp_t ts;
    ut_error_t err = ut_parse_strict("2024-12-14T03:13:21.123456789Z", &ts);
    if (err == UT_OK) {
        printf("Parsed: %lld nanos\n", (long long)ut_to_unix_nanos(ts));
    }

    return 0;
}
```

## Building

```bash
make              # Build library and tests
make test         # Run tests
make install      # Install to /usr/local (requires sudo)
make install PREFIX=/opt/local  # Custom prefix
```

## Installation

After building:

```bash
sudo make install
```

This installs:

- `/usr/local/lib/libuniversal_timestamp.a`
- `/usr/local/include/universal_timestamp.h`
- `/usr/local/lib/pkgconfig/universal_timestamp.pc`

### Using with pkg-config

```bash
gcc myapp.c $(pkg-config --cflags --libs universal_timestamp)
```

## API Reference

### Core Functions

| Function | Description |
|----------|-------------|
| `ut_now()` | Get current UTC timestamp |
| `ut_now_monotonic()` | Get monotonic timestamp (never goes backwards) |
| `ut_format()` | Format timestamp to ISO-8601 string |
| `ut_parse_strict()` | Parse with strict validation |
| `ut_parse_lenient()` | Parse with relaxed rules |
| `ut_from_unix_nanos()` | Create from Unix nanoseconds |
| `ut_to_unix_nanos()` | Convert to Unix nanoseconds |
| `ut_get_clock_precision()` | Detect hardware clock precision (0=ns, 1=µs, 2=ms, 3=s) |

### Calendar Conversions

| Function | Description |
|----------|-------------|
| `ut_gregorian_to_thai()` | Gregorian → Thai Buddhist Era (+543) |
| `ut_gregorian_to_dangi()` | Gregorian → Korean Dangi (+2333) |
| `ut_gregorian_to_minguo()` | Gregorian → Taiwan ROC (−1911) |
| `ut_to_japanese_era()` | Get Japanese era and year |
| `ut_to_iso_week()` | Get ISO week-date components |

## Specification

See [wiki/specification.md](wiki-to-copy/specification.md) for the full Universal Timestamp Specification.

## Project Structure

```text
universal_timestamp/
├── include/
│   └── universal_timestamp.h    # Public API
├── src/
│   ├── core/
│   │   ├── ut_internal.h        # Private declarations
│   │   └── ut_core.c            # Date/time utilities
│   ├── ut_now.c                 # now(), monotonic(), conversions
│   ├── ut_format.c              # Formatting
│   ├── ut_parse.c               # Parsing
│   └── ut_calendar.c            # Calendar conversions
├── test/
│   └── test.c                   # Test suite
├── build/                       # Object files
├── Makefile
└── README.md
```

## License

MIT License. See [LICENSE](LICENSE) for details.
