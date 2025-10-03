#pragma once
#include "includes_fwd.h"

glm::vec3 posOnCircle(float& theta, float delta, float radius, float yPos);

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

    virtual void onFrame() {}
    void moveCamera(glm::vec3 _position);
    // Camera(const Camera&) = delete;
    // Camera operator=(const Camera&) const = delete;
};

struct CircleBoundCamera : Camera {
    float thetaPosition;
    float radius;
    float speed;
    CircleBoundCamera::CircleBoundCamera(float _radius)
    {
        thetaPosition = 0.0f;
        radius = _radius;
        speed = 0.01f;
        position = posOnCircle(thetaPosition, 0.0f, _radius, position.y);
    }
    void onFrame();
};