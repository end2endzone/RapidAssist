@echo off

:: Allow the script to be launch locally or from AppVeyor
cd /d %~dp0
cd ..\..
set APPVEYOR_BUILD_FOLDER_LOCAL=%cd%

:: Detect if script is launch manually or from AppVeyor.
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  set APPVEYOR_BUILD_FOLDER=%APPVEYOR_BUILD_FOLDER_LOCAL%
  echo Building locally: APPVEYOR_BUILD_FOLDER set to %APPVEYOR_BUILD_FOLDER_LOCAL%
)
set APPVEYOR_BUILD_FOLDER_LOCAL=

echo ============================================================================
echo Cloning googletest into %APPVEYOR_BUILD_FOLDER%\third_parties\googletest
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
git clone "https://github.com/google/googletest.git"
cd googletest
echo.

echo Checking out version 1.8.0...
git checkout release-1.8.0
echo.

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
set GTEST_ROOT=%APPVEYOR_BUILD_FOLDER%\third_parties\googletest\install
cmake -DCMAKE_INSTALL_PREFIX=%GTEST_ROOT% -Dgtest_force_shared_crt=ON ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config Release
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %GTEST_ROOT%
echo ============================================================================
cmake --build . --config Release --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
