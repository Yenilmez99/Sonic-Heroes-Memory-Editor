# Target OS -> Windows
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR i686)

# Compilers (clang-cl) and Linker (lld-link)
set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_LINKER lld-link)

# Force x86
set(CMAKE_C_COMPILER_TARGET "i686-pc-windows-msvc")
set(CMAKE_CXX_COMPILER_TARGET "i686-pc-windows-msvc")

# xwin path
set(XWIN_DIR "$ENV{HOME}/.xwin")

# Include directories
set(XWIN_INCLUDES
    "-imsvc${XWIN_DIR}/crt/include"
    "-imsvc${XWIN_DIR}/sdk/include/ucrt"
    "-imsvc${XWIN_DIR}/sdk/include/um"
    "-imsvc${XWIN_DIR}/sdk/include/shared"
)

# Libraries (x86)
set(XWIN_LIBS
    "/libpath:${XWIN_DIR}/crt/lib/x86"
    "/libpath:${XWIN_DIR}/sdk/lib/ucrt/x86"
    "/libpath:${XWIN_DIR}/sdk/lib/um/x86"
)

# Convert to string
string(REPLACE ";" " " XWIN_INCLUDES_STR "${XWIN_INCLUDES}")
string(REPLACE ";" " " XWIN_LIBS_STR "${XWIN_LIBS}")

# Export flags to Cmake Init
set(CMAKE_C_FLAGS_INIT "${XWIN_INCLUDES_STR} /MT -DNDEBUG")
set(CMAKE_CXX_FLAGS_INIT "${XWIN_INCLUDES_STR} /MT -DNDEBUG")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "")

# Add Library paths and Force Use lld-link by -fuse-ld=lld
set(CMAKE_EXE_LINKER_FLAGS_INIT "${XWIN_LIBS_STR}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${XWIN_LIBS_STR}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${XWIN_LIBS_STR}")
