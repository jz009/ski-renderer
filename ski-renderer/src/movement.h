#pragma once
#include "includes_fwd.h"

struct Movement
{
    glm::vec3 speed = {0.05, 0.05, 0.05};
    std::deque<glm::vec3> targetPath = std::deque<glm::vec3>();

    glm::vec3 move(glm::vec3 position);
};

std::deque<glm::vec3> aStar(std::shared_ptr<Entity> entity, glm::vec3 target, Scene& scene);