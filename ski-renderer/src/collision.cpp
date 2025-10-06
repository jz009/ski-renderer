#pragma once
#include "includes_fwd.h"

#include "collision.h"
#include "entity.h"
#include "AABB.h"

void BoxCollider::onFrame()
{
    transformBox(parent->model.material.uniforms.modelMatrix);
}

void BoxCollider::createCollider(AABB _box, std::vector<Layer> _layers, std::shared_ptr<Entity> _parent) {
    box = _box;
    for (Layer i : _layers)
    {
        layerMask[(int)i] = 1;
    }
    parent = _parent;
}

void ColliderTree::add(std::shared_ptr<BoxCollider> collider) {
    collider->parent->collider = collider;
    colliders.push_back(collider);
};

std::vector<BoxCollision> ColliderTree::getBoxCollisions(AABB box) {
    std::vector<BoxCollision> collisions;
    for (std::shared_ptr<BoxCollider> collider : colliders) {
        // box.print();
        // printf("%s: ", collider->parent->name.c_str());
        // collider->box.print();
        if (box.boxOverlap(collider->box, glm::vec3{ 0.0f, 0.1f, 0.0f })) {
            collisions.push_back({ collider });
        }
    }
    return collisions;
}

RayIntersection BoxCollider::rayBoxIntersect(Raycast ray)
{
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    glm::vec3 bounds[2] = { box.min, box.max };
    tmin = (bounds[ray.sign[0]].x - ray.origin.x) * ray.invDirection.x;
    tmax = (bounds[1 - ray.sign[0]].x - ray.origin.x) * ray.invDirection.x;
    tymin = (bounds[ray.sign[1]].y - ray.origin.y) * ray.invDirection.y;
    tymax = (bounds[1 - ray.sign[1]].y - ray.origin.y) * ray.invDirection.y;
    if ((tmin > tymax) || (tymin > tmax))
        return { false };
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    tzmin = (bounds[ray.sign[2]].z - ray.origin.z) * ray.invDirection.z;
    tzmax = (bounds[1 - ray.sign[2]].z - ray.origin.z) * ray.invDirection.z;
    if ((tmin > tzmax) || (tzmin > tmax))
        return { false };
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    return RayIntersection{ ((tmin < ray.maxLength) && (tmax > 0.0f)), ray.origin + std::max(std::max(tmin, tymin), tzmin) * ray.direction, ray.origin + std::min(std::min(tmax, tymax), tzmax) * ray.direction };
}

std::vector<RayCollision> ColliderTree::getRayCollisions(const Raycast& ray) const
{
    std::vector<RayCollision> collisions;
    for (std::shared_ptr<BoxCollider> collider : colliders)
    {
        RayIntersection intersection = collider->rayBoxIntersect(ray);
        if (intersection.intersected)
        {
            collisions.push_back(RayCollision{ collider, intersection });
        }
    }
    return collisions;
}

void BoxCollider::transformBox(const glm::mat4x4& modelMatrix)
{
    box.min = modelMatrix * glm::vec4(box.baseMin, 1.0f);
    box.max = modelMatrix * glm::vec4(box.baseMax, 1.0f);
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
    return Raycast(nearPoint, direction, 100.0f);
}

void ColliderTree::bakeNavMesh() {
    navMesh = NavMesh(colliders);
}

std::bitset<32> NavMesh::getLayers(glm::vec3 point) {
    std::bitset<32> layers;
    for (NavMeshBox box : polygons) {
        if (box.box.pointInBox(point)) {
            layers = box.layerMask | layers;
        }
    }
    return layers;
}

bool NavMesh::lineOfSight(glm::vec3 a, glm::vec3 b, std::bitset<32> layerMask) {
    for (NavMeshBox box : polygons) {
        if ((box.layerMask & layerMask) == box.layerMask) {
            if (box.box.lineIntersect(glm::vec2(a.x, a.z), glm::vec2(b.x, b.z))) {
                return false;
            }
        }
    }
    return true;
}