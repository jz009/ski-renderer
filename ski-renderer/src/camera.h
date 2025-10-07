#pragma once
#include "includes_fwd.h"

#include "input.h"

glm::vec3 posOnCircle(glm::vec3 target, float& theta, float delta, float radius, float yPos);

enum struct CameraState {
    NONE,
    DRAG,
};

struct Camera
{
    DISALLOW_IMPLICIT_COPIES(Camera)
    
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float ratio;
    float focalLength;
    float near;
    float far;
    glm::vec2 dragStart;
    CameraState state = CameraState::NONE;

    Camera::Camera()
    {
        position = glm::vec3(0.0f, 10.0f, 0.0f);
        target = glm::vec3(0.0f, 0.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        ratio = (float)Constants::WIDTH / (float)Constants::HEIGHT;
        focalLength = 1.0;
        near = 0.01f;
        far = 100.0f;
    }

    virtual void onFrame(Scene&, const Input&) {};
    void moveCamera(glm::vec3 _position);

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
        position = posOnCircle(target, thetaPosition, 0.0f, _radius, position.y);
    }
    void onFrame(Scene& scene, const Input& input) override;
};