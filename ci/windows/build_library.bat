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
echo Generating RapidAssist library...
echo ============================================================================
cd /d %RAPIDASSIST_SOURCE_DIR%
mkdir build >NUL 2>NUL
cd build
cmake -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%RAPIDASSIST_SOURCE_DIR%\install -DCMAKE_PREFIX_PATH=%RAPIDASSIST_SOURCE_DIR%\third_parties\googletest\install -DRAPIDASSIST_BUILD_TEST=ON -DBUILD_SHARED_LIBS=OFF ..
if %errorlevel% neq 0 exit /b %errorlevel%

echo ============================================================================
echo Compiling RapidAssist library...
echo ============================================================================
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing RapidAssist library into %RAPIDASSIST_SOURCE_DIR%\install
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d %~dp0
