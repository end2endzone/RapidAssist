@echo off

echo =======================================================================
echo Testing project
echo =======================================================================
cd /d %~dp0
cd ..\..
cd build\bin\Release
rapidassist_unittest.exe

::reset_error in case of test case fail
exit /b 0
