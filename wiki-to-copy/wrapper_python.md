# Python Wrapper

Python bindings for the Universal Timestamp Library using ctypes FFI.

## Installation

1. Install the C library:

   ```bash
   make build_c
   sudo make install_c
   ```

2. Install the Python package:

   ```bash
   cd wrappers/python
   pip install .
   ```

## Requirements

- Python 3.9+
- Universal Timestamp C library (libuniversal_timestamp)

## Usage

```python
from universal_timestamp import Timestamp

# Get current time
ts = Timestamp.now()
print(ts)  # 2024-12-14T12:00:00.123456789Z

# Parse a timestamp
ts = Timestamp.parse("2024-12-14T03:13:21Z")
print(ts.nanos)  # Nanoseconds since epoch

# Lenient parsing (missing Z allowed)
ts = Timestamp.parse_lenient("2024-12-14T03:13:21")

# Comparison
if Timestamp.now() > ts:
    print("now is after ts")
```

## API Reference

### `Timestamp`

| Method | Description |
|--------|-------------|
| `Timestamp(nanos)` | Construct from nanoseconds |
| `Timestamp.now()` | Get current UTC time |
| `Timestamp.now_monotonic()` | Get monotonic timestamp |
| `Timestamp.parse(s, lenient=False)` | Parse ISO-8601 string |
| `Timestamp.parse_strict(s)` | Parse ISO-8601 (strict) |
| `Timestamp.parse_lenient(s)` | Parse ISO-8601 (lenient) |
| `ts.format(include_nanos=True)` | Format to ISO-8601 string |
| `ts.nanos` | Get underlying nanoseconds |
| `ts.to_iso_week()` | Get `(year, week, day)` tuple |
| `ts.to_japanese_era()` | Get `(era, year, name)` tuple |

Comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`

### `TimestampError` / `ParseError`

Exception classes thrown on parse/validation errors.

| Attribute | Description |
|-----------|-------------|
| `code` | `Error` enum value |
| `args[0]` | Error message |

### Calendar Functions

| Function | Description |
|----------|-------------|
| `gregorian_to_thai(year)` | Convert to Thai year (+543) |
| `thai_to_gregorian(year)` | Convert from Thai year |
| `gregorian_to_dangi(year)` | Convert to Dangi year (+2333) |
| `dangi_to_gregorian(year)` | Convert from Dangi year |
| `gregorian_to_minguo(year)` | Convert to Minguo year (-1911) |
| `minguo_to_gregorian(year)` | Convert from Minguo year |

### `get_clock_precision()`

Returns hardware clock precision level as `Precision` enum (NANOSECOND, MICROSECOND, MILLISECOND, SECOND).

## Enums

- `Error` — Error codes (OK, INVALID_FORMAT, INVALID_DATE, etc.)
- `Precision` — Clock precision levels
- `Calendar` — Calendar types
- `JapaneseEra` — Japanese era identifiers (REIWA, HEISEI, SHOWA, TAISHO, MEIJI)
