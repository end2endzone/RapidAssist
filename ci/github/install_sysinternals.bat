@echo off
cd /d %~dp0
powershell -nologo -executionpolicy bypass -File "%~n0.ps1"

pause
