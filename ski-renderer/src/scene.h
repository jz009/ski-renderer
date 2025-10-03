#pragma once
#include "includes_fwd.h"

#include "camera.h"

struct Scene {
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<BoxCollider> colliders;
    Camera camera = Camera();
};