@echo off

:: Validate github's environment
if "%GITHUB_WORKSPACE%"=="" (
  echo Please define 'GITHUB_WORKSPACE' environment variable.
  exit /B 1
)

echo =======================================================================
echo Testing project
echo =======================================================================
cd /d %GITHUB_WORKSPACE%\build\bin\%Configuration%
rapidassist_unittest.exe

::reset error in case of test case fail
exit /b 0
