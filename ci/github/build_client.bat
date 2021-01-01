@echo off

:: Validate github's environment
if "%GITHUB_WORKSPACE%"=="" (
  echo Please define 'GITHUB_WORKSPACE' environment variable.
  exit /B 1
)

echo ============================================================================
echo Generating...
echo ============================================================================
cd /d %GITHUB_WORKSPACE%\client
mkdir build >NUL 2>NUL
cd build
cmake -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_PREFIX_PATH=%GITHUB_WORKSPACE%\third_parties\googletest\install;%GITHUB_WORKSPACE%\install ..
if %errorlevel% neq 0 exit /b %errorlevel%

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d %~dp0
