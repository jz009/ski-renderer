#pragma once
#include "includes_fwd.h"

#include "camera.h"
#include "collision.h"
#include "input.h"
#include "editor.h"

enum struct SceneState {
    GAME,
    EDIT,
};

struct Scene {
    DISALLOW_IMPLICIT_COPIES(Scene)
    std::vector<std::shared_ptr<Entity>> entities;
    NavMesh navMesh;
    std::array<std::shared_ptr<Camera>, 8> cameras;
    std::shared_ptr<Camera> camera;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 projectionMatrix;
    SceneState state = SceneState::GAME;
    Editor editor;
    int nextId = 0;

    void addEntity(std::shared_ptr<Entity> entity);
    void useCamera(CameraType cameraType, const Input& input);
    bool isEditing();
    bool isGame();
    std::vector<RayCollision> getRayCollisions(const Raycast& ray);
    Raycast getRayFromMouse(glm::vec2 mousePos);
    glm::vec3 getWorldPosFromMouseAtY(glm::vec2 mousePos, float y);
    glm::vec3 getWorldPosFromMouseAtXZ(glm::vec2 mousePos, float x, float z);
    void onFrame(const Input& input);
    Scene::Scene() {
        cameras[(int)CameraType::CircleBoundCamera] = std::make_shared<CircleBoundCamera>(20.0f);
        cameras[(int)CameraType::FirstPersonCamera] = std::make_shared<FirstPersonCamera>();
        camera = cameras[(int)CameraType::CircleBoundCamera];
    }
};