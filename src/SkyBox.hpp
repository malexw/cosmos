#ifndef COSMOS_SKYBOX_HPP_
#define COSMOS_SKYBOX_HPP_

#include <memory>

#include "Renderable.hpp"
#include "util.hpp"

class SkyBox {
public:
    typedef std::shared_ptr<SkyBox> ShPtr;

    SkyBox()
        : renderable_(new Renderable(0)) {}

    Renderable& renderable() { return *renderable_; }
    const Renderable& renderable() const { return *renderable_; }

private:
    Renderable::ShPtr renderable_;

    DISALLOW_COPY_AND_ASSIGN(SkyBox);
};

#endif
