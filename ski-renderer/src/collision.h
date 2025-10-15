#pragma once
#include "includes_fwd.h"

#include "AABB.h"

struct Raycast
{
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 invDirection;
    float maxLength;
    int sign[3];
    Raycast(const glm::vec3& _origin, const glm::vec3& _direction, float _maxLength)
    {
        origin = _origin;
        direction = _direction;
        invDirection = glm::vec3(1 / direction.x, 1 / direction.y, 1 / direction.z);
        maxLength = _maxLength;
        sign[0] = (invDirection.x < 0);
        sign[1] = (invDirection.y < 0);
        sign[2] = (invDirection.z < 0);
    }
};

enum Layer
{
    NONE = 1 << 0,
    PLAYER = 1 << 1,
    WALKABLE = 1 << 2,
    IMPASSABLE = 1 << 3,
};

struct RayIntersection
{
    bool intersected;
    glm::vec3 near;
    glm::vec3 far;
};

struct BoxCollider
{
    AABB box;
    AABB unscaledBox;
    std::bitset<32> layerMask;

    BoxCollider(AABB _box, std::bitset<32> layers) : box(_box), unscaledBox(_box), layerMask(layers) {};
    RayIntersection rayBoxIntersect(Raycast ray) const;
    void transformBox(const glm::mat4x4& modelMatrix);
};

struct BoxCollision {
    std::shared_ptr<Entity> entity;
};

struct RayCollision
{
    std::shared_ptr<Entity> entity;
    RayIntersection intersection;
};

struct NavMeshBox {
    AABB2D box;
    std::bitset<32> layerMask;
    NavMeshBox() = default;
    NavMeshBox(AABB _box, std::bitset<32> _layerMask) : box(AABB2D::fromAABB(_box)), layerMask(_layerMask) {}
};

struct NavMesh {
    std::vector<NavMeshBox> polygons;

    NavMesh() {}
    NavMesh(std::vector<std::shared_ptr<Entity>> entities);

    std::bitset<32> getLayers(glm::vec3 point);
    bool lineOfSight(glm::vec3 a, glm::vec3 b);
    bool overlaps(AABB box);
    void print() {
        for (const NavMeshBox& box : polygons) {
            box.box.print();
            printf("%s\n", box.layerMask.to_string().c_str());
        }
    }
};

bool isWalkable(std::bitset<32> layerMask);
bool isImpassable(std::bitset<32> layerMask);
