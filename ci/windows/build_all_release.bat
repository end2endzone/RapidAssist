@echo off

:: Set RAPIDASSIST_SOURCE_DIR root directory
if "%RAPIDASSIST_SOURCE_DIR%"=="" (
  cd /d %~dp0
  cd ..\..
  set RAPIDASSIST_SOURCE_DIR=%CD%
  echo RAPIDASSIST_SOURCE_DIR set to '%RAPIDASSIST_SOURCE_DIR%'.
  cd /d %~dp0
)

:: Set build configuration parameters
set Configuration=Release
set Platform=x64
set PlatformToolset=""
echo Building RapidAssist for Windows in %Configuration%, %Platform% configuration...
echo.

:: Return back to scripts folder
cd /d %~dp0

:: Call windows scripts one by one.
call %RAPIDASSIST_SOURCE_DIR%\ci\windows\install_googletest.bat
if %errorlevel% neq 0 pause && exit /b %errorlevel%
call %RAPIDASSIST_SOURCE_DIR%\ci\windows\build_library.bat
if %errorlevel% neq 0 pause && exit /b %errorlevel%
call %RAPIDASSIST_SOURCE_DIR%\ci\windows\build_client.bat
if %errorlevel% neq 0 pause && exit /b %errorlevel%
call %RAPIDASSIST_SOURCE_DIR%\ci\windows\test_script.bat
if %errorlevel% neq 0 pause && exit /b %errorlevel%

:: Press a key to continue
pause
