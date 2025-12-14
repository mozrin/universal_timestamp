#!/bin/bash

# Universal Timestamp Bash Wrapper
# Depends on 'uts-cli' being in the PATH or configured via UTS_CLI_PATH

# Set path to cli if not set
: "${UTS_CLI_PATH:=uts-cli}"

# Check if cli exists
if ! command -v "$UTS_CLI_PATH" >/dev/null 2>&1; then
    # Try local build location if available
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    if [ -f "$SCRIPT_DIR/uts-cli" ]; then
        UTS_CLI_PATH="$SCRIPT_DIR/uts-cli"
    else
        echo "Error: uts-cli not found. Set UTS_CLI_PATH or install it." >&2
    fi
fi

# Get current timestamp
# Usage: now=$(ut_now)
ut_now() {
    "$UTS_CLI_PATH" now
}

# Get current timestamp in nanoseconds
# Usage: nanos=$(ut_now_nanos)
ut_now_nanos() {
    "$UTS_CLI_PATH" now-nanos
}

# Parse ISO-8601 string to nanoseconds
# Usage: nanos=$(ut_parse "2024-01-01T12:00:00Z")
ut_parse() {
    "$UTS_CLI_PATH" parse "$1"
}

# Format nanoseconds to ISO-8601 string
# Usage: str=$(ut_format 1704110400000000000)
ut_format() {
    "$UTS_CLI_PATH" format "$1"
}
