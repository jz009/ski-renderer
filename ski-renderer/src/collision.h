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

enum struct Layer
{
    NONE,
    PLAYER,
    WALKABLE,
    IMPASSABLE,
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
    std::bitset<32> layerMask;

    void createCollider(AABB _box, const std::vector<Layer>& _layers);
    RayIntersection rayBoxIntersect(Raycast ray);
    void transformBox(const glm::mat4x4& modelMatrix);
};

struct BoxCollision {
    std::shared_ptr<BoxCollider> collider;
};

struct RayCollision
{
    BoxCollider collider;
    RayIntersection intersection;
};

struct NavMeshBox {
    AABB2D box;
    std::bitset<32> layerMask;

    NavMeshBox(AABB _box,  std::bitset<32> _layerMask) : box(AABB2D::fromAABB(_box)), layerMask(_layerMask) {}
};

struct NavMesh {
    std::vector<NavMeshBox> polygons;

    NavMesh() {}
    NavMesh(std::vector<std::shared_ptr<BoxCollider>> colliders) {
        for (std::shared_ptr<BoxCollider> box : colliders) {
            polygons.push_back(NavMeshBox(box->box, box->layerMask));
        }
    }

    std::bitset<32> getLayers(glm::vec3 point);
    bool lineOfSight(glm::vec3 a, glm::vec3 b, std::bitset<32> layerMask);
    void print() {
        for (NavMeshBox box : polygons) {
            box.box.print();
            printf("%s", box.layerMask.to_string().c_str());
        }
    }
};

struct ColliderTree {
    std::vector<std::shared_ptr<BoxCollider>> colliders;
    NavMesh navMesh;

    void add(std::shared_ptr<BoxCollider> collider);
    void bakeNavMesh();
    std::vector<BoxCollision> getBoxCollisions(AABB box);
    std::vector<RayCollision> getRayCollisions(const Raycast& ray) const;
};



Raycast getRayFromMouse(float mouseX, float mouseY, const glm::mat4& view, const glm::mat4& proj);
