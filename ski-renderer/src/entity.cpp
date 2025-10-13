#pragma once
#include "includes_fwd.h"

#include "entity.h"

void Player::firstPersonOnFrame(Scene& scene, const Input& input) {
    scene.camera->moveCamera(transform.position + glm::vec3(0.0, 5.0f, 0.0));
    if (input.isPressed(GLFW_KEY_W)) {
        glm::vec3 direction = scene.camera->direction;
        glm::vec3 tentativePosition = movement.firstPersonMove(transform.position, direction);
        glm::mat4x4 tentativeModelMatrix = calculateModelMatrix(transform);
        AABB tentativeBox = { tentativeModelMatrix * glm::vec4(collider.unscaledBox.min, 1.0f), tentativeModelMatrix * glm::vec4(collider.unscaledBox.max, 1.0f) };
        if (!scene.navMesh.overlaps(tentativeBox))
            transform.position = tentativePosition;
    }
    else if (input.isPressed(GLFW_KEY_A)) {

    }


}

void Player::thirdPersonOnFrame(Scene& scene, const Input& input) {
    if (input.wasMousePressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        glm::vec2 mousePos = input.mouseClickInput.mousePos;
        Raycast ray = getRayFromMouse(mousePos.x, mousePos.y, model.material.uniforms.viewMatrix, model.material.uniforms.projectionMatrix);
        auto colliders = scene.getRayCollisions(ray);
        if (!colliders.empty()) {
            RayCollision collision = colliders.front();
            if (isWalkable(collision.entity->collider.layerMask) && areClose(collision.intersection.near.y, collision.entity->collider.box.max.y)) {
                auto path = findPath(shared_from_this(), collision.intersection.near, scene);
                movement.targetPath = path;
            }
        }
    }
}