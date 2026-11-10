@echo off
REM Thin wrapper - the real implementation lives in build.bat.
REM Builds Core + Main into Main\Debug\SpaceZ_2D_Beta.exe
call "%~dp0build.bat" console
if errorlevel 1 exit /b 1
exit /b 0
