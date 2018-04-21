@echo off

echo =======================================================================
echo Testing project
echo =======================================================================
cd /d %~dp0
cd ..\..
cd build\bin\Release
rapidassist_unittest.exe
