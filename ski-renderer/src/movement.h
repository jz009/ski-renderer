#pragma once
#include "includes_fwd.h"

struct Movement
{
    glm::vec3 speed = {0.05, 0.05, 0.05};
    glm::vec3 targetPosition;
};

glm::vec3 move(const Movement& movement, glm::vec3 position);

