#pragma once
#include "includes_fwd.h"

#include "editor.h"
#include "scene.h"
#include "entity.h"
#include "input.h"

void Editor::onFrame(Scene& scene, const Input& input) {

    bool isDragging = mouseDown;
    if (input.wasMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
        if (!mouseDown)
        {
            mouseDown = true;
            if (!selectionTool.handleMouseDown(scene, input, *this)) {
                currentTool->handleMouseDown(scene, input, *this);
            }
        }
    }
    else if (input.wasKeyPressed(GLFW_KEY_Y)) {
        currentTool = std::make_unique<YDragTool>(); 
    }
    else if (input.wasKeyPressed(GLFW_KEY_X)) {
        currentTool = std::make_unique<XZDragTool>();
    }
    else if (input.wasKeyPressed(GLFW_KEY_D)) {
        
    }
    else if (input.wasMouseReleased(GLFW_MOUSE_BUTTON_LEFT)) {
        mouseDown = false;
        if (!selectionTool.handleMouseUp(scene, input, *this)) {
            currentTool->handleMouseUp(scene, input, *this);
        }
    }

    if (isDragging && selectedForEdit) {
        currentTool->handleDrag(scene, input, *this);
    }

    lastFrameMousePos = input.mousePosition;
}

bool XZDragTool::handleDrag(Scene& scene, const Input& input, Editor& editor) {
    if (input.mousePosition == editor.lastFrameMousePos) {
        return false;
    }

    glm::vec3 worldMousePosition = scene.getWorldPosFromMouseAtY(input.mousePosition, editor.selectedForEdit->transform.position.y) + offset;
    if (input.isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        worldMousePosition.x = std::round(worldMousePosition.x);
        worldMousePosition.z = std::round(worldMousePosition.z);
    }
    editor.selectedForEdit->transform.position = worldMousePosition;
    return true;
}

bool XZDragTool::handleMouseDown(Scene& scene, const Input& input, Editor& editor) {
    offset = editor.selectedForEdit->transform.position - scene.getWorldPosFromMouseAtY(input.mousePosition, editor.selectedForEdit->transform.position.y);
    offset.y = 0.0f;
    return true;
}

bool YDragTool::handleMouseDown(Scene& scene, const Input& input, Editor& editor) {
    offset = editor.selectedForEdit->transform.position - scene.getWorldPosFromMouseAtXZ(input.mousePosition, editor.selectedForEdit->transform.position.x, editor.selectedForEdit->transform.position.z);
    offset.x = 0.0f;
    offset.z = 0.0f;
    return true;
}

bool YDragTool::handleDrag(Scene& scene, const Input& input, Editor& editor) {
    if (input.mousePosition == editor.lastFrameMousePos) {
        return false;
    }

    glm::vec3 worldMousePosition = scene.getWorldPosFromMouseAtXZ(input.mousePosition, editor.selectedForEdit->transform.position.x, editor.selectedForEdit->transform.position.z) + offset;

    if (input.isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        worldMousePosition.y = std::round(worldMousePosition.y);
    }
    editor.selectedForEdit->transform.position = worldMousePosition;
    return true;
}

bool SelectionTool::handleMouseDown(Scene& scene, const Input& input, Editor& editor) {
    glm::vec2 mousePos = input.mouseClickInput.mousePos;
    Raycast ray = scene.getRayFromMouse(mousePos);
    auto colliders = scene.getRayCollisions(ray);
    if (colliders.empty()) {
        if (editor.selectedForEdit) {
            editor.selectedForEdit->model.material.uniforms.color = editor.selectedForEdit->color;
            editor.selectedForEdit = nullptr;
        }
        return true;
    }

    if (editor.selectedForEdit) {
        editor.selectedForEdit->model.material.uniforms.color = editor.selectedForEdit->color;
    }

    RayCollision collision = colliders.front();
    editor.selectedForEdit = collision.entity;
    editor.selectedForEdit->model.material.uniforms.color = Constants::EDIT_COLOR;
    return false;
}

void Editor::endEditMode(Scene& scene) {
    if (selectedForEdit) {
        selectedForEdit->model.material.uniforms.color = selectedForEdit->color;
    }
    selectedForEdit = nullptr;
    scene.navMesh = NavMesh(scene.entities);
}