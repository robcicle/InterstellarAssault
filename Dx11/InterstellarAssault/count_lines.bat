@echo off
setlocal enabledelayedexpansion

:: Set the extension types to search
set "EXTENSIONS=.cpp .h .lua"

:: Count variables
set /a "TOTAL_LINES=0"
set /a "TOTAL_CODE_LINES=0"

:: Temporary files
set "ALLFILES=%TEMP%\allfiles.txt"
set "FILTEREDFILES=%TEMP%\filteredfiles.txt"

:: Find all files with specified extensions and save the list
for %%e in (%EXTENSIONS%) do (
    dir /S /B /A:-D *%%e >> "%ALLFILES%"
)

:: Process each file
for /F "tokens=*" %%F in (%ALLFILES%) do (
    set /a "FILE_LINES=0"
    set /a "CODE_LINES=0"

    :: Count total lines in file
    for /F %%A in ('type "%%F" ^| find /c /v ""') do set /a FILE_LINES=%%A

    :: Filter out single-line comments and count code lines
    findstr /R /V "^[[:space:]]*// ^[[:space:]]*\/\*.*\*\/ ^[[:space:]]*--" "%%F" > "%FILTEREDFILES%"
    for /F %%A in ('type "%FILTEREDFILES%" ^| find /c /v ""') do set /a CODE_LINES=%%A

    :: Update totals
    set /a TOTAL_LINES+=FILE_LINES
    set /a TOTAL_CODE_LINES+=CODE_LINES

    :: Display per file info (optional)
    echo File: %%F
    echo Total Lines: !FILE_LINES!
    echo Code Lines: !CODE_LINES!
    echo ---------------------------
)

:: Display total count
echo Total Lines in All Files: %TOTAL_LINES%
echo Total Code Lines in All Files: %TOTAL_CODE_LINES%

:: Clean up temporary files
del "%ALLFILES%"
del "%FILTEREDFILES%"

:: Pause the script to view the results
echo Press any key to exit...
pause

endlocal