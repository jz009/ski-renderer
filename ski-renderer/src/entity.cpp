#pragma once
#include "includes_fwd.h"

#include "entity.h"
#include "scene.h"
#include "movement.h"
#include "rendering_utils.h"
#include "camera.h"
#include "AABB.h"
#include "input.h"
#include "collision.h"

void Entity::playerOnFrame(Scene& scene, const Input& input)
{
    if (scene.camera->type == CameraType::FirstPersonCamera) {
        playerFirstPersonOnFrame(scene, input);
    }
    else if (scene.camera->type == CameraType::CircleBoundCamera) {
        playerThirdPersonOnFrame(scene, input);
    }
    transform.position = movement.move(transform.position);
    applyMatrices(scene);
}

void Entity::playerFirstPersonOnFrame(Scene& scene, const Input& input) {
    scene.camera->moveCamera(transform.position + glm::vec3(0.0, 5.0f, 0.0));
    if (input.isKeyDown(GLFW_KEY_W)) {
        glm::vec3 direction = scene.camera->direction;
        glm::vec3 tentativePosition = movement.firstPersonMove(transform.position, direction);
        glm::mat4x4 tentativeModelMatrix = calculateModelMatrix(transform);
        AABB tentativeBox = { tentativeModelMatrix * glm::vec4(collider.unscaledBox.min, 1.0f), tentativeModelMatrix * glm::vec4(collider.unscaledBox.max, 1.0f) };
        if (!scene.navMesh.overlaps(tentativeBox))
            transform.position = tentativePosition;
    }
    else if (input.isKeyDown(GLFW_KEY_A)) {

    }
}

void Entity::playerThirdPersonOnFrame(Scene& scene, const Input& input) {
    if (input.wasMousePressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        glm::vec2 mousePos = input.mouseClickInput.mousePos;
        Raycast ray = scene.getRayFromMouse(mousePos);
        auto colliders = scene.getRayCollisions(ray);
        if (!colliders.empty()) {
            RayCollision collision = colliders.front();
            if (isWalkable(collision.entity->collider.layerMask) && areClose(collision.intersection.near.y, collision.entity->collider.box.max.y)) {
                auto path = findPath(*this, collision.intersection.near, scene);
                movement.targetPath = path;
            }
        }
    }
}

void Entity::terrainOnFrame(Scene& scene, const Input&) {
    applyMatrices(scene);
}

void Entity::applyMatrices(const Scene& scene) {
    model.material.uniforms.modelMatrix = calculateModelMatrix(transform);
    model.material.uniforms.viewMatrix = scene.viewMatrix;
    collider.transformBox(model.material.uniforms.modelMatrix);
}