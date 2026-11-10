@echo off
REM Thin wrapper - the real implementation lives in build.bat.
REM Builds Core + UnityPlugin into ONE DLL (Debug) and copies it to
REM   <UnityRoot>\Assets\Plugins\x86_64\SpaceZ_2D_Beta.dll
call "%~dp0build.bat" plugins
if errorlevel 1 exit /b 1
exit /b 0
