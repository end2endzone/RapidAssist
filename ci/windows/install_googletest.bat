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

:: Prepare CMAKE parameters
set CMAKE_INSTALL_PREFIX=%RAPIDASSIST_SOURCE_DIR%\third_parties\googletest\install
set CMAKE_PREFIX_PATH=
set CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%;

echo ============================================================================
echo Cloning googletest into %RAPIDASSIST_SOURCE_DIR%\third_parties\googletest
echo ============================================================================
mkdir "%RAPIDASSIST_SOURCE_DIR%\third_parties" >NUL 2>NUL
cd "%RAPIDASSIST_SOURCE_DIR%\third_parties"
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
cmake -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -Dgtest_force_shared_crt=ON -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON -DCMAKE_INSTALL_PREFIX="%CMAKE_INSTALL_PREFIX%" -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%" ..
if %errorlevel% neq 0 exit /b %errorlevel%

echo.
echo.
echo ==================================================================================
echo Patching googletest to silence warning C4996 about deprecated 'std::tr1' namespace
echo ==================================================================================
echo add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)>>..\googletest\CMakeLists.txt
echo add_definitions(-D_CRT_SECURE_NO_WARNINGS)>>..\googletest\CMakeLists.txt
cmake .. 1>NUL 2>NUL
echo Done patching.
echo.
echo.

:: Continue with compilation
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing googletest into %RAPIDASSIST_SOURCE_DIR%\third_parties\googletest\install
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d "%~dp0"
