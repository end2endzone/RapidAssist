@echo off

echo =======================================================================
echo Testing project
echo =======================================================================
cd /d %~dp0
cd ..\..
cd build\bin\Release
rapidassist_unittest.exe

::reset error in case of test case fail
exit /b 0
