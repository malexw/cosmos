cmake_minimum_required(VERSION 3.16)
project(cosmos)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenAL REQUIRED)

# Auto-discover source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(cosmos ${SOURCES})

target_include_directories(cosmos PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${SDL2_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIR}
)

target_link_libraries(cosmos
    OpenGL::GL
    OpenGL::GLU
    ${SDL2_LIBRARIES}
    ${OPENAL_LIBRARY}
)
