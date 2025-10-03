#pragma once
#include "includes_fwd.h"

struct Camera
{
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float ratio;
    float focalLength;
    float near;
    float far;

    Camera::Camera()
    {
        position = glm::vec3(4.0f, 10.0f, 10.0f);
        target = glm::vec3(0.0f, 0.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        ratio = (float)Constants::WIDTH / (float)Constants::HEIGHT;
        focalLength = 1.0;
        near = 0.01f;
        far = 100.0f;
    }

    Camera(const Camera &) = delete;
    Camera operator=(const Camera &) const = delete;
};