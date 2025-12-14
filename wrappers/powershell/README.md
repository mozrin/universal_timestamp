# Universal Timestamp PowerShell Wrapper

A PowerShell wrapper for the Universal Timestamp library.

## Requirements

- Windows 10/11 or PowerShell Core on Linux/macOS
- `uts-cli.exe` (built from `src/cli/uts_cli.c`)

## Setup

1. Build the library and CLI:
   - If you have MinGW/MSYS2: `make` (in root) and `compile.bat` (in this dir)
   - If you have MSVC: Run `compile.bat` from Developer Command Prompt
2. Import the module:

```powershell
Import-Module .\universal_timestamp.ps1
```

## Usage

```powershell
# Get current time
$Now = Get-UtNow
Write-Host "Now: $Now"

# Parse string
$Nanos = ConvertFrom-UtIsoString "2024-01-01T00:00:00Z"

# Format nanos
$Str = ConvertTo-UtIsoString $Nanos
```

## Testing

Run `test_pwsh.ps1`.
