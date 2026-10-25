@echo off
setlocal EnableExtensions EnableDelayedExpansion
title Build Unity Plugin DLL (Release) - SpaceZ_2D_Exp_CCD

REM ============================================================
REM  Recursively compile Core + UnityPlugin into ONE optimized DLL
REM  (same name as the C++ project) and copy to:
REM    <UnityRoot>\Assets\Plugins\x86_64\SpaceZ_2D_Exp_CCD.dll
REM
REM  NOTE: this is the SAME output path as build_plugins.bat (Debug).
REM        The two configurations OVERWRITE each other on purpose, so
REM        the Unity-side .meta / GUID stays stable and no C# reference
REM        breaks. Only one flavour can live there at a time.
REM
REM  Intermediates: .build\obj_plugins_release\  (MUST stay separate
REM        from obj_plugins\ -- mixing /MD and /MDd objects in one
REM        link fails with LNK2038 RuntimeLibrary mismatch)
REM  PDB / import lib: .build\release\ (kept out of Unity assets)
REM
REM  Optimisation: /O2 /GL + link /LTCG. Measured on this machine,
REM  Matrix3x3 multiplication drops from ~57.7 ns/op (/Od) to ~2.0 ns/op.
REM  /RTC1 and /ZI are REMOVED (both are D8016-incompatible with the
REM  release options); /Zi is kept so crash stacks stay symbolicated.
REM
REM  Runtime: /MD. Requires the VC++ 2015-2022 x64 redistributable,
REM  which the Unity Editor already ships. Switch to /MT if the DLL
REM  must be fully self-contained on arbitrary machines.
REM ============================================================

REM ---- Locate directories (script lives in tools\, project root is one level up) ----
for %%i in ("%~dp0..") do set "CCD_ROOT=%%~fi"
for %%i in ("%CCD_ROOT%\..") do set "UNITY_ROOT=%%~fi"

set "SRC_CORE=%CCD_ROOT%\Core"
set "SRC_PLUGIN=%CCD_ROOT%\UnityPlugin"
set "OUT_DIR=%UNITY_ROOT%\Assets\Plugins\x86_64"
set "BUILD_DIR=%CCD_ROOT%\.build"
set "OBJ_DIR=%BUILD_DIR%\obj_plugins_release"
set "REL_DIR=%BUILD_DIR%\release"
set "DLL_NAME=SpaceZ_2D_Exp_CCD.dll"
set "PDB_NAME=%DLL_NAME:.dll=.pdb%"
set "LIB_NAME=%DLL_NAME:.dll=.lib%"

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

REM ---- [2/4] Collect sources recursively (Core, UnityPlugin) ----
echo [2/4] Collecting sources: Core, UnityPlugin ...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
set "RSP=%BUILD_DIR%\sources_plugins_release.rsp"
type nul > "%RSP%"
for /f "delims=" %%f in ('dir /s /b "%SRC_CORE%\*.cpp" 2^>nul') do echo "%%f">>"%RSP%"
for /f "delims=" %%f in ('dir /s /b "%SRC_PLUGIN%\*.cpp" 2^>nul') do echo "%%f">>"%RSP%"

REM Count lines with built-in commands only (never call external find.exe,
REM whose path may be shadowed by GNU find from a Unix-like shell PATH).
set "SRC_COUNT=0"
for /f "usebackq delims=" %%f in (`type "%RSP%"`) do set /a SRC_COUNT+=1
if "%SRC_COUNT%"=="0" (
    echo [SKIP] No .cpp files found under Core or UnityPlugin, DLL not built.
    exit /b 0
)
echo        %SRC_COUNT% source file(s) found.

REM ---- [3/4] Compile (Release x64) ----
echo [3/4] Compiling (Release ^| x64 ^| /O2 /GL) ...
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"
if not exist "%REL_DIR%" mkdir "%REL_DIR%"
cl /nologo /std:c++20 /utf-8 /EHsc /W4 /Zi /FS /O2 /GL /Gy /MD /DNDEBUG /D_UNICODE /DUNICODE /I"%SRC_CORE%" /I"%SRC_PLUGIN%" /c /Fo"%OBJ_DIR%\\" /Fd"%OBJ_DIR%\\" @"%RSP%"
if errorlevel 1 (
    echo [ERROR] Compilation failed.
    exit /b 1
)

REM ---- [4/4] Link into a single DLL (whole-program optimisation) ----
echo [4/4] Linking %DLL_NAME% (Release) ...
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

REM Pre-delete the old dll first; if it survives deletion, it is locked
REM (almost always loaded by the Unity Editor) and relinking would fail
REM with an opaque LNK1104.
if exist "%OUT_DIR%\%DLL_NAME%" del /f "%OUT_DIR%\%DLL_NAME%" 2>nul
if exist "%OUT_DIR%\%DLL_NAME%" (
    echo [ERROR] %DLL_NAME% is locked, probably loaded by Unity Editor. Close Unity, then rebuild.
    exit /b 1
)

set "OBJS="
for /r "%OBJ_DIR%" %%o in (*.obj) do set "OBJS=!OBJS! "%%o""

link /nologo /DLL /LTCG /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF /OUT:"%OUT_DIR%\%DLL_NAME%" /PDB:"%REL_DIR%\%PDB_NAME%" /IMPLIB:"%REL_DIR%\%LIB_NAME%" %OBJS%
if errorlevel 1 (
    echo [ERROR] Linking failed.
    exit /b 1
)

echo [DONE] Generated %OUT_DIR%\%DLL_NAME%  (Release ^| /O2 /GL /LTCG)
echo        This overwrites the Debug DLL at the same path.
exit /b 0
