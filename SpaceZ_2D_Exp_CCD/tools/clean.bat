@echo off
setlocal EnableExtensions
title Clean Build Artifacts - SpaceZ_2D_Exp_CCD

REM ============================================================
REM  Remove ALL build intermediates and outputs of THIS project:
REM    1. .build\                (objects / response files / import libs)
REM    2. Main\Debug\            (exe / pdb / ilk)
REM    3. Tests\Debug\           (test exe / pdb / ilk)
REM    4. Assets\Plugins\x86_64\SpaceZ_2D_Exp_CCD.dll (+ .pdb, + .meta)
REM       Debug and Release share this one output path, so a single
REM       delete covers whichever flavour was built last.
REM  Only files produced by this project's build scripts are deleted.
REM  Sibling C++ projects are NOT touched: each owns its own clean script.
REM ============================================================

REM ---- Locate directories (script lives in tools\, project root is one level up) ----
for %%i in ("%~dp0..") do set "CCD_ROOT=%%~fi"
for %%i in ("%CCD_ROOT%\..") do set "UNITY_ROOT=%%~fi"

set "OUT_PLUGINS=%UNITY_ROOT%\Assets\Plugins\x86_64"
set "OUT_CONSOLE=%CCD_ROOT%\Main\Debug"
set "OUT_TESTS=%CCD_ROOT%\Tests\Debug"
set "BUILD_DIR=%CCD_ROOT%\.build"
set "DLL_NAME=SpaceZ_2D_Exp_CCD.dll"
set "EXE_NAME=SpaceZ_2D_Exp_CCD.exe"
set "PDB_EXE=%EXE_NAME:.exe=.pdb%"
set "EXE_TESTS=SpaceZ_CcdTests.exe"

REM ---- 1. Intermediates ----
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    echo [CLEANED] Intermediates: .build\
)

REM ---- 2. Console program output ----
if exist "%OUT_CONSOLE%" (
    del /q "%OUT_CONSOLE%\%EXE_NAME%" 2>nul
    del /q "%OUT_CONSOLE%\%PDB_EXE%" 2>nul
    del /q "%OUT_CONSOLE%\%EXE_NAME:.exe=.ilk%" 2>nul
    echo [CLEANED] Main\Debug\ executable and debug files
)

REM ---- 3. Test program output ----
if exist "%OUT_TESTS%" (
    del /q "%OUT_TESTS%\%EXE_TESTS%" 2>nul
    del /q "%OUT_TESTS%\%EXE_TESTS:.exe=.pdb%" 2>nul
    del /q "%OUT_TESTS%\%EXE_TESTS:.exe=.ilk%" 2>nul
    echo [CLEANED] Tests\Debug\ executable and debug files
)

REM ---- 4. Unity plugin output (including Unity's same-name .meta) ----
if exist "%OUT_PLUGINS%" (
    del /q "%OUT_PLUGINS%\%DLL_NAME%" 2>nul
    del /q "%OUT_PLUGINS%\%DLL_NAME%.meta" 2>nul
    echo [CLEANED] Assets\Plugins\x86_64\SpaceZ_2D_Exp_CCD.dll including .meta
)

echo [DONE] Clean finished.
exit /b 0
