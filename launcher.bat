@echo off
TITLE RVCE Campus Cleaning Scheduler
COLOR 0A
CLS

REM Check if GCC exists
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] GCC Compiler not found!
    echo Please install MinGW or TDM-GCC to run C programs.
    echo.
    PAUSE
    EXIT
)

echo ==========================================
echo       Compiling Campus Scheduler...
echo ==========================================

REM Compile the program
gcc main.c campus.c -o cleaner_app.exe

REM Check if compilation was successful
if exist cleaner_app.exe (
    echo [SUCCESS] Compilation complete. Launching...
    echo.
    timeout /t 2 >nul
    
    REM Run the executable
    cleaner_app.exe
    
    REM Pause so the window doesn't close immediately upon exit
    echo.
    echo ==========================================
    echo    Program exited. Press any key to close.
    pause >nul
) else (
    echo.
    echo [ERROR] Compilation failed. Please check your code for errors.
    PAUSE
)