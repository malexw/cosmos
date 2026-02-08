# cosmos

My playground for game engine architecture and graphics experiments

-ry 270
-t 0 0 -0.5
-t -1.5 0 1.5

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
- [ ] Resource packages
- [x] Particle system improvements
- [ ] SDL3
- [ ] Migrate from OpenAL to Steam Audio + miniaudio
- [ ] Physically-based rendering
- [ ] glTF 2.0 support
- [ ] Animation system
- [ ] SSAO
- [ ] HDR
- [ ] Vulkan backend
- [ ] Improved keybinding; prep for OpenXR's actions
- [ ] Support VR with OpenXR

