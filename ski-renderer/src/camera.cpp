#pragma once
#include "includes_fwd.h"

#include "camera.h"
#include "input.h"
#include "scene.h"

void Camera::moveCamera(glm::vec3 _position) {
    position = _position;
}

void Camera::aimCamera(glm::vec3 _target) {
    target = _target;
}

void CircleBoundCamera::onFrame(Scene& scene, const Input& input) {
    if (input.wasPressed(GLFW_KEY_LEFT_SHIFT)) {
        scene.useCamera(CameraType::FirstPersonCamera, input);
        return;
    }

    glm::vec2 mousePos = glm::vec2(input.mousePosition);
    float s = speed;

    bool scrollWheelPressed = input.mouseClickInput.fresh && input.mouseClickInput.button == GLFW_MOUSE_BUTTON_MIDDLE && input.mouseClickInput.action == GLFW_PRESS;
    bool scrollWheelReleased = input.mouseClickInput.fresh && input.mouseClickInput.button == GLFW_MOUSE_BUTTON_MIDDLE && input.mouseClickInput.action == GLFW_RELEASE;

    if (scrollWheelPressed) {
        state = CameraState::DRAG;
        lastFrameMousePos = mousePos;
    }
    if (scrollWheelReleased) {
        state = CameraState::NONE;
    }

    if (state == CameraState::DRAG) {
        glm::vec2 offset = (mousePos - lastFrameMousePos) / 10.0f;
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

        lastFrameMousePos = mousePos;
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

void FirstPersonCamera::onFrame(Scene&, const Input& input) {
    glm::vec2 offset =lastFrameMousePos - input.mousePosition;
    offset.x *= sensitivity;
    offset.y *= sensitivity;
    yaw += offset.x;
    pitch += offset.y;
    pitch = std::clamp(pitch, -89.0f, 89.0f);
    yaw = std::clamp(yaw, -360.0f, 360.0f);
    if (yaw == 360.0f || yaw == -360.f) {
        yaw = 0.0f;
    }

    direction = glm::normalize(glm::vec3(std::cos(yaw * Constants::D2R) * std::cos(pitch * Constants::D2R), std::sin(pitch * Constants::D2R), std::sin(yaw * Constants::D2R) * std::cos(pitch * Constants::D2R)));
    aimCamera(position + direction);
    lastFrameMousePos = input.mousePosition;
}

glm::vec3 posOnCircle(glm::vec3 target, float& theta, float delta, float radius, float yPos) {
    theta += delta;
    return glm::vec3(target.x + radius * std::cos(theta), target.y + yPos, target.z + radius * std::sin(theta));
}