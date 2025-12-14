# Universal Timestamp Specification (UTS)

**Version:** 1.0.1  
**Status:** Draft  
**Last Updated:** 2024-12-14

---

## 1. Overview

The Universal Timestamp Specification defines a deterministic, cross‑platform, zero‑ambiguity timestamp format and behavior.  
All implementations across all languages MUST conform to this specification exactly.

The goals:

- Deterministic formatting  
- Deterministic parsing  
- Zero timezone ambiguity  
- Zero locale dependency  
- Nanosecond precision  
- Optional monotonic ordering  
- Identical behavior across languages  

---

## 2. Format Specification

### 2.1 Canonical Format (Strict)

All timestamps MUST be formatted as:

```timestamp
YYYY-MM-DDTHH:MM:SSZ
```

| Component | Description | Range |
|-----------|-------------|-------|
| `YYYY` | 4-digit year | 0000–9999 |
| `MM` | Month | 01–12 |
| `DD` | Day | 01–31 (validated per month/year) |
| `T` | Literal separator | — |
| `HH` | Hour (24h) | 00–23 |
| `MM` | Minute | 00–59 |
| `SS` | Second | 00–59 |
| `Z` | UTC designator | Required |

**Example:**  
`2024-12-14T03:13:21Z`

Strict mode MUST accept this format.

---

### 2.2 Extended Format (Nanoseconds)

When sub‑second precision is required:

```timestamp
YYYY-MM-DDTHH:MM:SS.nnnnnnnnnZ
```

| Component | Description | Range |
|-----------|-------------|-------|
| `.nnnnnnnnn` | Fractional seconds | 1–9 digits |

Rules:

- Strict mode MUST accept fractional seconds with 1–9 digits.
- Trailing zeros MAY be omitted.
- Leading zeros MUST be preserved.
- More than 9 digits MUST be rejected in strict mode.
- Lenient mode MAY round or reject >9 digits.

**Examples:**

- `2024-12-14T03:13:21.5Z`
- `2024-12-14T03:13:21.050Z`
- `2024-12-14T03:13:21.123456789Z`

---

### 2.3 Timezone Requirement

All timestamps MUST be in UTC.  
The `Z` suffix is REQUIRED in strict mode.

Lenient mode MAY accept:

- Missing `Z` (assume UTC)
- `z` (lowercase)
- `+00:00` or `-00:00` offsets

Non‑zero offsets MUST be rejected in strict mode.

---

### 2.4 Date Types

All implementations MUST use the **proleptic Gregorian calendar** as the base for all date calculations. Additional Gregorian-derived calendars are supported for display and input purposes.

#### 2.4.1 Supported Calendars

| Calendar | Description | Year Offset | Status |
|----------|-------------|-------------|--------|
| `gregorian` | Proleptic Gregorian (ISO‑8601) | 0 | **Default** |
| `thai` | Thai Solar Calendar (Buddhist Era) | +543 | Supported |
| `dangi` | Korean Dangi Calendar | +2333 | Supported |
| `minguo` | Minguo/ROC Calendar (Taiwan) | −1911 | Supported |
| `japanese` | Japanese Era Calendar (Gengō) | Era-based | Supported |
| `iso_week` | ISO week-date (YYYY-Www-D) | 0 | Supported |

#### 2.4.2 Year Offset Calendars

For `thai`, `dangi`, and `minguo`, the year is derived by applying an offset to the Gregorian year:

```text
thai_year    = gregorian_year + 543
dangi_year   = gregorian_year + 2333
minguo_year  = gregorian_year - 1911
```

**Examples:**

- Gregorian 2024 = Thai 2567 = Dangi 4357 = Minguo 113

#### 2.4.3 Japanese Era Calendar

Japanese years are expressed as era name + year within era:

| Era | Japanese | Start Date | Gregorian Start |
|-----|----------|------------|-----------------|
| Reiwa | 令和 | 2019-05-01 | 2019 |
| Heisei | 平成 | 1989-01-08 | 1989 |
| Shōwa | 昭和 | 1926-12-25 | 1926 |
| Taishō | 大正 | 1912-07-30 | 1912 |
| Meiji | 明治 | 1868-01-25 | 1868 |

**Conversion:**

```text
gregorian_year = era_start_year + era_year - 1
```

**Example:** Reiwa 6 = 2019 + 6 - 1 = 2024

#### 2.4.4 ISO Week Date

The `iso_week` calendar represents dates as year-week-day:

```text
YYYY-Www-D
```

Where:

- `YYYY` = ISO week-numbering year
- `Www` = Week 01–53
- `D` = Day 1–7 (Monday = 1)

**Example:** `2024-W50-6` = Saturday of week 50, 2024 = 2024-12-14

#### 2.4.5 Internal Representation

All calendars share the same internal representation (nanoseconds since Unix epoch). Calendar type only affects parsing and formatting, not storage.

#### 2.4.6 Unsupported Calendars

The following are explicitly out of scope:

- `julian` — Julian calendar (different leap year rules)
- `hebrew` — Hebrew lunisolar calendar
- `islamic` — Islamic lunar calendar
- `chinese` — Chinese lunisolar calendar

Implementations MUST reject these calendar types.

---

## 3. Parsing Rules

### 3.1 Strict Mode

Strict parsing MUST:

- Require exact canonical or extended format
- Require uppercase `Z`
- Reject lowercase `z`
- Reject missing `Z`
- Reject offsets (`+00:00`, `-00:00`)
- Reject whitespace
- Reject invalid dates (e.g., Feb 30)
- Reject leap seconds (`SS = 60`)
- Reject fractional seconds >9 digits
- Reject out‑of‑range values

**Accepted:**

```timestamp
2024-12-14T03:13:21Z
2024-12-14T03:13:21.123456789Z
```

**Rejected:**

```timestamp
2024-12-14T03:13:21
2024-12-14T03:13:21z
2024-12-14T03:13:21+00:00
2024-12-14 03:13:21Z
2024-12-14T25:13:21Z
2024-12-14T03:13:21.123456789123Z
```

---

### 3.2 Lenient Mode

Lenient parsing MUST:

- Accept missing `Z` (assume UTC)
- Accept lowercase `z`
- Accept `+00:00` or `-00:00`
- Accept optional fractional seconds
- Reject invalid dates
- Reject out‑of‑range values
- MAY round or reject >9 fractional digits

**Additionally accepted:**

```timestamp
2024-12-14T03:13:21
2024-12-14T03:13:21z
2024-12-14T03:13:21+00:00
2024-12-14T03:13:21-00:00
```

---

### 3.3 Error Handling

Parsing MUST return structured error codes:

| Code | Description |
|------|-------------|
| `INVALID_FORMAT` | Pattern mismatch |
| `INVALID_DATE` | Calendar date invalid |
| `OUT_OF_RANGE` | Component outside valid range |
| `UNSUPPORTED_OFFSET` | Offset not allowed in strict mode |
| `FRACTION_TOO_LONG` | More than 9 fractional digits |
| `LEAP_SECOND_UNSUPPORTED` | Second = 60 |

---

## 4. Monotonic Guarantee

### 4.1 Purpose

System clocks can move backwards due to:

- NTP adjustments  
- Manual changes  
- VM migration  
- Clock drift correction  

Monotonic mode ensures timestamps never regress.

---

### 4.2 Monotonic Algorithm

```c
function generateMonotonic():
    current = getSystemTimeUTC()

    if current <= lastTimestamp:
        lastTimestamp = lastTimestamp + 1 nanosecond
    else:
        lastTimestamp = current

    return lastTimestamp
```

---

### 4.3 Requirements

1. **Thread Safety:**  
   Implementations MUST ensure atomic updates to `lastTimestamp`.

2. **Overflow Handling:**  
   If nanoseconds overflow, increment seconds.

3. **Initialization:**  
   First call uses system time.

4. **Process Lifetime:**  
   Monotonic state resets on process restart.

5. **Regression Callback:**  
   Implementations MUST provide:

```c
onClockRegression(expected, actual, adjusted)
```

---

## 5. Zero Dependencies

### 5.1 Prohibited

Implementations MUST NOT depend on:

| Dependency | Reason |
|------------|--------|
| Locale data | Non-deterministic |
| Timezone databases | External state |
| ICU/CLDR | Heavy, inconsistent |
| System date libraries | Platform variance |

### 5.2 Allowed

- System UTC time APIs  
- Standard library integer/string ops  
- Memory allocation  

### 5.3 Recommended Time Sources

| Platform | API |
|----------|-----|
| POSIX | `clock_gettime(CLOCK_REALTIME)` |
| Windows | `GetSystemTimePreciseAsFileTime()` |
| JS | `Date.now()` (ms precision) |
| Embedded | RTC |

---

## 6. Cross‑Language Conformance

### 6.1 Required API Surface

All implementations MUST provide:

| Function | Description |
|----------|-------------|
| `now()` | Current UTC timestamp |
| `nowMonotonic()` | Monotonic timestamp |
| `parse(string, strict)` | Parse timestamp |
| `format(timestamp)` | Canonical formatting |
| `fromUnixNanos(int64)` | Convert from epoch |
| `toUnixNanos(timestamp)` | Convert to epoch |
| `getClockPrecision()` | Detect hardware precision (0=ns, 1=µs, 2=ms, 3=s, -1=error) |

---

### 6.2 Required Test Vectors

#### Formatting

| Unix nanos | Expected |
|------------|----------|
| 0 | `1970-01-01T00:00:00Z` |
| 1000000000000000000 | `2001-09-09T01:46:40Z` |
| 1734146001123456789 | `2024-12-14T03:13:21.123456789Z` |

#### Parsing (Strict)

| Input | Valid | Unix nanos |
|-------|-------|------------|
| `1970-01-01T00:00:00Z` | ✓ | 0 |
| `2024-12-14T03:13:21Z` | ✓ | 1734146001000000000 |
| `2024-12-14T03:13:21.5Z` | ✓ | 1734146001500000000 |
| `2024-12-14T03:13:21` | ✗ | — |
| `2024-02-30T00:00:00Z` | ✗ | — |

#### Parsing (Lenient)

| Input | Valid | Unix nanos |
|-------|-------|------------|
| `2024-12-14T03:13:21` | ✓ | 1734146001000000000 |
| `2024-12-14T03:13:21+00:00` | ✓ | 1734146001000000000 |
| `2024-02-30T00:00:00Z` | ✗ | — |

#### Monotonic Sequence

Given mock clock:

1. `1000000000`
2. `500000000`
3. `1000000001`

Expected:

1. `1000000000`
2. `1000000001`
3. `1000000002`

---

## 7. Implementation Checklist

### Required Features

- [ ] Canonical formatting  
- [ ] Nanosecond formatting  
- [ ] Strict parsing  
- [ ] Lenient parsing  
- [ ] Monotonic timestamp generation  
- [ ] Regression callback  
- [ ] Unix nanos conversion  
- [ ] Thread safety  
- [ ] Zero external dependencies  

### Required Tests

- [ ] Format vectors  
- [ ] Strict parse vectors  
- [ ] Lenient parse vectors  
- [ ] Monotonic sequence  
- [ ] Regression callback  
- [ ] Invalid date rejection  
- [ ] Fractional precision  
- [ ] Thread safety  

---

## Appendix A: Grammar (EBNF)

```ebnf
timestamp      = date "T" time "Z"
date           = year "-" month "-" day
time           = hour ":" minute ":" second [fraction]
year           = 4DIGIT
month          = 2DIGIT  ; 01-12
day            = 2DIGIT  ; 01-31
hour           = 2DIGIT  ; 00-23
minute         = 2DIGIT  ; 00-59
second         = 2DIGIT  ; 00-59
fraction       = "." 1*9DIGIT
```

---

## Appendix B: Design Rationale

- Nanoseconds align with modern OS APIs  
- UTC eliminates ambiguity  
- Strict/lenient modes allow flexibility without sacrificing determinism  
- Monotonic mode solves clock regression  
- Zero dependencies ensure reproducibility
