@echo off
setlocal EnableExtensions
title Clean Build Artifacts - EditorPlugin

REM ============================================================
REM  Remove everything produced by build_editor_plugin.bat:
REM    1. .build\   (objects / response file / pdb / lib / exp)
REM    2. <UnityRoot>\Assets\Plugins\x86_64\EditorPlugin.dll
REM       (plus Unity's same-name .meta)
REM  Unity assets other than this DLL are never touched.
REM ============================================================

REM ---- Locate directories (script lives in tools\, project root is one level up) ----
for %%i in ("%~dp0..") do set "PLUGIN_ROOT=%%~fi"
for %%i in ("%PLUGIN_ROOT%\..") do set "UNITY_ROOT=%%~fi"

set "BUILD_DIR=%PLUGIN_ROOT%\.build"
set "OUT_DIR=%UNITY_ROOT%\Assets\Plugins\x86_64"
set "DLL_NAME=EditorPlugin.dll"

REM ---- 1. Intermediates ----
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    echo [CLEANED] Intermediates: .build\
)

REM ---- 2. DLL output in Unity assets ----
if exist "%OUT_DIR%\%DLL_NAME%" (
    del /q "%OUT_DIR%\%DLL_NAME%" 2>nul
    if exist "%OUT_DIR%\%DLL_NAME%" (
        echo [ERROR] %DLL_NAME% is locked, probably loaded by Unity Editor. Close Unity, then clean.
        exit /b 1
    )
    del /q "%OUT_DIR%\%DLL_NAME%.meta" 2>nul
    echo [CLEANED] %OUT_DIR%\%DLL_NAME%
)

echo [DONE] Clean finished.
exit /b 0
