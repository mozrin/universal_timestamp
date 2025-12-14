use universal_timestamp::Timestamp;

#[test]
fn test_integration_now() {
    let now = Timestamp::now();
    assert!(now.as_nanos() > 0);
}

#[test]
fn test_integration_parse_format() {
    let ts_str = "2024-12-14T12:00:00Z";
    let ts = Timestamp::parse(ts_str).expect("Failed to parse");
    // Format usually mimics input if strict ISO
    assert_eq!(ts.format(false), "2024-12-14T12:00:00Z");
}

#[test]
fn test_integration_monotonic() {
    let t1 = Timestamp::now_monotonic();
    let t2 = Timestamp::now_monotonic();
    assert!(t2 > t1);
}

#[test]
fn test_calendar() {
    let thai_year = universal_timestamp::calendar::gregorian_to_thai(2024);
    assert_eq!(thai_year, 2567);
}
