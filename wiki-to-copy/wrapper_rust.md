# Rust Wrapper

Safe Rust bindings for the Universal Timestamp Library.

## Installation

1. Install the C library:

   ```bash
   make build_c
   sudo make install_c
   ```

2. Add to your `Cargo.toml`:

   ```toml
   [dependencies]
   universal_timestamp = { path = "path/to/wrappers/rust" }
   ```
   
   *Note: This wrapper is designed to be vendored or used as a local path dependency initially.*

## Requirements

- Rust 1.56+
- Universal Timestamp C library (libuniversal_timestamp)

## Usage

```rust
use universal_timestamp::Timestamp;

fn main() {
    // Get current time
    let now = Timestamp::now();
    println!("{}", now); // 2024-12-14T12:00:00.123456789Z

    // Parse
    let ts = Timestamp::parse("2024-12-14T03:13:21Z").unwrap();
    
    // Comparison
    if now > ts {
        println!("now is after ts");
    }
}
```

## API Reference

### `Timestamp`

| Method | Description |
|--------|-------------|
| `now()` | Get current UTC time |
| `now_monotonic()` | Get monotonic timestamp |
| `from_nanos(i64)` | Create from nanoseconds |
| `parse(&str)` | Parse ISO-8601 (strict) |
| `parse_lenient(&str)` | Parse ISO-8601 (lenient) |
| `format(bool)` | Format to string |
| `as_nanos()` | Get underlying nanoseconds |

### `calendar` module

Functions: `gregorian_to_thai`, `thai_to_gregorian`, etc.

## Testing

Run tests with:

```bash
cargo test
```
