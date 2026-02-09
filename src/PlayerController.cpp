#include "PlayerController.hpp"

#include <imgui.h>

PlayerController::PlayerController(SDL_Window* window, Transform::ShPtr transform, CollidableObject::ShPtr collidable)
    : window_(window), transform_(transform), collidable_(collidable) {
}

void PlayerController::update(const ActionState& actions) {
    const ImGuiIO& io = ImGui::GetIO();

    // Safety: force-release mouse grab if ImGui wants mouse
    if (grabbing_ && io.WantCaptureMouse) {
        grabbing_ = false;
        SDL_SetWindowRelativeMouseMode(window_, false);
    }

    // Grab toggle
    if (!io.WantCaptureMouse) {
        if (actions.just_triggered(Action::Grab)) {
            grabbing_ = true;
            SDL_SetWindowRelativeMouseMode(window_, true);
        }
        if (grabbing_ && !actions.is_active(Action::Grab)) {
            grabbing_ = false;
            SDL_SetWindowRelativeMouseMode(window_, false);
        }
    }

    // Mouse look (only when grabbing)
    if (grabbing_) {
        float mdx = actions.axis(Axis::MouseLookX);
        float mdy = actions.axis(Axis::MouseLookY);
        if (mdx != 0.0f || mdy != 0.0f) {
            transform_->rotate(Vector3f::NEGATIVE_Y, mdx);
            transform_->rotate_relative(Vector3f::NEGATIVE_X, mdy);
        }
    }

    // Gamepad look (right stick, always active)
    float look_x = actions.axis(Axis::LookX);
    float look_y = actions.axis(Axis::LookY);
    constexpr float LOOK_SENSITIVITY = 3.0f;
    if (look_x != 0.0f || look_y != 0.0f) {
        transform_->rotate(Vector3f::NEGATIVE_Y, look_x * LOOK_SENSITIVITY);
        transform_->rotate_relative(Vector3f::NEGATIVE_X, look_y * LOOK_SENSITIVITY);
    }

    // Movement
    float move_x = actions.axis(Axis::MoveX);
    float move_y = actions.axis(Axis::MoveY);

    constexpr float SPEED = 10.0f;
    Vector3f velocity(move_x * SPEED, 0.0f, move_y * SPEED);
    collidable_->set_velocity(velocity);
}
