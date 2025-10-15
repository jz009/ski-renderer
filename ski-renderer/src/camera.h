#pragma once
#include "includes_fwd.h"

glm::vec3 posOnCircle(glm::vec3 target, float& theta, float delta, float radius, float yPos);

enum struct CameraState {
    NONE,
    DRAG,
    FIRST_PERSON,
};

enum struct CameraType {
    CircleBoundCamera,
    FirstPersonCamera,
};

struct Camera
{
    DISALLOW_IMPLICIT_COPIES(Camera)

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    glm::vec3 direction;
    float ratio;
    float focalLength;
    float near;
    float far;
    float theta;
    glm::vec2 lastFrameMousePos;
    CameraState state = CameraState::NONE;
    CameraType type = CameraType::CircleBoundCamera;

    Camera::Camera()
    {
        position = glm::vec3(0.0f, 20.0f, 0.0f);
        target = glm::vec3(0.0f, 0.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        direction = glm::vec3(1.0f, 0.0f, 0.0f);
        ratio = (float)Constants::WIDTH / (float)Constants::HEIGHT;
        focalLength = 1.0;
        near = 0.01f;
        far = 100.0f;
    }

    virtual void onFrame(Scene&, const Input&) {};
    void moveCamera(glm::vec3 _position);
    void Camera::aimCamera(glm::vec3 target);

};

struct CircleBoundCamera : Camera {
    float radius;
    float speed;
    CircleBoundCamera::CircleBoundCamera(float _radius)
    {
        theta = 0.0f;
        radius = _radius;
        speed = 0.01f;
        position = posOnCircle(0.0f, position.y);
    }
    void onFrame(Scene& scene, const Input& input) override;
    glm::vec3 posOnCircle(float delta, float yPos);
};

struct FirstPersonCamera : Camera {
    float pitch;
    float yaw;
    float sensitivity;
    void onFrame(Scene& scene, const Input& input) override;

    FirstPersonCamera::FirstPersonCamera() {
        focalLength = 1.0;
        near = 0.01f;
        far = 100.0f;
        sensitivity = 0.25f;
    }
};

glm::vec3 mouseOffsetToWorldDelta(glm::vec2 offset, float theta);