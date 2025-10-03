#pragma once
#include "includes_fwd.h"

#include "camera.h"
#include "input.h"

void Camera::moveCamera(glm::vec3 _position) {
    position = _position;
}

void CircleBoundCamera::onFrame() {
    glm::vec2 mousePos = glm::vec2(getInput()->mousePosition);
    float s = speed;
    if (mousePos.x < Constants::WIDTH/10) {
        if (mousePos.x <= 1) {
            s *= 2;
        }
        moveCamera(posOnCircle(thetaPosition, -s, radius, position.y));
    }

    if (mousePos.x > Constants::WIDTH - Constants::WIDTH/10) {
        if (mousePos.x >= Constants::WIDTH - 1) {
            s *= 2;
        }
        moveCamera(posOnCircle(thetaPosition, s, radius, position.y));
    }
}

glm::vec3 posOnCircle(float& theta, float delta, float radius, float yPos) {
    theta += delta;
    return glm::vec3(radius * cos(theta), yPos, radius * sin(theta));
}