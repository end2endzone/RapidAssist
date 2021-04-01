@echo off

:: Validate mandatory environment variables
if "%RAPIDASSIST_SOURCE_DIR%"=="" (
  echo Please define 'RAPIDASSIST_SOURCE_DIR' environment variable.
  exit /B 1
)
if "%Configuration%"=="" (
  echo Please define 'Configuration' environment variable.
  exit /B 1
)
if "%Platform%"=="" (
  echo Please define 'Platform' environment variable.
  exit /B 1
)

echo =======================================================================
echo Testing RapidAssist library...
echo =======================================================================
cd /d %RAPIDASSIST_SOURCE_DIR%\build\bin\%Configuration%
if "%Configuration%"=="Debug" (
  rapidassist_unittest-d.exe
) else (
  rapidassist_unittest.exe
)

::reset error in case of test case fail
exit /b 0
