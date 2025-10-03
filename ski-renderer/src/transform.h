#pragma once
#include "includes_fwd.h"

struct Transform
{
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 position;
    glm::vec3 offset;

    Transform::Transform() = default;
    Transform(glm::vec3 _scale, glm::vec3 _rotation, glm::vec3 _position, glm::vec3 _offset)
    {
        scale = _scale;
        rotation = _rotation;
        position = _position;
        offset = _offset;
    }
};