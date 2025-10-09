#pragma once
#include "includes_fwd.h"

struct AABB
{
    glm::vec3 baseMin;
    glm::vec3 baseMax;
    glm::vec3 min;
    glm::vec3 max;

    AABB::AABB() = default;
    AABB(glm::vec3 _min, glm::vec3 _max) : baseMin(_min), baseMax(_max), min(_min), max(_max) {}

    void AABB::print()
    {
        printf("%f, %f, %f, %f, %f, %f\n", min.x, min.y, min.z, max.x, max.y, max.z);
    }

    bool AABB::pointInBox(glm::vec3 point)
    {
        return (
            point.x >= min.x &&
            point.x <= max.x &&
            point.y >= min.y &&
            point.y <= max.y &&
            point.z >= min.z &&
            point.z <= max.z);
    }

    bool AABB::boxOverlap(AABB b, glm::vec3 tolerance)
    {
        return
            (min.x <= b.max.x + tolerance.x && max.x >= b.min.x - tolerance.x) &&
            (min.y <= b.max.y + tolerance.y && max.y >= b.min.y - tolerance.y) &&
            (min.z <= b.max.z + tolerance.z && max.z >= b.min.z - tolerance.z);
    }
};

struct AABB2D {
    glm::vec2 min;
    glm::vec2 max;

    AABB2D::AABB2D() = default;
    AABB2D(glm::vec2 _min, glm::vec2 _max) : min(_min), max(_max) {}

    void AABB2D::print() const
    {
        printf("min: %f, %f, max: %f, %f\n", min.x, min.y, max.x, max.y);
    }

    bool AABB2D::pointInBox(glm::vec3 point) const
    {
        return (
            point.x >= min.x &&
            point.x <= max.x &&
            point.z >= min.y &&
            point.z <= max.y);
    }

    bool AABB2D::boxOverlap(AABB b) const
    {
        return
            (min.x <= b.max.x && max.x >= b.min.x) &&
            (min.y <= b.max.y && max.y >= b.min.y);
    }

    bool AABB2D::lineIntersect(glm::vec2 a, glm::vec2 b) const {
        float lineMinX = std::min(a.x, b.x);
        float lineMinY = std::min(a.y, b.y);
        float lineMaxX = std::max(a.x, b.x);
        float lineMaxY = std::max(a.y, b.y);
        if (lineMaxX < min.x || lineMinX > max.x || lineMaxY < min.y || lineMinY > max.y) {
            return false;
        }

        float dx = a.x - b.x;
        float dy = a.y - b.y;

        float p[4] = { -dx, dx, -dy, dy };
        float q[4] = { b.x - min.x, max.x - b.x, b.y - min.y, max.y - b.y };

        float t0 = 0.0f;
        float t1 = 1.0f;

        for (int i = 0; i < 4; ++i) {
            if (p[i] == 0.0f) {
                if (q[i] < 0.0f)
                    return false;
            }
            else {
                float r = q[i] / p[i];
                if (p[i] < 0.0f) {
                    t0 = std::max(t0, r);
                }
                else {
                    t1 = std::min(t1, r);
                }
                if (t0 > t1)
                    return false;
            }
        }
        return true;
    }

    static AABB2D AABB2D::fromAABB(AABB aabb) {
        return { {aabb.min.x, aabb.min.z}, {aabb.max.x, aabb.max.z} };
    }
};