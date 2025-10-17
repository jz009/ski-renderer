#pragma once
#include "includes_fwd.h"

#include "movement.h"
#include "collision.h"
#include "rendering_utils.h"
#include "transform.h"

enum struct EntityType {
    PLAYER,
    TERRAIN,
};

struct Entity
{
    EntityType type;
    Model model;
    Transform transform;
    BoxCollider collider;
    Movement movement;
    glm::vec4 color;
    void onFrame(Scene& scene, const Input& input) {
        switch (type) {
        case EntityType::PLAYER:
        playerOnFrame(scene, input);
        break;
        case EntityType::TERRAIN:
        terrainOnFrame(scene, input);
        break;
        }
    }
    void playerOnFrame(Scene& scene, const Input& input);
    void playerFirstPersonOnFrame(Scene& scene, const Input& input);
    void playerThirdPersonOnFrame(Scene& scene, const Input& input);
    void terrainOnFrame(Scene& scene, const Input&);
    void applyMatrices(const Scene& scene);
    Entity::Entity(BoxCollider box, const Camera& camera) : collider(box), model(camera) {}
};