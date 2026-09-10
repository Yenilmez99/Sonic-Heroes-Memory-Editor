#!/bin/bash

echo "--- Starting Cmake build - Clang-cl (force static and x86) ---"
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=clang-cl-x86.cmake -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_BUILD_TYPE=Release

# If the CMake configuration is successful (if the exit code is 0), proceed to compile.
if [ $? -eq 0 ]; then
    echo ""
    echo "--- Compiling ---"
    cmake --build build
else
    echo ""
    echo "!!! Something went wrong about CMake !!!"
fi

# Prevent the window from closing.
echo ""
read -p "Press any key to continue . . ."