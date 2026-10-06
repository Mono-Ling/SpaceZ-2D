@echo off
setlocal EnableExtensions EnableDelayedExpansion
title Build Console Program - SpaceZ_2D_Alpha

REM ============================================================
REM  Recursively compile Core + Main into a console program
REM  (same name as the C++ project) and output to:
REM    Main\Debug\SpaceZ_2D_Alpha.exe (+.pdb / .ilk)
REM  Intermediates: .build\obj_console\ (cleaned by clean.bat)
REM  NOTE: objects are flattened by source base name, so do NOT
REM        use the same .cpp file name in different sub-folders.
REM ============================================================

REM ---- Locate directories (script lives in tools\, project root is one level up) ----
for %%i in ("%~dp0..") do set "ALPHA_ROOT=%%~fi"
for %%i in ("%ALPHA_ROOT%\..") do set "UNITY_ROOT=%%~fi"

set "SRC_CORE=%ALPHA_ROOT%\Core"
set "SRC_MAIN=%ALPHA_ROOT%\Main"
set "OUT_DIR=%SRC_MAIN%\Debug"
set "BUILD_DIR=%ALPHA_ROOT%\.build"
set "OBJ_DIR=%BUILD_DIR%\obj_console"
set "EXE_NAME=SpaceZ_2D_Alpha.exe"
set "PDB_NAME=%EXE_NAME:.exe=.pdb%"

REM ---- [1/4] Locate MSVC x64 toolchain ----
echo [1/4] Locating MSVC toolchain...
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found. Install Visual Studio 2017+ with the "Desktop development with C++" workload.
    exit /b 1
)
set "VS_ROOT="
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_ROOT=%%i"
if not defined VS_ROOT (
    echo [ERROR] No Visual Studio instance with the MSVC C++ toolset was found.
    exit /b 1
)
call "%VS_ROOT%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to initialize the MSVC x64 environment.
    exit /b 1
)

REM ---- [2/4] Collect sources recursively (Core, Main) ----
echo [2/4] Collecting sources: Core, Main ...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
set "RSP=%BUILD_DIR%\sources_console.rsp"
type nul > "%RSP%"
for /f "delims=" %%f in ('dir /s /b "%SRC_CORE%\*.cpp" 2^>nul') do echo "%%f">>"%RSP%"
for /f "delims=" %%f in ('dir /s /b "%SRC_MAIN%\*.cpp" 2^>nul') do echo "%%f">>"%RSP%"

REM Count lines with built-in commands only (never call external find.exe,
REM whose path may be shadowed by GNU find from a Unix-like shell PATH).
set "SRC_COUNT=0"
for /f "usebackq delims=" %%f in (`type "%RSP%"`) do set /a SRC_COUNT+=1
if "%SRC_COUNT%"=="0" (
    echo [SKIP] No .cpp files found under Core or Main, executable not built.
    exit /b 0
)
echo        %SRC_COUNT% source file(s) found.

REM ---- [3/4] Compile (Debug x64) ----
echo [3/4] Compiling (Debug ^| x64) ...
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"
cl /nologo /std:c++20 /utf-8 /EHsc /W4 /Zi /FS /Od /RTC1 /MDd /D_DEBUG /D_UNICODE /DUNICODE /I"%SRC_CORE%" /c /Fo"%OBJ_DIR%\\" /Fd"%OBJ_DIR%\\" @"%RSP%"
if errorlevel 1 (
    echo [ERROR] Compilation failed.
    exit /b 1
)

REM ---- [4/4] Link into a console program ----
echo [4/4] Linking %EXE_NAME% ...
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
set "OBJS="
for /r "%OBJ_DIR%" %%o in (*.obj) do set "OBJS=!OBJS! "%%o""

link /nologo /DEBUG /INCREMENTAL:NO /OUT:"%OUT_DIR%\%EXE_NAME%" /PDB:"%OUT_DIR%\%PDB_NAME%" %OBJS%
if errorlevel 1 (
    echo [ERROR] Linking failed.
    exit /b 1
)

echo [DONE] Generated %OUT_DIR%\%EXE_NAME%
exit /b 0
