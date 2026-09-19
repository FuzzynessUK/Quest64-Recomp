@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set PATH=%PATH%;C:\Users\Fuzzyness\AppData\Local\Microsoft\WinGet\Packages\ezwinports.make_Microsoft.Winget.Source_8wekyb3d8bbwe\bin
cd /d "D:\Games\Quest64Final"
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build-cmake --target Quest64Recompiled --config Release
