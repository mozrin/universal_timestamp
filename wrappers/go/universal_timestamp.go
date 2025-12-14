package universal_timestamp

/*
#cgo CFLAGS: -I../../include
#cgo LDFLAGS: -L../../dist -l:libuniversal_timestamp.a
#include <stdlib.h>
#include "universal_timestamp.h"
*/
import "C"

import (
	"errors"
	"time"
	"unsafe"
)

// Timestamp represents a Universal Timestamp.
// It wraps the C ut_timestamp_t which is an int64 of nanoseconds.
type Timestamp int64

// Now returns the current UTC timestamp.
func Now() Timestamp {
	return Timestamp(C.ut_now().nanos)
}

// NowNanos returns the current UTC timestamp as Unix nanoseconds (int64).
func NowNanos() int64 {
	return int64(C.ut_now().nanos)
}

// Parse parses an ISO-8601 string into a timestamp.
// It uses strict strict parsing by default.
func Parse(s string) (Timestamp, error) {
	cs := C.CString(s)
	defer C.free(unsafe.Pointer(cs))

	var ts C.ut_timestamp_t
	if err := C.ut_parse_strict(cs, &ts); err != C.UT_OK {
		return 0, errors.New("invalid timestamp format")
	}
	return Timestamp(ts.nanos), nil
}

// Format formats the timestamp as an ISO-8601 string.
func (t Timestamp) Format() string {
	var buf [64]C.char
	cts := C.ut_timestamp_t{nanos: C.long(t)}
	C.ut_format(cts, &buf[0], 64, true) // strict/z
	return C.GoString(&buf[0])
}

// ToTime converts the timestamp to a standard Go time.Time.
func (t Timestamp) ToTime() time.Time {
	return time.Unix(0, int64(t)).UTC()
}

// FromTime creates a Timestamp from a standard Go time.Time.
func FromTime(t time.Time) Timestamp {
	return Timestamp(t.UnixNano())
}
