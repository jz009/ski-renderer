#pragma once
#include "includes_fwd.h"

#include "AABB.h"

struct Raycast
{
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 inv_direction;
    int sign[3];
    Raycast(const glm::vec3 &_origin, const glm::vec3 &_direction)
    {
        origin = _origin;
        direction = _direction;
        inv_direction = glm::vec3(1 / direction.x, 1 / direction.y, 1 / direction.z);
        sign[0] = (inv_direction.x < 0);
        sign[1] = (inv_direction.y < 0);
        sign[2] = (inv_direction.z < 0);
    }
};

enum Layer
{
    NONE,
    WALKABLE,
};

struct Intersection
{
    bool intersected;
    glm::vec3 near;
    glm::vec3 far;
};

struct BoxCollider
{
    AABB box;
    std::bitset<32> layerMask;
    std::shared_ptr<Entity> parent;
    BoxCollider(AABB _box, std::vector<Layer> _layers, std::shared_ptr<Entity> _parent)
    {
        box = _box;
        for (int i : _layers)
        {
            layerMask[i] = 1;
        }
        parent = _parent;
    }

    void onFrame();
};

struct Collision
{
    std::shared_ptr<Entity> entity;
    AABB box;
    Intersection intersection;
};

void printAABB(AABB aabb);
bool pointInBox(glm::vec3 point, AABB box);
bool boxOverlap(AABB a, AABB b);
Intersection rayBoxIntersect(AABB box, Raycast ray, float t0, float t1);
Raycast getRayFromMouse(float mouseX, float mouseY, const glm::mat4& view, const glm::mat4& proj);
std::vector<Collision> getCollisionsFromRay(const Raycast& ray, const std::vector<BoxCollider>& colliders, Layer layer);
void transformBox(AABB &box, const glm::mat4x4& modelMatrix);