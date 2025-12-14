# PowerShell Wrapper

The PowerShell wrapper provides a convenient way to use Universal Timestamp in Windows environments. It wraps the `uts-cli.exe` binary.

## Installation

1. Build `uts-cli.exe` using `compile.bat` in `wrappers/powershell`.
2. Import the module:

```powershell
Import-Module .\wrappers\powershell\universal_timestamp.ps1
```

## Usage

```powershell
# Get current time
Get-UtNow

# Get nanoseconds
Get-UtNowNanos

# Parse string
ConvertFrom-UtIsoString "2024-12-14T12:00:00Z"

# Format nanoseconds
ConvertTo-UtIsoString 1734146000000000000
```
