# cosmos

My playground for game engine architecture and graphics experiments

## Dependencies

- CMake 3.16+
- Ninja
- SDL3
- OpenGL
- OpenAL

Ubuntu 25.04+ / Debian:

    sudo apt install cmake ninja-build libsdl3-dev libglu1-mesa-dev libopenal-dev

On Ubuntu 24.04 and earlier, SDL3 is not in the package repos and must be built from source:

    sudo apt install cmake ninja-build libglu1-mesa-dev libopenal-dev libglm-dev \
        libwayland-dev libxkbcommon-dev wayland-protocols
    git clone --depth 1 https://github.com/libsdl-org/SDL.git SDL3
    cmake -S SDL3 -B SDL3/build -G Ninja -DCMAKE_BUILD_TYPE=Release
    cmake --build SDL3/build
    sudo cmake --install SDL3/build

macOS:
    brew install cmake ninja sdl3 openal-soft
    export CMAKE_PREFIX_PATH="/opt/homebrew/opt/openal-soft"

## Build

    mkdir build && cd build
    cmake -G Ninja ..
    ninja

## Run

From the project root (so it can find res/):

    ./build/cosmos

## To-do

- [ ] Dear ImGui integration
- [x] Resource packages
- [x] Particle system improvements
- [x] SDL3
- [x] HDR
- [ ] SSAO
- [ ] Multiple light sources and types
- [ ] Emissive particles
- [ ] Multiple shadow casters and shadow types
- [ ] Migrate from OpenAL to Steam Audio + miniaudio
- [ ] Physically-based rendering
- [ ] glTF 2.0 support
- [ ] Animation system
- [ ] Vulkan backend
- [ ] Improved keybinding; prep for OpenXR's actions
- [ ] Support VR with OpenXR
