#pragma once
#include "includes_fwd.h"

#include "camera.h"
#include "input.h"
#include "scene.h"

void Camera::moveCamera(glm::vec3 _position) {
    position = _position;
}

void CircleBoundCamera::onFrame(Scene& scene) {
    const Input& input = scene.input;
    glm::vec2 mousePos = glm::vec2(input.mousePosition);
    float s = speed;

    bool scrollWheelPressed = input.mouseClickInput.fresh && input.mouseClickInput.button == GLFW_MOUSE_BUTTON_MIDDLE && input.mouseClickInput.action == GLFW_PRESS;
    bool scrollWheelReleased = input.mouseClickInput.fresh && input.mouseClickInput.button == GLFW_MOUSE_BUTTON_MIDDLE && input.mouseClickInput.action == GLFW_RELEASE;

    if (scrollWheelPressed) {
        state = CameraState::DRAG;
        dragStart = mousePos;
    }
    if (scrollWheelReleased) {
        state = CameraState::NONE;
    }

    if (state == CameraState::DRAG) {
        glm::vec2 offset = (mousePos - dragStart) / 10.0f;
        if (offset.x < 0) {
            float deltaX = offset.x * std::cos(thetaPosition + (90 * Constants::D2R));
            float deltaZ = offset.x * std::sin(thetaPosition + (90 * Constants::D2R));
            target.x -= deltaX;
            target.z -= deltaZ;
            position.x -= deltaX;
            position.z -= deltaZ;
        }
        else {
            float deltaX = offset.x * std::cos(thetaPosition - (90 * Constants::D2R));
            float deltaZ = offset.x * std::sin(thetaPosition - (90 * Constants::D2R));
            target.x += deltaX;
            target.z += deltaZ;
            position.x += deltaX;
            position.z += deltaZ;
        }

        float deltaX = offset.y * std::cos(thetaPosition);
        float deltaZ = offset.y * std::sin(thetaPosition);
        target.x -= deltaX;
        target.z -= deltaZ;
        position.x -= deltaX;
        position.z -= deltaZ;

        dragStart = mousePos;
    }

    if (state == CameraState::NONE && mousePos.x < Constants::WIDTH / 10) {
        if (mousePos.x <= 1) {
            s *= 2;
        }
        moveCamera(posOnCircle(target, thetaPosition, -s, radius, position.y));
    }

    else if (state == CameraState::NONE && mousePos.x > Constants::WIDTH - Constants::WIDTH / 10) {
        if (mousePos.x >= Constants::WIDTH - 1) {
            s *= 2;
        }
        moveCamera(posOnCircle(target, thetaPosition, s, radius, position.y));
    }
}

glm::vec3 posOnCircle(glm::vec3 target, float& theta, float delta, float radius, float yPos) {
    theta += delta;
    return glm::vec3(target.x + radius * std::cos(theta), target.y + yPos, target.z + radius * std::sin(theta));
}