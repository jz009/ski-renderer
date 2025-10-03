#pragma once
#include "includes_fwd.h"

#include "input.h"
#include "transform.h"
#include "collision.h"
#include "scene.h"
#include "movement.h"
#include "rendering_utils.h"

struct Entity
{
    bool loaded;
    Model model;
    Transform transform;
    virtual void onFrame(Scene&) {}
    virtual void onLoad() {}
    virtual std::optional<Model> getModel()
    {
        return std::nullopt;
    }
};

struct Terrain : Entity
{
    void onFrame(Scene& scene) override
    {
        (void)scene;
        updateModel(model, transform, scene.camera);
    }
    std::optional<Model> getModel() override {
        return model;
    }
};

struct Player : Entity
{
    Movement movement;
    void onFrame(Scene& scene) override
    {
        if (getInput()->mouseClickInput.fresh)
        {
            glm::vec2 mousePos = glm::vec2(getInput()->mouseClickInput.mousePos);
            Raycast ray = getRayFromMouse(mousePos.x, mousePos.y, model.material.uniforms.viewMatrix, model.material.uniforms.projectionMatrix);
            auto colliders = getCollisionsFromRay(ray, scene.colliders, Layer::WALKABLE);
            if (!colliders.empty()) {
                Collision collider = colliders.front();
                if (std::abs(collider.intersection.near.y - collider.box.max.y) < .001) {
                    movement.targetPosition = collider.intersection.near;
                }
            }
        }

        transform.position = move(movement, transform.position);
        updateModel(model, transform, scene.camera);
    }

    std::optional<Model> getModel() override {
        return model;
    }
};