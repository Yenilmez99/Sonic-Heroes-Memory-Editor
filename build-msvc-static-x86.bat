@echo off
title Mod Build Script (X86 Static)

:: 1. find visual studio path by vswhere
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do set VS_PATH=%%i

:: 2. Developer Command Prompt for VS
call "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x86

:: 3. Cmake build (force static and x86)
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=msvc-x86.cmake -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_BUILD_TYPE=Release

:: 4. Start compiling
if %ERRORLEVEL% EQU 0 (
    echo.
    echo --- Compiling... ---
    cmake --build build
)

:: 5. Prevent the window from closing.
echo.
pause