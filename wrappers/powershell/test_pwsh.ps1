
$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Import-Module "$ScriptDir\universal_timestamp.ps1" -Force

Write-Host "Running PowerShell wrapper tests..." -ForegroundColor Cyan

# Test 1: Get-UtNow
Write-Host "Testing Get-UtNow..." -NoNewline
$Now = Get-UtNow
if ([string]::IsNullOrWhiteSpace($Now)) {
    Write-Error "Get-UtNow returned empty."
}
Write-Host "PASS ($Now)" -ForegroundColor Green

# Test 2: Round Trip
Write-Host "Testing Round Trip..." -NoNewline
$InputStr = "2024-12-14T12:00:00Z"
$Nanos = ConvertFrom-UtIsoString -IsoString $InputStr
$OutputStr = ConvertTo-UtIsoString -Nanos $Nanos

if ($InputStr -eq $OutputStr) {
    Write-Host "PASS" -ForegroundColor Green
} else {
    Write-Error "Round trip failed. Expected '$InputStr', got '$OutputStr'"
}

Write-Host "All PowerShell tests passed!" -ForegroundColor Green
