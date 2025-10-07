#pragma once
#include "includes_fwd.h"

#include "movement.h"
#include "scene.h"
#include "entity.h"

struct Candidate {
    glm::vec3 position;
    float f;

    bool operator>(const Candidate& other) const {
        return f > other.f;
    }

    Candidate(glm::vec3 _position) : position(_position) {}
};

template<> struct std::hash<glm::vec3> {
    size_t operator()(const glm::vec3& v) const noexcept {
        size_t h1 = std::hash<int>()((int)(v.x * 65536));
        size_t h2 = std::hash<int>()((int)(v.y * 65536));
        size_t h3 = std::hash<int>()((int)(v.z * 65536));
        return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
    }
};

template<> struct std::hash<Candidate> {
    size_t operator()(const Candidate& v) const noexcept {
        size_t h1 = std::hash<int>()((int)(v.position.x * 65536));
        size_t h2 = std::hash<int>()((int)(v.position.y * 65536));
        size_t h3 = std::hash<int>()((int)(v.position.z * 65536));
        return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
    }
};

template<> struct std::equal_to<Candidate> {
    bool operator()(const Candidate& a, const Candidate& b) const noexcept {
        return areClose(a.position.x, b.position.x) && areClose(a.position.y, b.position.y) && areClose(a.position.z, b.position.z);
    }
};

glm::vec3 Movement::move(glm::vec3 position)
{
    if (targetPath.size() == 0) {
        return position;
    }
    if (glm::distance(position, targetPath.front()) < 0.1f) {
        targetPath.pop_front();
    }
    if (targetPath.size() == 0) {
        return position;
    }
    glm::vec3 targetPosition = targetPath.front();
    float distance = glm::distance(position, targetPosition);
    float finalSpeed = (distance / speed);
    return glm::mix(position, targetPosition, Time::deltaTime / finalSpeed);
}

std::deque<glm::vec3> constructPath(std::unordered_map<glm::vec3, glm::vec3> cameFrom, glm::vec3 current) {
    std::deque<glm::vec3> path;
    path.push_back(current);
    while (cameFrom.count(current)) {
        current = cameFrom[current];
        path.push_front(current);
    }
    return path;
}

std::vector<glm::vec3> getNeighbors(glm::vec3 center, std::shared_ptr<Entity> entity, Scene& scene) {
    std::vector<glm::vec3> neighbors;
    auto colliders = scene.colliders;

    float step = 1.0f;
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            float dx = center.x + step * i;
            float dz = center.z + step * j;
            if (j == 0 && i == 0) {
                continue;
            }
            glm::vec3 point(dx, center.y, dz);
            auto layers = scene.colliders.navMesh.getLayers(point);
            if (layers[(int)Layer::WALKABLE] && !layers[(int)Layer::IMPASSABLE])
                neighbors.push_back(point);
        }
    }
    return neighbors;
}

std::deque<glm::vec3> aStar(std::shared_ptr<Entity> entity, const glm::vec3 target, Scene& scene) {
    int count = 0;
    if (!areClose(entity->transform.position.y, target.y)) {
        return std::deque<glm::vec3>();
    }

    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> candidates;
    std::unordered_set<glm::vec3> visited;
    std::unordered_map<glm::vec3, glm::vec3> cameFrom;
    std::unordered_map<glm::vec3, float> gScore;

    Candidate start = { entity->transform.position };
    start.f = glm::distance(start.position, target);

    candidates.push(start);
    visited.insert(start.position);
    gScore[start.position] = 0.0f;

    while (!candidates.empty()) {
        count++;
        if (count > 1000) {
            break;
        }
        const Candidate& current = candidates.top();
        candidates.pop();

        if (glm::distance(current.position, target) < 0.6f) {
            return constructPath(cameFrom, current.position);
        }

        std::bitset<32> impassable;
        impassable.flip((int)Layer::IMPASSABLE);
        bool lineOfSight = scene.colliders.navMesh.lineOfSight(current.position, target, impassable);
        if (lineOfSight) {
            cameFrom[target] = current.position;
            return constructPath(cameFrom, target);
        }

        for (Candidate candidate : getNeighbors(current.position, entity, scene)) {
            if (visited.find(candidate.position) == visited.end()) {
                gScore[candidate.position] = FLT_MAX;
                visited.insert(candidate.position);
            }

            float tg = gScore[current.position] + glm::distance(current.position, candidate.position);
            if (tg < gScore[candidate.position]) {
                cameFrom[candidate.position] = current.position;
                gScore[candidate.position] = tg;
                candidate.f = tg + glm::distance(candidate.position, target);
                candidates.push(candidate);
            }
        }
    }

    return std::deque<glm::vec3>();
}

