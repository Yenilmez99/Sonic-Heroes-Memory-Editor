#!/bin/bash

echo "--- Starting Cmake build - GCC (x86) ---"
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=gcc-linux32.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_SHARED_LINKER_FLAGS="-static-libgcc -static-libstdc++" -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++"

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