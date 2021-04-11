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

:: Find project root directory
cd /d %~dp0
cd ..\..
set RAPIDASSIST_SOURCE_DIR=%CD%
cd /d %~dp0
if "%RAPIDASSIST_SOURCE_DIR%"=="" (
  echo Please define 'RAPIDASSIST_SOURCE_DIR' environment variable.
  exit /B 1
)

echo ============================================================================
echo Generating RapidAssist client example...
echo ============================================================================
cd /d %RAPIDASSIST_SOURCE_DIR%\client
mkdir build >NUL 2>NUL
cd build
cmake -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_PREFIX_PATH=%RAPIDASSIST_SOURCE_DIR%\third_parties\googletest\install;%RAPIDASSIST_SOURCE_DIR%\install ..
if %errorlevel% neq 0 exit /b %errorlevel%

echo ============================================================================
echo Compiling RapidAssist client example...
echo ============================================================================
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d %~dp0
