# Universal Timestamp - Python Wrapper

Python bindings for the Universal Timestamp C library using ctypes.

## Requirements

- Python 3.9+
- The `libuniversal_timestamp` C library must be installed

## Installation

1. First, install the C library:

   ```bash
   cd /path/to/universal_timestamp
   make
   sudo make install_c
   ```

2. Then install the Python package:

   ```bash
   cd wrappers/python
   pip install .
   ```

   Or for development:

   ```bash
   pip install -e .
   ```

## Usage

```python
from universal_timestamp import Timestamp

# Get current time
ts = Timestamp.now()
print(ts)  # 2024-12-14T12:00:00.123456789Z

# Parse a timestamp
ts = Timestamp.parse("2024-12-14T00:00:00Z")
print(ts.nanos)  # 1734134400000000000

# Lenient parsing
ts = Timestamp.parse_lenient("2024-12-14T00:00:00")  # Missing Z is OK

# Format with/without nanoseconds
print(ts.format(include_nanos=True))
print(ts.format(include_nanos=False))

# Calendar conversions
from universal_timestamp import gregorian_to_thai, gregorian_to_dangi

print(gregorian_to_thai(2024))   # 2567
print(gregorian_to_dangi(2024))  # 4357

# Japanese era
era, year, name = ts.to_japanese_era()
print(f"{name} {year}")  # Reiwa 6

# ISO week
year, week, day = ts.to_iso_week()
print(f"{year}-W{week:02d}-{day}")  # 2024-W50-6

# Clock precision
from universal_timestamp import get_clock_precision, Precision

prec = get_clock_precision()
if prec == Precision.NANOSECOND:
    print("Full nanosecond precision available")
```

## API Reference

### `Timestamp`

| Method | Description |
|--------|-------------|
| `Timestamp.now()` | Get current UTC time |
| `Timestamp.now_monotonic()` | Get monotonically increasing UTC time |
| `Timestamp.parse(s, lenient=False)` | Parse ISO-8601 string |
| `Timestamp.parse_strict(s)` | Parse in strict mode |
| `Timestamp.parse_lenient(s)` | Parse in lenient mode |
| `ts.format(include_nanos=True)` | Format to ISO-8601 string |
| `ts.nanos` | Nanoseconds since Unix epoch |
| `ts.to_iso_week()` | Get (year, week, day) tuple |
| `ts.to_japanese_era()` | Get (era, year, name) tuple |

### Calendar Functions

| Function | Description |
|----------|-------------|
| `gregorian_to_thai(year)` | Gregorian → Thai (+543) |
| `thai_to_gregorian(year)` | Thai → Gregorian (-543) |
| `gregorian_to_dangi(year)` | Gregorian → Korean Dangi (+2333) |
| `dangi_to_gregorian(year)` | Dangi → Gregorian (-2333) |
| `gregorian_to_minguo(year)` | Gregorian → Minguo/ROC (-1911) |
| `minguo_to_gregorian(year)` | Minguo → Gregorian (+1911) |

### Exceptions

- `TimestampError` - Base exception with `.code` attribute
- `ParseError` - Raised when parsing fails
