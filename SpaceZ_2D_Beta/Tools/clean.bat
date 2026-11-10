@echo off
REM Thin wrapper - the real implementation lives in build.bat.
REM Removes this project's intermediates and outputs only:
REM   .build\, Main\Debug\ and Tests\Debug\ executables,
REM   Assets\Plugins\x86_64\SpaceZ_2D_Beta.dll and its .meta
REM Sibling C++ projects own their own clean scripts and are not touched.
call "%~dp0build.bat" clean
if errorlevel 1 exit /b 1
exit /b 0
