@echo off

:: Validate mandatory environment variables
if "%Configuration%"=="" (
  echo Please define 'Configuration' environment variable.
  exit /B 1
)
if "%Platform%"=="" (
  echo Please define 'Platform' environment variable.
  exit /B 1
)

:: Set RAPIDASSIST_SOURCE_DIR root directory
setlocal enabledelayedexpansion
if "%RAPIDASSIST_SOURCE_DIR%"=="" (
  :: Delayed expansion is required within parentheses https://superuser.com/questions/78496/variables-in-batch-file-not-being-set-when-inside-if
  cd /d "%~dp0"
  cd ..\..
  set RAPIDASSIST_SOURCE_DIR=!CD!
  cd ..\..
  echo RAPIDASSIST_SOURCE_DIR set to '!RAPIDASSIST_SOURCE_DIR!'.
)
endlocal & set RAPIDASSIST_SOURCE_DIR=%RAPIDASSIST_SOURCE_DIR%

echo =======================================================================
echo Testing RapidAssist library...
echo =======================================================================
cd /d "%RAPIDASSIST_SOURCE_DIR%\build\bin\%Configuration%"
if "%Configuration%"=="Debug" (
  rapidassist_unittest-d.exe
) else (
  rapidassist_unittest.exe
)
