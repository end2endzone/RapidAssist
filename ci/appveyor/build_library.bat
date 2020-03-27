@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

echo ============================================================================
echo Generating...
echo ============================================================================
cd /d %APPVEYOR_BUILD_FOLDER%
mkdir build >NUL 2>NUL
cd build
cmake -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%\install -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%\third_parties\googletest\install -DRAPIDASSIST_BUILD_TEST=ON -DBUILD_SHARED_LIBS=OFF ..
if %errorlevel% neq 0 exit /b %errorlevel%

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %APPVEYOR_BUILD_FOLDER%\install
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d %~dp0
