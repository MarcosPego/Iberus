@echo off
REM Build and run all tests. Outputs to Tests-Build.
REM Use this when editing to verify the build is not broken.

cd /d "%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "scripts\run-all-tests.ps1" %*
exit /b %ERRORLEVEL%
