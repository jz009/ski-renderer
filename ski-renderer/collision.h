#pragma once
#include "includes_fwd.h"

#include "shared.h"

void printAABB(AABB aabb)
{
    printf("%f, %f, %f, %f, %f, %f\n", aabb.min.x, aabb.max.x, aabb.min.y, aabb.max.y, aabb.min.z, aabb.max.z);
}

bool pointInAABB(glm::vec3 point, AABB box)
{
    return (
        point.x >= box.min.x &&
        point.x <= box.max.x &&
        point.y >= box.min.y &&
        point.y <= box.max.y&&
        point.z >= box.min.z &&
        point.z <= box.max.z);
}