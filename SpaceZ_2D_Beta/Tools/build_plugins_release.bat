@echo off
REM Thin wrapper - the real implementation lives in build.bat.
REM Release build of the same DLL (/O2 /GL + link /LTCG, /MD).
REM It overwrites the Debug DLL at the SAME path, which keeps the Unity
REM .meta / GUID stable; only one flavour can exist there at a time.
call "%~dp0build.bat" plugins-release
if errorlevel 1 exit /b 1
exit /b 0
