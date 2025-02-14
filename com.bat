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
echo Copying Files
xcopy "Ninja/SFX" "Debug/SFX" /y /i /E
xcopy "Ninja/GFX" "Debug/GFX" /y /i /E
xcopy "Ninja/data" "Debug/data" /y /i /E
xcopy "Ninja/Screenshots" "Debug/Screenshots" /y /i /E
xcopy "Ninja/gamecontrollerdb.txt" "Debug/gamecontrollerdb.txt" /y /E
xcopy "Ninja/SFX" "Release/SFX" /y /i /E
xcopy "Ninja/GFX" "Release/GFX" /y /i /E
xcopy "Ninja/data" "Release/data" /y /i /E
xcopy "Ninja/Screenshots" "Release/Screenshots" /y /i /E
xcopy "Ninja/gamecontrollerdb.txt" "Release/gamecontrollerdb.txt" /y /E
echo.
echo Mission Complete
::pause so you can see the compilation errors if there are any
pause