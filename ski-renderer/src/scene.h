#pragma once
#include "includes_fwd.h"

#include "camera.h"
#include "collision.h"
#include "input.h"

struct Scene {
    DISALLOW_IMPLICIT_COPIES(Scene)
    std::vector<std::shared_ptr<Entity>> entities;
    ColliderTree colliders;
    std::array<std::shared_ptr<Camera>, 8> cameras;
    std::shared_ptr<Camera> camera;

    void addEntity(std::shared_ptr<Entity> entity);
    void useCamera(CameraType cameraType, const Input& input);
    Scene::Scene() {
        cameras[(int)CameraType::CircleBoundCamera] = std::make_shared<CircleBoundCamera>(20.0f);
        cameras[(int)CameraType::FirstPersonCamera] = std::make_shared<FirstPersonCamera>();
        camera = cameras[(int)CameraType::CircleBoundCamera];
    }
};