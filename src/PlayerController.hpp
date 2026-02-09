#ifndef COSMOS_PLAYERCONTROLLER_HPP_
#define COSMOS_PLAYERCONTROLLER_HPP_

#include <SDL3/SDL.h>

#include "ActionState.hpp"
#include "CollidableObject.hpp"
#include "Transform.hpp"
#include "Vector3f.hpp"

class PlayerController {
public:
    PlayerController(SDL_Window* window, Transform::ShPtr transform, CollidableObject::ShPtr collidable);
    void update(const ActionState& actions);

private:
    SDL_Window* window_;
    Transform::ShPtr transform_;
    CollidableObject::ShPtr collidable_;
    bool grabbing_ = false;
};

#endif
