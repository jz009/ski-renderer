#pragma once
#include "includes_fwd.h"

#include "camera.h"
#include "collision.h"
#include "input.h"


struct Scene {
    DISALLOW_IMPLICIT_COPIES(Scene)
    std::vector<std::shared_ptr<Entity>> entities;
    ColliderTree colliders;
    std::unique_ptr<Camera> camera;
    Input input;

    Scene::Scene(std::unique_ptr<Camera> _camera, GLFWwindow* window) : camera(std::move(_camera)), input(Input(window)) {}
};