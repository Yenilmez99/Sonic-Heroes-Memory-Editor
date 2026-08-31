# Target OS -> Linux
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i686)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# 32-bit flags
set(CMAKE_C_FLAGS "-m32" CACHE STRING "C Flags" FORCE)
set(CMAKE_CXX_FLAGS "-m32" CACHE STRING "C++ Flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "-m32" CACHE STRING "Shared Linker Flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "-m32" CACHE STRING "Executable Linker Flags" FORCE)

# Force x86
set(CMAKE_SYSTEM_LIBRARY_PATH /usr/lib32)
set(FIND_LIBRARY_USE_LIB32_PATHS TRUE)
set(FIND_LIBRARY_USE_LIB64_PATHS FALSE)

# pkg-config search paths
set(ENV{PKG_CONFIG_LIBDIR} "/usr/lib32/pkgconfig:/usr/share/pkgconfig")
