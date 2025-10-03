#pragma once
#include "includes_fwd.h"

struct AABB
{
    glm::vec3 baseMin;
    glm::vec3 baseMax;
    glm::vec3 min;
    glm::vec3 max;
    AABB::AABB() = default;
    AABB(glm::vec3 min, glm::vec3 max)
    {
        baseMin = min;
        baseMax = max;
    }
};