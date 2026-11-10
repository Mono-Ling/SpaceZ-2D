@echo off
REM Thin wrapper - the real implementation lives in build.bat.
REM Builds Core + Tests into Tests\Debug\SpaceZ_2D_BetaTests.exe
REM This target is standalone: it never touches Assets\Plugins.
call "%~dp0build.bat" tests
if errorlevel 1 exit /b 1
exit /b 0
