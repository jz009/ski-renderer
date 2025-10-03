#pragma once
#include "includes_fwd.h"

#include "camera.h"

struct Scene {
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<BoxCollider> colliders;
    std::shared_ptr<Camera> camera;

    Scene::Scene() {}
    Scene(const Scene &) = delete;
    Scene operator=(const Scene &) const = delete;
};