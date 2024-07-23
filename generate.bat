@echo off

pushd 
premake\premake5.exe --file=premake5.lua vs2022
popd
pause