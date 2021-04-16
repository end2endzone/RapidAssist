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
set CMAKE_INSTALL_PREFIX=%RAPIDASSIST_SOURCE_DIR%\install
set CMAKE_PREFIX_PATH=
set CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%;%RAPIDASSIST_SOURCE_DIR%\third_parties\googletest\install
set CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%;%RAPIDASSIST_SOURCE_DIR%\install

echo ============================================================================
echo Generating RapidAssist client example...
echo ============================================================================
cd /d "%RAPIDASSIST_SOURCE_DIR%\client"
mkdir build >NUL 2>NUL
cd build
cmake -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX="%CMAKE_INSTALL_PREFIX%" -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%" ..
if %errorlevel% neq 0 exit /b %errorlevel%

echo ============================================================================
echo Compiling RapidAssist client example...
echo ============================================================================
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d "%~dp0"
