@echo off
setlocal EnableExtensions
title Clean Build Artifacts - SpaceZ_2D_Alpha

REM ============================================================
REM  Remove ALL build intermediates and outputs:
REM    1. .build\            (objects / response files / import libs)
REM    2. Main\Debug\        (exe / pdb / ilk)
REM    3. Assets\Plugins\x86_64\ (SpaceZ_2D_Alpha.dll, EditorPlugin.dll
REM       and their pdb, plus Unity's same-name .meta)
REM  Only files produced by the build scripts are deleted.
REM ============================================================

REM ---- Locate directories (script lives in tools\, project root is one level up) ----
for %%i in ("%~dp0..") do set "ALPHA_ROOT=%%~fi"
for %%i in ("%ALPHA_ROOT%\..") do set "UNITY_ROOT=%%~fi"

set "OUT_PLUGINS=%UNITY_ROOT%\Assets\Plugins\x86_64"
set "OUT_CONSOLE=%ALPHA_ROOT%\Main\Debug"
set "BUILD_DIR=%ALPHA_ROOT%\.build"
set "DLL_NAME=SpaceZ_2D_Alpha.dll"
set "DLL_EDITOR=EditorPlugin.dll"
set "EXE_NAME=SpaceZ_2D_Alpha.exe"
set "PDB_EXE=%EXE_NAME:.exe=.pdb%"

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

REM ---- 3. Unity plugin output (including Unity's same-name .meta) ----
if exist "%OUT_PLUGINS%" (
    del /q "%OUT_PLUGINS%\%DLL_NAME%" 2>nul
    del /q "%OUT_PLUGINS%\%DLL_NAME%.meta" 2>nul
    del /q "%OUT_PLUGINS%\%DLL_EDITOR%" 2>nul
    del /q "%OUT_PLUGINS%\%DLL_EDITOR%.meta" 2>nul
    echo [CLEANED] Assets\Plugins\x86_64\ dll files including .meta
)

echo [DONE] Clean finished.
exit /b 0
