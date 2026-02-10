#ifndef COSMOS_SKYBOX_HPP_
#define COSMOS_SKYBOX_HPP_

#include <memory>

#include "Renderable.hpp"
#include "util.hpp"

class SkyBox {
public:
    using ShPtr = std::shared_ptr<SkyBox>;

    SkyBox()
        : renderable_(std::make_shared<Renderable>(0)) {}

    Renderable& renderable() { return *renderable_; }
    const Renderable& renderable() const { return *renderable_; }

private:
    Renderable::ShPtr renderable_;

    SkyBox(const SkyBox&) = delete;
    SkyBox& operator=(const SkyBox&) = delete;
};

#endif
