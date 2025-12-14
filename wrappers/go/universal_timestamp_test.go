package universal_timestamp

import (
	"testing"
	"time"
)

func TestNow(t *testing.T) {
	ts := Now()
	if ts == 0 {
		t.Error("Now() returned 0")
	}
}

func TestParseFormat(t *testing.T) {
	input := "2024-12-14T12:00:00Z"
	ts, err := Parse(input)
	if err != nil {
		t.Fatalf("Parse failed: %v", err)
	}

	output := ts.Format()
	if input != output {
		t.Errorf("Round trip failed. Expected %s, got %s", input, output)
	}
}

func TestToTime(t *testing.T) {
	now := time.Now().UTC()
	ts := FromTime(now)
	goTime := ts.ToTime()

	if !goTime.Equal(now) {
		t.Errorf("Time conversion failed. Expected %v, got %v", now, goTime)
	}
}
