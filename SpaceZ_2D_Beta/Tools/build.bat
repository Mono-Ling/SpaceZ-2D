@echo off
setlocal EnableExtensions EnableDelayedExpansion
title Build SpaceZ_2D_Beta - %~1

REM ============================================================
REM  Single build implementation. The per-target .bat files in this
REM  folder are thin wrappers that just call:
REM     build.bat console | tests | plugins | plugins-release | clean
REM
REM  Targets and outputs
REM     console          Main\Debug\SpaceZ_2D_Beta.exe       + .pdb
REM     tests            Tests\Debug\SpaceZ_2D_BetaTests.exe  + .pdb
REM     plugins          <UnityRoot>\Assets\Plugins\x86_64\SpaceZ_2D_Beta.dll
REM     plugins-release  same DLL path as "plugins", it overwrites it
REM     clean            remove this project's intermediates and outputs
REM
REM  OBJECT LAYOUT - the part that matters
REM     Object files MIRROR the source tree under .build\obj_<target>\:
REM       Core\Debug\Foo.cpp         -> .build\obj_plugins\Core\Debug\Foo.obj
REM       UnityPlugin\Debug\Foo.cpp  -> .build\obj_plugins\UnityPlugin\Debug\Foo.obj
REM
REM     cl names every .obj after the SOURCE BASE NAME only. With a single
REM     flat /Fo directory, Core\Debug\Foo.obj and UnityPlugin\Debug\Foo.obj
REM     land on the SAME file: the later compile silently overwrites the
REM     earlier one and the link then fails with LNK2005/LNK2019 on symbols
REM     that obviously exist - or, worse, links the wrong object and fails
REM     at run time. Mirroring the path keeps every module isolated, and
REM     also keeps two same-named sub-folders inside ONE module apart
REM     (Core\Debug\A.cpp vs Core\Math\A.cpp).
REM
REM     Cost: one cl invocation per source DIRECTORY, not per file.
REM
REM  Every intermediate stays under .build\ and is removed by "clean".
REM ============================================================

set "TARGET=%~1"
if "%TARGET%"=="" goto :usage

REM ---- Locate directories (script lives in Tools\, project root is one level up) ----
for %%i in ("%~dp0..") do set "BETA_ROOT=%%~fi"
for %%i in ("%BETA_ROOT%\..") do set "UNITY_ROOT=%%~fi"

REM cl /I and link /OUT arguments below are deliberately UNQUOTED (quoting a
REM path that already sits inside set "VAR=..." is fragile), so reject a
REM project path containing a space instead of failing later with a
REM half-parsed command line.
if not "%BETA_ROOT%"=="%BETA_ROOT: =%" (
    echo [ERROR] The project path must not contain a space: %BETA_ROOT%
    exit /b 1
)

set "SRC_CORE=%BETA_ROOT%\Core"
set "SRC_PLUGIN=%BETA_ROOT%\UnityPlugin"
set "BUILD_DIR=%BETA_ROOT%\.build"
set "OUT_PLUGINS=%UNITY_ROOT%\Assets\Plugins\x86_64"
set "DLL_NAME=SpaceZ_2D_Beta.dll"
set "PDB_NAME=%DLL_NAME:.dll=.pdb%"
set "LIB_NAME=%DLL_NAME:.dll=.lib%"

if /I "%TARGET%"=="console"         goto :target_console
if /I "%TARGET%"=="tests"           goto :target_tests
if /I "%TARGET%"=="plugins"         goto :target_plugins
if /I "%TARGET%"=="plugins-release" goto :target_plugins_release
if /I "%TARGET%"=="clean"           goto :target_clean
echo [ERROR] Unknown target "%TARGET%".
goto :usage

REM ============================================================
REM  Target definitions. Each one sets:
REM    MODULES     source roots to compile, mirrored under OBJ_DIR
REM    OBJ_DIR     private intermediate directory for this target
REM    CFLAGS      compiler options, including every /I search root
REM    LINK_MODE   exe | dll
REM  Debug and Release objects NEVER share a directory: mixing /MDd and
REM  /MD objects in one link fails with LNK2038 RuntimeLibrary mismatch.
REM ============================================================

:target_console
set "MODULES=Core Main"
set "OBJ_DIR=%BUILD_DIR%\obj_console"
set "CFLAGS=/nologo /c /std:c++20 /utf-8 /EHsc /W4 /Zi /FS /Od /RTC1 /MDd /D_DEBUG /D_UNICODE /DUNICODE /I%BETA_ROOT% /I%SRC_CORE%"
set "LINK_MODE=exe"
set "OUT_EXE=%BETA_ROOT%\Main\Debug\SpaceZ_2D_Beta.exe"
set "OUT_PDB=%BETA_ROOT%\Main\Debug\SpaceZ_2D_Beta.pdb"
goto :build

:target_tests
set "MODULES=Core Tests"
set "OBJ_DIR=%BUILD_DIR%\obj_tests"
set "CFLAGS=/nologo /c /std:c++20 /utf-8 /EHsc /W4 /Zi /FS /Od /RTC1 /MDd /D_DEBUG /D_UNICODE /DUNICODE /I%BETA_ROOT% /I%SRC_CORE%"
set "LINK_MODE=exe"
set "OUT_EXE=%BETA_ROOT%\Tests\Debug\SpaceZ_2D_BetaTests.exe"
set "OUT_PDB=%BETA_ROOT%\Tests\Debug\SpaceZ_2D_BetaTests.pdb"
goto :build

:target_plugins
if not exist "%UNITY_ROOT%\Assets" goto :bad_layout
set "MODULES=Core UnityPlugin"
set "OBJ_DIR=%BUILD_DIR%\obj_plugins"
set "CFLAGS=/nologo /c /std:c++20 /utf-8 /EHsc /W4 /Zi /FS /Od /RTC1 /MDd /D_DEBUG /D_UNICODE /DUNICODE /I%BETA_ROOT% /I%SRC_CORE% /I%SRC_PLUGIN%"
set "LINK_MODE=dll"
set "LINK_EXTRA="
set "OUT_DLL=%OUT_PLUGINS%\%DLL_NAME%"
set "OUT_DLL_PDB=%BUILD_DIR%\%PDB_NAME%"
set "OUT_DLL_LIB=%BUILD_DIR%\%LIB_NAME%"
goto :build

:target_plugins_release
if not exist "%UNITY_ROOT%\Assets" goto :bad_layout
set "MODULES=Core UnityPlugin"
set "OBJ_DIR=%BUILD_DIR%\obj_plugins_release"
set "CFLAGS=/nologo /c /std:c++20 /utf-8 /EHsc /W4 /Zi /FS /O2 /GL /Gy /MD /DNDEBUG /D_UNICODE /DUNICODE /I%BETA_ROOT% /I%SRC_CORE% /I%SRC_PLUGIN%"
set "LINK_MODE=dll"
set "LINK_EXTRA=/LTCG /OPT:REF /OPT:ICF"
set "OUT_DLL=%OUT_PLUGINS%\%DLL_NAME%"
set "OUT_DLL_PDB=%BUILD_DIR%\release\%PDB_NAME%"
set "OUT_DLL_LIB=%BUILD_DIR%\release\%LIB_NAME%"
if not exist "%BUILD_DIR%\release" mkdir "%BUILD_DIR%\release"
goto :build

REM ============================================================
REM  Build pipeline
REM ============================================================

:build
REM ---- [1/3] Locate MSVC x64 toolchain ----
echo [1/3] Locating MSVC toolchain...
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found. Install Visual Studio 2017+ with the Desktop development with C++ workload.
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

REM ---- [2/3] Compile every module into a mirrored object tree ----
echo [2/3] Compiling modules: %MODULES% ...
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"

REM Everything is recompiled from scratch on every run, so dropping the
REM previous objects first costs nothing and guarantees that a deleted or
REM renamed .cpp can never leave a stale .obj behind for the linker.
del /s /q "%OBJ_DIR%\*.obj" >nul 2>&1

set "TOTAL_SRC=0"
set "FAILED=0"
for %%m in (%MODULES%) do (
    if "!FAILED!"=="0" call :compile_module "%BETA_ROOT%\%%m"
)
if "!FAILED!"=="1" (
    echo [ERROR] Compilation failed.
    exit /b 1
)
if "!TOTAL_SRC!"=="0" (
    echo [SKIP] No .cpp file found under %MODULES%, nothing to build.
    exit /b 0
)
echo        !TOTAL_SRC! source file(s) compiled.

REM ---- [3/3] Link ----
echo [3/3] Linking ...
set "LINKRSP=%BUILD_DIR%\link_%TARGET%.rsp"
type nul > "%LINKRSP%"
for /r "%OBJ_DIR%" %%o in (*.obj) do echo "%%o">>"%LINKRSP%"

if /I "%LINK_MODE%"=="dll" goto :link_dll

:link_exe
for %%i in ("%OUT_EXE%") do set "OUT_DIR=%%~dpi"
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
link /nologo /DEBUG /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /OUT:"%OUT_EXE%" /PDB:"%OUT_PDB%" @"%LINKRSP%"
if errorlevel 1 (
    echo [ERROR] Linking failed.
    exit /b 1
)
echo [DONE] Generated %OUT_EXE%
goto :done

:link_dll
if not exist "%OUT_PLUGINS%" mkdir "%OUT_PLUGINS%"

REM Pre-delete the old DLL. If it survives the delete it is locked, almost
REM always by the Unity Editor that already loaded it, and relinking would
REM fail with an opaque LNK1104 instead of a useful message.
if exist "%OUT_DLL%" del /f "%OUT_DLL%" 2>nul
if exist "%OUT_DLL%" (
    echo [ERROR] %DLL_NAME% is locked, probably loaded by Unity Editor. Close Unity, then rebuild.
    exit /b 1
)

link /nologo /DLL /DEBUG /INCREMENTAL:NO %LINK_EXTRA% /OUT:"%OUT_DLL%" /PDB:"%OUT_DLL_PDB%" /IMPLIB:"%OUT_DLL_LIB%" @"%LINKRSP%"
if errorlevel 1 (
    echo [ERROR] Linking failed.
    exit /b 1
)
echo [DONE] Generated %OUT_DLL%
echo        Exports:
dumpbin /nologo /exports "%OUT_DLL%" | findstr /C:"number of names"
goto :done

REM ============================================================
REM  :compile_module  %1 = absolute module source root
REM  Compiles the module root and every sub-directory of it, each into
REM  the object directory that mirrors its own path. Returns 1 on the
REM  first failing directory (FAILED is checked by the caller, because
REM  errorlevel inside a for /f loop only reports the last iteration).
REM ============================================================
:compile_module
if not exist "%~1" exit /b 0
call :compile_dir "%~1"
if errorlevel 1 (
    set "FAILED=1"
    exit /b 1
)
for /f "delims=" %%d in ('dir /s /b /ad "%~1" 2^>nul') do (
    if "!FAILED!"=="0" (
        call :compile_dir "%%d"
        if errorlevel 1 set "FAILED=1"
    )
)
exit /b 0

REM ============================================================
REM  :compile_dir  %1 = absolute source directory
REM  %BETA_ROOT% is stripped from the path to get the mirrored object
REM  directory, so Core\Debug\Foo.cpp and UnityPlugin\Debug\Foo.cpp can
REM  never resolve to the same .obj.
REM ============================================================
:compile_dir
set "SRCDIR=%~1"
set "SUBDIR=!SRCDIR:%BETA_ROOT%=!"
if "!SUBDIR!"=="!SRCDIR!" (
    echo [ERROR] Cannot map "!SRCDIR!" below "%BETA_ROOT%".
    exit /b 1
)
set "ODIR=%OBJ_DIR%!SUBDIR!"
set "NDIR=0"
for /f "delims=" %%f in ('dir /b "%SRCDIR%\*.cpp" 2^>nul') do set /a NDIR+=1
if "%NDIR%"=="0" exit /b 0
if not exist "!ODIR!" mkdir "!ODIR!"
set /a TOTAL_SRC+=%NDIR%
echo        !SUBDIR! : %NDIR% file
cl %CFLAGS% "%SRCDIR%\*.cpp" /Fo"!ODIR!\\" /Fd"%OBJ_DIR%\vc.pdb"
if errorlevel 1 exit /b 1
exit /b 0

REM ============================================================
REM  clean
REM ============================================================
:target_clean
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    echo [CLEANED] Intermediates: .build\
)
if exist "%BETA_ROOT%\Main\Debug" (
    del /q "%BETA_ROOT%\Main\Debug\SpaceZ_2D_Beta.exe" 2>nul
    del /q "%BETA_ROOT%\Main\Debug\SpaceZ_2D_Beta.pdb" 2>nul
    del /q "%BETA_ROOT%\Main\Debug\SpaceZ_2D_Beta.ilk" 2>nul
    echo [CLEANED] Main\Debug\ executable and debug files
)
if exist "%BETA_ROOT%\Tests\Debug" (
    del /q "%BETA_ROOT%\Tests\Debug\SpaceZ_2D_BetaTests.exe" 2>nul
    del /q "%BETA_ROOT%\Tests\Debug\SpaceZ_2D_BetaTests.pdb" 2>nul
    del /q "%BETA_ROOT%\Tests\Debug\SpaceZ_2D_BetaTests.ilk" 2>nul
    echo [CLEANED] Tests\Debug\ executable and debug files
)
if exist "%OUT_PLUGINS%" (
    del /q "%OUT_PLUGINS%\%DLL_NAME%" 2>nul
    del /q "%OUT_PLUGINS%\%DLL_NAME%.meta" 2>nul
    echo [CLEANED] Assets\Plugins\x86_64\%DLL_NAME% including its .meta
)
echo [DONE] Clean finished.
exit /b 0

REM ============================================================
REM  Errors / usage
REM ============================================================
:bad_layout
echo [ERROR] Expected the project folder to sit next to Assets\ and ProjectSettings\.
echo         Unity project root resolved to: %UNITY_ROOT%
exit /b 1

:usage
echo Usage: build.bat ^<console^|tests^|plugins^|plugins-release^|clean^>
exit /b 1

:done
exit /b 0
