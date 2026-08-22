cmake_minimum_required(VERSION 3.20)

project("Sonic_Heroes_Memory_Editor" VERSION 3.0.0 LANGUAGES CXX)

if(UNIX AND NOT APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")

    set(CMAKE_SYSTEM_LIBRARY_PATH /usr/lib32)
    set(FIND_LIBRARY_USE_LIB32_PATHS TRUE)
    set(FIND_LIBRARY_USE_LIB64_PATHS FALSE)

    set(OPENGL_glx_LIBRARY "/usr/lib32/libGLX.so" CACHE FILEPATH "32-bit GLX" FORCE)
    set(OPENGL_opengl_LIBRARY "/usr/lib32/libOpenGL.so" CACHE FILEPATH "32-bit OpenGL" FORCE)
    set(OPENGL_gl_LIBRARY "/usr/lib32/libGL.so" CACHE FILEPATH "32-bit GL" FORCE)
endif()

set(SOURCE_FILES
    src/main.cpp
    src/windowmanager.cpp
    src/menues.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ============================================================
# GLFW
# ============================================================
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.5.1
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(glfw)

# ============================================================
# Dear ImGui
# ============================================================

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.9b-docking
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(imgui)

# ============================================================
# Memory Library
# ============================================================

FetchContent_Declare(
    YenMemLib
    GIT_REPOSITORY https://github.com/Yenilmez99/YenMemLib.git
    GIT_TAG        v1.0.7
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(YenMemLib)

# ============================================================
# ImGui library
# ============================================================

find_package(OpenGL REQUIRED)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp

    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC
    glfw
    OpenGL::GL
)

# ============================================================
# Application
# ============================================================

add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    OpenGL::GL
    YenLib::Memory
)

# ============================================================
# Windows & MinGW Static Runtime
# ============================================================

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_link_options(${PROJECT_NAME} PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
        )
    elseif(MSVC)
        target_link_options(${PROJECT_NAME} PRIVATE "/ENTRY:mainCRTStartup")
        set_property(TARGET ${PROJECT_NAME} PROPERTY 
            MSVC_RUNTIME_LIBRARY "MultiThreaded"
        )
    endif()
endif()
