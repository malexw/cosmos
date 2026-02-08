cmake_minimum_required(VERSION 3.16)
project(cosmos)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(SDL3 QUIET CONFIG)
if(NOT SDL3_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
    add_library(SDL3::SDL3 ALIAS PkgConfig::SDL3)
endif()
find_package(OpenAL REQUIRED)
find_package(glm REQUIRED)

# Auto-discover source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "lib/*.cpp")

add_executable(cosmos ${SOURCES})

# Enable OpenGL extension prototypes on Linux
target_compile_definitions(cosmos PRIVATE GL_GLEXT_PROTOTYPES)

# Silence macOS OpenGL deprecation warnings
target_compile_definitions(cosmos PRIVATE GL_SILENCE_DEPRECATION)

target_include_directories(cosmos PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/lib
    ${OPENAL_INCLUDE_DIR}
)

target_link_libraries(cosmos
    OpenGL::GL
    SDL3::SDL3
    ${OPENAL_LIBRARY}
    glm::glm
)
