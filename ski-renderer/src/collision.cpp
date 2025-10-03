#pragma once
#include "includes_fwd.h"

#include "collision.h"
#include "entity.h"
#include "AABB.h"

void BoxCollider::onFrame()
{
    transformBox(box, parent->model.material.uniforms.projectionMatrix, parent->model.material.uniforms.viewMatrix, parent->model.material.uniforms.modelMatrix);
}

void printAABB(AABB aabb)
{
    printf("%f, %f, %f, %f, %f, %f\n", aabb.min.x, aabb.max.x, aabb.min.y, aabb.max.y, aabb.min.z, aabb.max.z);
}

bool pointInBox(glm::vec3 point, AABB box)
{
    return (
        point.x >= box.min.x &&
        point.x <= box.max.x &&
        point.y >= box.min.y &&
        point.y <= box.max.y &&
        point.z >= box.min.z &&
        point.z <= box.max.z);
}

bool boxOverlap(AABB a, AABB b)
{
    return (
        a.min.x <= b.max.x &&
        a.max.x >= b.min.x &&
        a.min.y <= b.max.y &&
        a.max.y >= b.min.y &&
        a.min.z <= b.max.z &&
        a.max.z >= b.min.z);
}

Intersection rayBoxIntersect(AABB box, Raycast ray, float t0, float t1)
{
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    glm::vec3 bounds[2] = {box.min, box.max};
    tmin = (bounds[ray.sign[0]].x - ray.origin.x) * ray.inv_direction.x;
    tmax = (bounds[1 - ray.sign[0]].x - ray.origin.x) * ray.inv_direction.x;
    tymin = (bounds[ray.sign[1]].y - ray.origin.y) * ray.inv_direction.y;
    tymax = (bounds[1 - ray.sign[1]].y - ray.origin.y) * ray.inv_direction.y;
    if ((tmin > tymax) || (tymin > tmax))
        return {false};
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    tzmin = (bounds[ray.sign[2]].z - ray.origin.z) * ray.inv_direction.z;
    tzmax = (bounds[1 - ray.sign[2]].z - ray.origin.z) * ray.inv_direction.z;
    if ((tmin > tzmax) || (tzmin > tmax))
        return {false};
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    return Intersection{((tmin < t1) && (tmax > t0)), ray.origin + std::max(std::max(tmin, tymin), tzmin) * ray.direction, ray.origin + std::min(std::min(tmax, tymax), tzmax) * ray.direction};
}

Raycast getRayFromMouse(
    float mouseX, float mouseY,
    const glm::mat4& view,
    const glm::mat4& proj)
{
    glm::vec4 viewport(0, 0, Constants::WIDTH, Constants::HEIGHT);

    glm::vec3 nearPoint = glm::unProject(glm::vec3(mouseX, Constants::HEIGHT - mouseY, 0.0f), view, proj, viewport);
    glm::vec3 farPoint = glm::unProject(glm::vec3(mouseX, Constants::HEIGHT - mouseY, 1.0f), view, proj, viewport);

    glm::vec3 direction = glm::normalize(farPoint - nearPoint);
    return Raycast(nearPoint, direction);
}

std::vector<Collision> getCollisionsFromRay(const Raycast &ray, const std::vector<BoxCollider> &colliders, Layer layer)
{
    std::vector<Collision> collisions;
    for (const BoxCollider &collider : colliders)
    {
        Intersection intersection = rayBoxIntersect(collider.box, ray, 0.0f, 100.0f);
        if (intersection.intersected && collider.layerMask[layer])
        {
            collisions.push_back(Collision{collider.parent, collider.box, intersection});
        }
    }
    return collisions;
}

void transformBox(AABB& box, const glm::mat4x4& projMatrix, const glm::mat4x4& viewMatrix, const glm::mat4x4& modelMatrix)
{
    box.min = modelMatrix * glm::vec4(box.baseMin, 0.0f);
    box.max = modelMatrix * glm::vec4(box.baseMax, 0.0f);
}