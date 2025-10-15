#pragma once
#include "includes_fwd.h"

struct Movement
{
    float speed = 15.0f;
    std::deque<glm::vec3> targetPath = std::deque<glm::vec3>();

    glm::vec3 move(glm::vec3 position);
    glm::vec3 firstPersonMove(glm::vec3 position, glm::vec3 direction);
};

std::deque<glm::vec3> findPath(const Entity& entity, glm::vec3 target, Scene& scene);