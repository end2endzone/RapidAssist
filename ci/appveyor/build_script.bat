@echo off

echo =======================================================================
echo Building project
echo =======================================================================
cd /d %~dp0
cd ..\..
mkdir build >NUL 2>NUL
cd build
cmake ..
cmake --build . --config Release
