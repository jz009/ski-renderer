#pragma once
#include "includes_fwd.h"

#include "camera.h"
#include "collision.h"
#include "editor.h"
#include "rendering_utils.h"

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

    void useCamera(CameraType cameraType, const Input& input);
    bool isEditing();
    bool isGame();
    void createEntity(EntityType type, const AABB& boundingBox, std::bitset<32> layers, Model& model, const Transform& transform, glm::vec4 color);
    std::shared_ptr<Entity> copyEntity(Entity entity);
    void deleteEntity(std::shared_ptr<Entity> entity);
    std::vector<RayCollision> getRayCollisions(const Raycast& ray);
    Raycast getRayFromMouse(glm::vec2 mousePos);
    glm::vec3 getWorldPosFromMouseAtY(glm::vec2 mousePos, float y);
    glm::vec3 getWorldPosFromMouseAtXZ(glm::vec2 mousePos, float x, float z);
    void onFrame(const Input& input);
    Scene();
};