cmake_minimum_required(VERSION 3.20)

project(Sonic_Heroes_Memory_Editor_Workspace VERSION 3.0.0 LANGUAGES CXX)

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

add_executable(MemoryEditorExecutable
    ${SOURCE_FILES}
)
set_target_properties(MemoryEditorExecutable PROPERTIES OUTPUT_NAME "Sonic Heroes Memory Editor")
target_include_directories(MemoryEditorExecutable PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MemoryEditorExecutable PRIVATE
    imgui
    OpenGL::GL
    YenLib::Memory
)

# ============================================================
# Windows Static Runtime & Memory Editor Opener DLL
# ============================================================

if(WIN32)
    add_library(BridgeDLL SHARED src/dllmain.cpp)
    set_target_properties(BridgeDLL PROPERTIES OUTPUT_NAME "memeditoropener")

    set_target_properties(MemoryEditorExecutable PROPERTIES
        WIN32_EXECUTABLE TRUE
    )

    if(MSVC)
        target_link_options(MemoryEditorExecutable PRIVATE "/ENTRY:mainCRTStartup")
    endif()
	
endif()
