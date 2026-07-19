cmake_minimum_required(VERSION 3.16)
project(cosmos)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    steamaudio
    URL https://github.com/ValveSoftware/steam-audio/releases/download/v4.8.1/steamaudio_4.8.1.zip
    URL_HASH SHA256=4a0aa5ec1176f38f0b0993a37c2259d9e86f27e22d5e24f83ec4c3cb9a1d5449
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(steamaudio)

add_library(phonon SHARED IMPORTED)
set_target_properties(phonon PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${steamaudio_SOURCE_DIR}/include)
if(APPLE)
    set_target_properties(phonon PROPERTIES
        IMPORTED_LOCATION ${steamaudio_SOURCE_DIR}/lib/osx/libphonon.dylib)
elseif(WIN32)
    set_target_properties(phonon PROPERTIES
        IMPORTED_LOCATION ${steamaudio_SOURCE_DIR}/lib/windows-x64/phonon.dll
        IMPORTED_IMPLIB ${steamaudio_SOURCE_DIR}/lib/windows-x64/phonon.lib)
else()
    set_target_properties(phonon PROPERTIES
        IMPORTED_LOCATION ${steamaudio_SOURCE_DIR}/lib/linux-x64/libphonon.so)
endif()

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(SDL3 QUIET CONFIG)
if(NOT SDL3_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
    add_library(SDL3::SDL3 ALIAS PkgConfig::SDL3)
endif()
find_package(glm REQUIRED)

# Auto-discover source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "lib/*.cpp")

add_executable(cosmos ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(cosmos PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/lib
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# SYSTEM so vendored-header warnings are suppressed
target_include_directories(cosmos SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/glad/include
)

target_link_libraries(cosmos
    OpenGL::GL
    SDL3::SDL3
    glm::glm
    phonon
)

# libphonon is resolved from next to the executable
add_custom_command(TARGET cosmos POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:phonon> $<TARGET_FILE_DIR:cosmos>)
if(APPLE)
    set_target_properties(cosmos PROPERTIES BUILD_RPATH "@executable_path")
elseif(UNIX)
    set_target_properties(cosmos PROPERTIES BUILD_RPATH "$ORIGIN")
endif()
