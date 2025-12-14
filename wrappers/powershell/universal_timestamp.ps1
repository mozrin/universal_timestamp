
# Universal Timestamp PowerShell Wrapper
$ErrorActionPreference = "Stop"

# Define path to CLI tool
if ($env:UTS_CLI_PATH) {
    $UtsCli = $env:UTS_CLI_PATH
} else {
    $ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    $LocalCli = Join-Path $ScriptDir "uts-cli.exe"
    if (Test-Path $LocalCli) {
        $UtsCli = $LocalCli
    } else {
        Write-Error "uts-cli.exe not found. Set env:UTS_CLI_PATH or place it in the same directory."
    }
}

function Get-UtNow {
    <#
    .SYNOPSIS
    Get current UTC timestamp in ISO-8601 format.
    #>
    & $UtsCli "now"
}

function Get-UtNowNanos {
    <#
    .SYNOPSIS
    Get current UTC timestamp in nanoseconds.
    #>
    & $UtsCli "now-nanos"
}

function ConvertFrom-UtIsoString {
    <#
    .SYNOPSIS
    Parse ISO-8601 string to nanoseconds.
    #>
    param([string]$IsoString)
    & $UtsCli "parse" $IsoString
}

function ConvertTo-UtIsoString {
    <#
    .SYNOPSIS
    Format nanoseconds to ISO-8601 string.
    #>
    param([long]$Nanos)
    & $UtsCli "format" $Nanos
}

Export-ModuleMember -Function Get-UtNow, Get-UtNowNanos, ConvertFrom-UtIsoString, ConvertTo-UtIsoString
