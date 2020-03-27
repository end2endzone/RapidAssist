@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

echo ============================================================================
echo Generating...
echo ============================================================================
cd /d %APPVEYOR_BUILD_FOLDER%\client
mkdir build >NUL 2>NUL
cd build
cmake -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%\third_parties\googletest\install;%APPVEYOR_BUILD_FOLDER%\install ..
if %errorlevel% neq 0 exit /b %errorlevel%

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d %~dp0
