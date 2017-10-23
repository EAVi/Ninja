::This file is my version of a Windows Makefile, but making VS17 do all the work

@echo off
color 0D
echo Loading Dev Environment..
set "VSCMD_START_DIR=%CD%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
echo Compiling Debug Build..
call devenv Ninja.sln /build Debug
echo Compiling Release Build..
call devenv Ninja.sln /build Release
echo.
echo Mission Complete
::pause so you can see the compilation errors if there are any
pause