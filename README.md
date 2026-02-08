# cosmos

My playground for game engine architecture and graphics experiments

## Dependencies

- CMake 3.16+
- Ninja
- SDL2
- OpenGL
- OpenAL

Ubuntu/Debian:
    sudo apt install cmake ninja-build libsdl2-dev libglu1-mesa-dev libopenal-dev

macOS:
    brew install cmake ninja sdl2 openal-soft
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
