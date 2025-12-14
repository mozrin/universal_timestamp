@echo off
setlocal

set "SRC=..\..\src\cli\uts_cli.c"
set "OUT=uts-cli.exe"
set "INC=..\..\include"
set "LIB=..\..\dist\universal_timestamp.lib"

echo Compiling %OUT%...

REM Try MSVC
cl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using MSVC...
    cl /nologo /O2 /I"%INC%" "%SRC%" "%LIB%" /Fe"%OUT%"
    if %ERRORLEVEL% EQU 0 (
        echo Build successful.
        del *.obj
        exit /b 0
    )
)

REM Try MinGW/GCC
gcc --version >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using GCC...
    gcc -O2 -I"%INC%" "%SRC%" -o "%OUT%" -L..\..\dist -l:libuniversal_timestamp.a
    if %ERRORLEVEL% EQU 0 (
        echo Build successful.
        exit /b 0
    )
)

echo Error: No suitable compiler found (MSVC cl or GCC).
exit /b 1
