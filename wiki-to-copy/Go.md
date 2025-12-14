# Go Wrapper

The Go wrapper provides CGO bindings for the Universal Timestamp library.

## Installation

1. Ensure the C library is installed or available.
2. Get the package:

```bash
go get github.com/mozrin/universal_timestamp/wrappers/go
```

## Usage

```go
package main

import (
    "fmt"
    uts "github.com/mozrin/universal_timestamp/wrappers/go"
)

func main() {
    // Get current time
    now := uts.Now()
    fmt.Println(now.Format())

    // Parse
    ts, _ := uts.Parse("2024-12-14T12:00:00Z")
    fmt.Printf("Nanos: %d\n", int64(ts))
}
```
