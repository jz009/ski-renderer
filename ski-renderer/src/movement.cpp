#pragma once
#include "includes_fwd.h"

#include "movement.h"

glm::vec3 move(const Movement& movement, glm::vec3 position)
{
    return glm::mix(position, movement.targetPosition, 0.05f);
}
