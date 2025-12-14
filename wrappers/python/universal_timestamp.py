"""
Universal Timestamp Library - Python Wrapper

Provides Pythonic access to the Universal Timestamp C library via ctypes.

Example:
    >>> from universal_timestamp import Timestamp
    >>> ts = Timestamp.now()
    >>> print(ts)
    2024-12-14T12:00:00.123456789Z
    >>> ts2 = Timestamp.parse("2024-12-14T00:00:00Z")
    >>> print(ts2.nanos)
    1734134400000000000
"""

from __future__ import annotations

import ctypes
import ctypes.util
import os
from ctypes import c_int, c_int64, c_char_p, c_size_t, c_bool, POINTER, Structure
from enum import IntEnum
from pathlib import Path
from typing import Optional, Callable


# --- Library Loading ---

def _find_library() -> ctypes.CDLL:
    """Find and load the universal_timestamp shared library."""
    
    # Search paths in order of preference
    search_paths = [
        # Relative to this file (development)
        Path(__file__).parent.parent.parent / "build" / "libuniversal_timestamp.so",
        Path(__file__).parent.parent.parent / "dist" / "lib" / "libuniversal_timestamp.so",
        # System paths
        Path("/usr/local/lib/libuniversal_timestamp.so"),
        Path("/usr/lib/libuniversal_timestamp.so"),
    ]
    
    # Try LD_LIBRARY_PATH locations
    ld_path = os.environ.get("LD_LIBRARY_PATH", "")
    for path_dir in ld_path.split(":"):
        if path_dir:
            search_paths.append(Path(path_dir) / "libuniversal_timestamp.so")
    
    # Try ctypes.util.find_library
    lib_path = ctypes.util.find_library("universal_timestamp")
    if lib_path:
        search_paths.insert(0, Path(lib_path))
    
    # Try each path
    for path in search_paths:
        if path.exists():
            try:
                return ctypes.CDLL(str(path))
            except OSError:
                continue
    
    raise OSError(
        "Could not find libuniversal_timestamp.so. "
        "Please ensure the library is installed or set LD_LIBRARY_PATH."
    )


_lib: Optional[ctypes.CDLL] = None


def _get_lib() -> ctypes.CDLL:
    """Get the loaded library, loading it if necessary."""
    global _lib
    if _lib is None:
        _lib = _find_library()
        _setup_bindings(_lib)
    return _lib


# --- C Types ---

class _UtTimestamp(Structure):
    """C struct ut_timestamp_t."""
    _fields_ = [("nanos", c_int64)]


class Error(IntEnum):
    """Error codes from ut_error_t."""
    OK = 0
    INVALID_FORMAT = 1
    INVALID_DATE = 2
    OUT_OF_RANGE = 3
    UNSUPPORTED_OFFSET = 4
    FRACTION_TOO_LONG = 5
    LEAP_SECOND = 6
    NULL_POINTER = 7


class Precision(IntEnum):
    """Clock precision levels from ut_precision_t."""
    NANOSECOND = 0
    MICROSECOND = 1
    MILLISECOND = 2
    SECOND = 3
    ERROR = -1


class Calendar(IntEnum):
    """Calendar types from ut_calendar_t."""
    GREGORIAN = 0
    THAI = 1
    DANGI = 2
    MINGUO = 3
    JAPANESE = 4
    ISO_WEEK = 5


class JapaneseEra(IntEnum):
    """Japanese era identifiers from ut_japanese_era_t."""
    REIWA = 0
    HEISEI = 1
    SHOWA = 2
    TAISHO = 3
    MEIJI = 4


# --- Function Bindings ---

def _setup_bindings(lib: ctypes.CDLL) -> None:
    """Set up ctypes function signatures."""
    
    # ut_now
    lib.ut_now.argtypes = []
    lib.ut_now.restype = _UtTimestamp
    
    # ut_now_monotonic
    lib.ut_now_monotonic.argtypes = []
    lib.ut_now_monotonic.restype = _UtTimestamp
    
    # ut_format
    lib.ut_format.argtypes = [_UtTimestamp, c_char_p, c_size_t, c_bool]
    lib.ut_format.restype = c_int
    
    # ut_parse_strict
    lib.ut_parse_strict.argtypes = [c_char_p, POINTER(_UtTimestamp)]
    lib.ut_parse_strict.restype = c_int
    
    # ut_parse_lenient
    lib.ut_parse_lenient.argtypes = [c_char_p, POINTER(_UtTimestamp)]
    lib.ut_parse_lenient.restype = c_int
    
    # ut_from_unix_nanos
    lib.ut_from_unix_nanos.argtypes = [c_int64]
    lib.ut_from_unix_nanos.restype = _UtTimestamp
    
    # ut_to_unix_nanos
    lib.ut_to_unix_nanos.argtypes = [_UtTimestamp]
    lib.ut_to_unix_nanos.restype = c_int64
    
    # ut_error_string
    lib.ut_error_string.argtypes = [c_int]
    lib.ut_error_string.restype = c_char_p
    
    # ut_get_clock_precision
    lib.ut_get_clock_precision.argtypes = []
    lib.ut_get_clock_precision.restype = c_int
    
    # Calendar conversions
    lib.ut_gregorian_to_thai.argtypes = [c_int]
    lib.ut_gregorian_to_thai.restype = c_int
    
    lib.ut_thai_to_gregorian.argtypes = [c_int]
    lib.ut_thai_to_gregorian.restype = c_int
    
    lib.ut_gregorian_to_dangi.argtypes = [c_int]
    lib.ut_gregorian_to_dangi.restype = c_int
    
    lib.ut_dangi_to_gregorian.argtypes = [c_int]
    lib.ut_dangi_to_gregorian.restype = c_int
    
    lib.ut_gregorian_to_minguo.argtypes = [c_int]
    lib.ut_gregorian_to_minguo.restype = c_int
    
    lib.ut_minguo_to_gregorian.argtypes = [c_int]
    lib.ut_minguo_to_gregorian.restype = c_int
    
    # Japanese era
    lib.ut_to_japanese_era.argtypes = [_UtTimestamp, POINTER(c_int), POINTER(c_int)]
    lib.ut_to_japanese_era.restype = c_int
    
    lib.ut_japanese_era_name.argtypes = [c_int]
    lib.ut_japanese_era_name.restype = c_char_p
    
    # ISO week
    lib.ut_to_iso_week.argtypes = [_UtTimestamp, POINTER(c_int), POINTER(c_int), POINTER(c_int)]
    lib.ut_to_iso_week.restype = None


# --- Exceptions ---

class TimestampError(Exception):
    """Base exception for timestamp errors."""
    
    def __init__(self, code: Error, message: Optional[str] = None):
        self.code = code
        if message is None:
            lib = _get_lib()
            message = lib.ut_error_string(code).decode("utf-8")
        super().__init__(message)


class ParseError(TimestampError):
    """Raised when parsing fails."""
    pass


# --- Main Classes ---

class Timestamp:
    """
    Represents a UTC timestamp with nanosecond precision.
    
    Example:
        >>> ts = Timestamp.now()
        >>> print(ts.format())
        2024-12-14T12:00:00.123456789Z
        
        >>> ts = Timestamp.parse("2024-12-14T00:00:00Z")
        >>> print(ts.nanos)
        1734134400000000000
    """
    
    __slots__ = ("_ts",)
    
    def __init__(self, nanos: int = 0):
        """Create a timestamp from nanoseconds since Unix epoch."""
        lib = _get_lib()
        self._ts = lib.ut_from_unix_nanos(nanos)
    
    @classmethod
    def _from_c(cls, ts: _UtTimestamp) -> Timestamp:
        """Create from C struct (internal use)."""
        instance = object.__new__(cls)
        instance._ts = ts
        return instance
    
    @classmethod
    def now(cls) -> Timestamp:
        """Get current UTC time."""
        lib = _get_lib()
        return cls._from_c(lib.ut_now())
    
    @classmethod
    def now_monotonic(cls) -> Timestamp:
        """Get current UTC time with monotonic guarantee."""
        lib = _get_lib()
        return cls._from_c(lib.ut_now_monotonic())
    
    @classmethod
    def parse(cls, s: str, *, lenient: bool = False) -> Timestamp:
        """
        Parse an ISO-8601 timestamp string.
        
        Args:
            s: The timestamp string to parse.
            lenient: If True, use lenient parsing mode.
        
        Returns:
            Parsed Timestamp.
        
        Raises:
            ParseError: If parsing fails.
        """
        lib = _get_lib()
        ts = _UtTimestamp()
        
        parse_fn = lib.ut_parse_lenient if lenient else lib.ut_parse_strict
        err = parse_fn(s.encode("utf-8"), ctypes.byref(ts))
        
        if err != Error.OK:
            raise ParseError(Error(err))
        
        return cls._from_c(ts)
    
    @classmethod
    def parse_strict(cls, s: str) -> Timestamp:
        """Parse with strict mode (alias for parse(s, lenient=False))."""
        return cls.parse(s, lenient=False)
    
    @classmethod
    def parse_lenient(cls, s: str) -> Timestamp:
        """Parse with lenient mode (alias for parse(s, lenient=True))."""
        return cls.parse(s, lenient=True)
    
    @property
    def nanos(self) -> int:
        """Nanoseconds since Unix epoch."""
        return self._ts.nanos
    
    def format(self, include_nanos: bool = True) -> str:
        """
        Format to ISO-8601 string.
        
        Args:
            include_nanos: Include fractional seconds if non-zero.
        
        Returns:
            Formatted timestamp string.
        """
        lib = _get_lib()
        buf = ctypes.create_string_buffer(32)
        lib.ut_format(self._ts, buf, 32, include_nanos)
        return buf.value.decode("utf-8")
    
    def to_iso_week(self) -> tuple[int, int, int]:
        """
        Get ISO week date components.
        
        Returns:
            Tuple of (year, week, day) where day is 1-7 (Monday=1).
        """
        lib = _get_lib()
        year = c_int()
        week = c_int()
        day = c_int()
        lib.ut_to_iso_week(self._ts, ctypes.byref(year), ctypes.byref(week), ctypes.byref(day))
        return (year.value, week.value, day.value)
    
    def to_japanese_era(self) -> tuple[JapaneseEra, int, str]:
        """
        Get Japanese era representation.
        
        Returns:
            Tuple of (era, year_in_era, era_name).
        
        Raises:
            TimestampError: If date is before Meiji era.
        """
        lib = _get_lib()
        era = c_int()
        year = c_int()
        err = lib.ut_to_japanese_era(self._ts, ctypes.byref(era), ctypes.byref(year))
        
        if err != Error.OK:
            raise TimestampError(Error(err))
        
        era_enum = JapaneseEra(era.value)
        era_name = lib.ut_japanese_era_name(era.value).decode("utf-8")
        return (era_enum, year.value, era_name)
    
    def __str__(self) -> str:
        return self.format(include_nanos=True)
    
    def __repr__(self) -> str:
        return f"Timestamp({self._ts.nanos})"
    
    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Timestamp):
            return NotImplemented
        return self._ts.nanos == other._ts.nanos
    
    def __ne__(self, other: object) -> bool:
        if not isinstance(other, Timestamp):
            return NotImplemented
        return self._ts.nanos != other._ts.nanos
    
    def __lt__(self, other: Timestamp) -> bool:
        return self._ts.nanos < other._ts.nanos
    
    def __le__(self, other: Timestamp) -> bool:
        return self._ts.nanos <= other._ts.nanos
    
    def __gt__(self, other: Timestamp) -> bool:
        return self._ts.nanos > other._ts.nanos
    
    def __ge__(self, other: Timestamp) -> bool:
        return self._ts.nanos >= other._ts.nanos
    
    def __hash__(self) -> int:
        return hash(self._ts.nanos)


# --- Calendar Utilities ---

def gregorian_to_thai(year: int) -> int:
    """Convert Gregorian year to Thai Buddhist Era year (+543)."""
    return _get_lib().ut_gregorian_to_thai(year)


def thai_to_gregorian(year: int) -> int:
    """Convert Thai Buddhist Era year to Gregorian year (-543)."""
    return _get_lib().ut_thai_to_gregorian(year)


def gregorian_to_dangi(year: int) -> int:
    """Convert Gregorian year to Korean Dangi year (+2333)."""
    return _get_lib().ut_gregorian_to_dangi(year)


def dangi_to_gregorian(year: int) -> int:
    """Convert Korean Dangi year to Gregorian year (-2333)."""
    return _get_lib().ut_dangi_to_gregorian(year)


def gregorian_to_minguo(year: int) -> int:
    """Convert Gregorian year to Minguo/ROC year (-1911)."""
    return _get_lib().ut_gregorian_to_minguo(year)


def minguo_to_gregorian(year: int) -> int:
    """Convert Minguo/ROC year to Gregorian year (+1911)."""
    return _get_lib().ut_minguo_to_gregorian(year)


def get_clock_precision() -> Precision:
    """Get the clock precision available on this hardware."""
    return Precision(_get_lib().ut_get_clock_precision())


# --- Convenience Exports ---

__all__ = [
    # Main class
    "Timestamp",
    # Exceptions
    "TimestampError",
    "ParseError",
    # Enums
    "Error",
    "Precision",
    "Calendar",
    "JapaneseEra",
    # Calendar functions
    "gregorian_to_thai",
    "thai_to_gregorian",
    "gregorian_to_dangi",
    "dangi_to_gregorian",
    "gregorian_to_minguo",
    "minguo_to_gregorian",
    "get_clock_precision",
]
