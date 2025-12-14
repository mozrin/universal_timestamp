# Bash Wrapper

A simple CLI utility and Bash library for the Universal Timestamp Library.

## Overview

The Bash wrapper consists of:
1.  `uts-cli`: A C binary that links against the library and provides a CLI interface.
2.  `universal_timestamp.sh`: A shell script library that wraps `uts-cli` with convenient shell functions.

## Installation

```bash
make build_bash
sudo make install_bash
```

This installs:
- `/usr/local/bin/uts-cli`
- `/usr/local/bin/universal_timestamp.sh`

## Usage (CLI)

You can use the CLI tool directly:

```bash
uts-cli now
# Output: 2024-12-14T12:00:00.123456789Z

uts-cli now-nanos
# Output: 1702555200123456789

uts-cli parse "2024-01-01T00:00:00Z"
# Output: 1704067200000000000
```

## Usage (Scripting)

Source the library in your scripts:

```bash
#!/bin/bash
source /usr/local/bin/universal_timestamp.sh

# Get current time
ts=$(ut_now)
echo "Current time: $ts"

# Get nanoseconds
nanos=$(ut_now_nanos)

# Arithmetic (using bc or shell math)
future_nanos=$((nanos + 1000000000)) # +1 second

# Format back to string
future_ts=$(ut_format "$future_nanos")
echo "One second later: $future_ts"
```

## Functions

| Function | CLI Command | Description |
|----------|-------------|-------------|
| `ut_now` | `uts-cli now` | Get current ISO-8601 timestamp |
| `ut_now_nanos` | `uts-cli now-nanos` | Get current Unix nanoseconds |
| `ut_parse <str>` | `uts-cli parse` | Parse ISO-8601 string to nanos |
| `ut_format <nanos>` | `uts-cli format` | Format nanos to ISO-8601 string |

## Testing

```bash
make test_bash
```
