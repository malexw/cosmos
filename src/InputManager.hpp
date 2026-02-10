#ifndef COSMOS_INPUTMANAGER_H_
#define COSMOS_INPUTMANAGER_H_

#include "ActionMap.hpp"
#include "ActionState.hpp"
#include "InputState.hpp"
#include "util.hpp"

class InputManager {
public:
    InputManager() {}

    void update();
    const InputState& input_state() const { return input_state_; }
    const ActionState& action_state() const { return action_state_; }

private:
    InputState input_state_;
    ActionMap action_map_;
    ActionState action_state_;

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
};

#endif
