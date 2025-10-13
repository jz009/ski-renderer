#pragma once
#include "includes_fwd.h"

#include "editor.h"
#include "scene.h"
#include "entity.h"

void Editor::onFrame(Scene& scene, const Input& input) {
    if (input.wasMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 mousePos = input.mouseClickInput.mousePos;
        Raycast ray = getRayFromMouse(mousePos.x, mousePos.y, scene.viewMatrix, scene.projectionMatrix);
        auto colliders = scene.getRayCollisions(ray);
        if (colliders.empty()) {
            if (selectedForEdit) {
                selectedForEdit->model.material.uniforms.color = selectedForEdit->color;
                selectedForEdit = nullptr;
            }
            return;
        }

        RayCollision collision = colliders.front();
        if (selectedForEdit) {
            if (selectedForEdit->id == collision.entity->id) {
                state = EditState::DRAG;
                lastFrameMousePos = input.mousePosition;
            }
            else {
                selectedForEdit->model.material.uniforms.color = selectedForEdit->color;
            }
        }
        selectedForEdit = collision.entity;
        collision.entity->model.material.uniforms.color = Constants::EDIT_COLOR;

    }
    if (input.wasMouseReleased(GLFW_MOUSE_BUTTON_LEFT)) {
        state = EditState::NONE;
    }
    if (selectedForEdit) {
        if (state == EditState::DRAG) {
            glm::vec2 offset = (input.mousePosition - lastFrameMousePos) / 10.0f;
            glm::vec3 delta = mouseOffsetToWorldDelta(offset, scene.camera->theta);
            selectedForEdit->transform.position -= delta;
            lastFrameMousePos = input.mousePosition;
        }

        if (input.wasPressed(GLFW_KEY_W)) {
            selectedForEdit->transform.position.z += 1.0f;
        }
    }
}

void Editor::endEditMode(Scene& scene) {
    if (selectedForEdit) {
        selectedForEdit->model.material.uniforms.color = selectedForEdit->color;
    }
    selectedForEdit = nullptr;
    scene.navMesh = NavMesh(scene.entities);
}