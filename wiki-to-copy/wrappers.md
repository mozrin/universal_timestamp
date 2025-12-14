# Language Wrappers

The Universal Timestamp Library provides official wrappers for multiple programming languages.

## Available Wrappers

| Language | Directory | Status |
|----------|-----------|--------|
| [C++](wrapper_cpp.md) | `wrappers/cpp/` | ✅ Available |
| [Python](wrapper_python.md) | `wrappers/python/` | ✅ Available |
| [Rust](wrapper_rust.md) | `wrappers/rust/` | ✅ Available |
| Go | `wrappers/go/` | 🚧 Planned |

## Design Principles

All wrappers follow these principles:

1. **Idiomatic API** — Use language-native conventions
2. **Thin layer** — Minimal overhead over the C library
3. **Complete coverage** — Expose all C API functionality
4. **Type safety** — Leverage language type systems
5. **Error handling** — Use language-native error mechanisms

## Building Wrappers

Each wrapper has its own build instructions in its documentation.
