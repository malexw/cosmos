#ifndef COSMOS_ACTION_HPP_
#define COSMOS_ACTION_HPP_

#include <cstddef>
#include <functional>

enum class Action { ToggleDebugMenu, Quit, Grab };
enum class Axis { MoveX, MoveY, LookX, LookY, MouseLookX, MouseLookY };

struct ActionHash {
    std::size_t operator()(Action a) const {
        return std::hash<int>()(static_cast<int>(a));
    }
};

struct AxisHash {
    std::size_t operator()(Axis a) const {
        return std::hash<int>()(static_cast<int>(a));
    }
};

#endif
