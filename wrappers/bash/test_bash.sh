#!/bin/bash
set -e

# Setup environment
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export UTS_CLI_PATH="$SCRIPT_DIR/uts-cli"
export LD_LIBRARY_PATH="$SCRIPT_DIR/../../dist:$LD_LIBRARY_PATH"

# Load library
source "$SCRIPT_DIR/universal_timestamp.sh"

echo "Running Bash wrapper tests..."

# Test 1: Now using CLI directly
echo "Testing CLI direct usage..."
"$UTS_CLI_PATH" now > /dev/null
echo "PASS: uts-cli now"

# Test 2: Library function ut_now
echo "Testing ut_now()..."
ts=$(ut_now)
if [[ -z "$ts" ]]; then
    echo "FAIL: ut_now returned empty"
    exit 1
fi
echo "PASS: ut_now -> $ts"

# Test 3: Parse and Format
echo "Testing parse and format..."
input="2024-12-14T12:00:00Z"
nanos=$(ut_parse "$input")
echo "  Parsed nanos: $nanos"
output=$(ut_format "$nanos")
echo "  Formatted: $output"

if [[ "$input" == "$output"* ]]; then
    echo "PASS: Round trip successful"
else
    echo "FAIL: Round trip mismatch. Expected '$input', got '$output'"
    exit 1
fi

echo "All Bash tests passed!"
