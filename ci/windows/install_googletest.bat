@echo off

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
echo Cloning googletest into %RAPIDASSIST_SOURCE_DIR%\third_parties\googletest
echo ============================================================================
mkdir %RAPIDASSIST_SOURCE_DIR%\third_parties >NUL 2>NUL
cd %RAPIDASSIST_SOURCE_DIR%\third_parties
git clone "https://github.com/google/googletest.git"
cd googletest
echo.

echo Checking out version 1.8.0...
git -c advice.detachedHead=false checkout release-1.8.0
echo.

echo ============================================================================
echo Compiling googletest...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
cmake -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%RAPIDASSIST_SOURCE_DIR%\third_parties\googletest\install -Dgtest_force_shared_crt=ON -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing googletest into %RAPIDASSIST_SOURCE_DIR%\third_parties\googletest\install
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
