#ifndef COSMOS_ACTIONSTATE_HPP_
#define COSMOS_ACTIONSTATE_HPP_

#include <unordered_map>
#include <unordered_set>

#include "Action.hpp"

struct ActionState {
    bool just_triggered(Action a) const { return triggered_.count(a) > 0; }
    bool is_active(Action a) const { return active_.count(a) > 0; }
    float axis(Axis a) const {
        auto it = axes_.find(a);
        return (it != axes_.end()) ? it->second : 0.0f;
    }

    std::unordered_set<Action, ActionHash> triggered_;
    std::unordered_set<Action, ActionHash> active_;
    std::unordered_map<Axis, float, AxisHash> axes_;
};

#endif
