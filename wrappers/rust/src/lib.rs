//! Safe Rust wrapper for the Universal Timestamp C library.
//!
//! # Example
//!
//! ```no_run
//! use universal_timestamp::Timestamp;
//!
//! let now = Timestamp::now();
//! println!("{}", now);
//! ```

use std::ffi::{CStr, CString};
use std::fmt;
use std::os::raw::{c_char, c_int};

// --- FFI Bindings ---

#[repr(C)]
#[derive(Debug, Copy, Clone, PartialEq, Eq, PartialOrd, Ord, Hash)]
struct ut_timestamp_t {
    nanos: i64,
}

#[allow(non_camel_case_types)]
type ut_error_t = c_int;

#[allow(non_camel_case_types)]
type ut_precision_t = c_int;

#[allow(dead_code)]
const UT_MAX_STRING_LEN: usize = 32;

#[allow(dead_code)]
const UT_OK: ut_error_t = 0;

extern "C" {
    fn ut_now() -> ut_timestamp_t;
    fn ut_now_monotonic() -> ut_timestamp_t;
    fn ut_format(ts: ut_timestamp_t, buf: *mut c_char, buf_size: usize, include_nanos: bool) -> c_int;
    fn ut_parse_strict(str: *const c_char, out: *mut ut_timestamp_t) -> ut_error_t;
    fn ut_parse_lenient(str: *const c_char, out: *mut ut_timestamp_t) -> ut_error_t;
    fn ut_from_unix_nanos(nanos: i64) -> ut_timestamp_t;
    fn ut_to_unix_nanos(ts: ut_timestamp_t) -> i64;
    fn ut_error_string(err: ut_error_t) -> *const c_char;
    fn ut_get_clock_precision() -> ut_precision_t;
    
    // Calendar
    fn ut_gregorian_to_thai(year: c_int) -> c_int;
    fn ut_thai_to_gregorian(year: c_int) -> c_int;
    fn ut_gregorian_to_dangi(year: c_int) -> c_int;
    fn ut_dangi_to_gregorian(year: c_int) -> c_int;
    fn ut_gregorian_to_minguo(year: c_int) -> c_int;
    fn ut_minguo_to_gregorian(year: c_int) -> c_int;
    
    fn ut_to_japanese_era(ts: ut_timestamp_t, era: *mut c_int, era_year: *mut c_int) -> ut_error_t;
    fn ut_japanese_era_name(era: c_int) -> *const c_char;
    
    fn ut_to_iso_week(ts: ut_timestamp_t, year: *mut c_int, week: *mut c_int, day: *mut c_int);
}

// --- Wrapper Implementation ---

#[derive(Debug, Clone)]
pub struct Error {
    code: ut_error_t,
    message: String,
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.message)
    }
}

impl std::error::Error for Error {}

impl Error {
    fn new(code: ut_error_t) -> Self {
        let msg_ptr = unsafe { ut_error_string(code) };
        let message = unsafe { CStr::from_ptr(msg_ptr) }.to_string_lossy().into_owned();
        Error { code, message }
    }
}

pub type Result<T> = std::result::Result<T, Error>;

#[derive(Debug, Copy, Clone, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Timestamp {
    inner: ut_timestamp_t,
}

impl Timestamp {
    /// Get the current UTC time.
    pub fn now() -> Self {
        unsafe {
             Timestamp { inner: ut_now() }
        }
    }

    /// Get the current UTC time with monotonic guarantee.
    pub fn now_monotonic() -> Self {
        unsafe {
            Timestamp { inner: ut_now_monotonic() }
        }
    }

    /// Create from Unix nanoseconds.
    pub fn from_nanos(nanos: i64) -> Self {
        unsafe {
            Timestamp { inner: ut_from_unix_nanos(nanos) }
        }
    }

    /// Get nanoseconds since Unix epoch.
    pub fn as_nanos(&self) -> i64 {
        unsafe { ut_to_unix_nanos(self.inner) }
    }

    /// Parse ISO-8601 string (strict).
    pub fn parse(s: &str) -> Result<Self> {
        let c_str = CString::new(s).map_err(|_| Error { code: -1, message: "Invalid C string".to_string() })?;
        let mut ts = ut_timestamp_t { nanos: 0 };
        let err = unsafe { ut_parse_strict(c_str.as_ptr(), &mut ts) };
        if err != UT_OK {
            return Err(Error::new(err));
        }
        Ok(Timestamp { inner: ts })
    }

    /// Parse ISO-8601 string (lenient).
    pub fn parse_lenient(s: &str) -> Result<Self> {
        let c_str = CString::new(s).map_err(|_| Error { code: -1, message: "Invalid C string".to_string() })?;
        let mut ts = ut_timestamp_t { nanos: 0 };
        let err = unsafe { ut_parse_lenient(c_str.as_ptr(), &mut ts) };
        if err != UT_OK {
            return Err(Error::new(err));
        }
        Ok(Timestamp { inner: ts })
    }

    /// Format to ISO-8601 string.
    pub fn format(&self, include_nanos: bool) -> String {
        let mut buf = vec![0u8; UT_MAX_STRING_LEN];
        unsafe {
             ut_format(self.inner, buf.as_mut_ptr() as *mut c_char, buf.len(), include_nanos);
        }
        let end = buf.iter().position(|&c| c == 0).unwrap_or(buf.len());
        String::from_utf8_lossy(&buf[..end]).into_owned()
    }

    pub fn to_iso_week(&self) -> (i32, i32, i32) {
        let mut year = 0;
        let mut week = 0;
        let mut day = 0;
        unsafe {
            ut_to_iso_week(self.inner, &mut year, &mut week, &mut day);
        }
        (year, week, day)
    }
    
    pub fn to_japanese_era(&self) -> Result<(i32, i32, String)> {
        let mut era = 0;
        let mut year = 0;
        let err = unsafe { ut_to_japanese_era(self.inner, &mut era, &mut year) };
        if err != UT_OK {
            return Err(Error::new(err));
        }
        let name_ptr = unsafe { ut_japanese_era_name(era) };
        let name = unsafe { CStr::from_ptr(name_ptr) }.to_string_lossy().into_owned();
        Ok((era, year, name))
    }
}

impl fmt::Display for Timestamp {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.format(true))
    }
}

#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub enum Precision {
    Nanosecond = 0,
    Microsecond = 1,
    Millisecond = 2,
    Second = 3,
    Error = -1,
}

pub fn get_clock_precision() -> Precision {
    let p = unsafe { ut_get_clock_precision() };
    match p {
        0 => Precision::Nanosecond,
        1 => Precision::Microsecond,
        2 => Precision::Millisecond,
        3 => Precision::Second,
        _ => Precision::Error,
    }
}

pub mod calendar {
    use super::*;

    pub fn gregorian_to_thai(year: i32) -> i32 {
        unsafe { ut_gregorian_to_thai(year) }
    }
    pub fn thai_to_gregorian(year: i32) -> i32 {
        unsafe { ut_thai_to_gregorian(year) }
    }
    pub fn gregorian_to_dangi(year: i32) -> i32 {
        unsafe { ut_gregorian_to_dangi(year) }
    }
    pub fn dangi_to_gregorian(year: i32) -> i32 {
        unsafe { ut_dangi_to_gregorian(year) }
    }
    pub fn gregorian_to_minguo(year: i32) -> i32 {
        unsafe { ut_gregorian_to_minguo(year) }
    }
    pub fn minguo_to_gregorian(year: i32) -> i32 {
        unsafe { ut_minguo_to_gregorian(year) }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_now() {
        let _ = Timestamp::now();
    }

    #[test]
    fn test_format_parse() {
        let ts = Timestamp::now_monotonic();
        let s = ts.format(true);
        let parsed = Timestamp::parse(&s).unwrap();
        assert_eq!(ts, parsed);
    }
    
    #[test]
    fn test_calendar() {
        assert_eq!(calendar::gregorian_to_thai(2024), 2567);
    }
}
