#pragma once
#include "includes_fwd.h"

#include "entity.h"

void Player::firstPersonOnFrame(Scene& scene, const Input& input) {
    scene.camera->moveCamera(transform.position + glm::vec3(0.0, 5.0f, 0.0));
    if (input.isPressed(GLFW_KEY_W)) {
        glm::vec3 direction = scene.camera->direction;
        //float halfModelWidth = (collider->box.max.x - collider->box.min.x) / 2;
        glm::vec3 tentativePosition = movement.firstPersonMove(transform.position, direction);
        auto layers = scene.colliders.navMesh.getLayers(tentativePosition);
        if (isWalkable(layers) && !isImpassable(layers))
            transform.position = movement.firstPersonMove(transform.position, direction);
    }


}