#pragma once
#include "includes_fwd.h"

#include "input.h"
#include "transform.h"
#include "collision.h"
#include "scene.h"
#include "movement.h"
#include "rendering_utils.h"

struct Entity : std::enable_shared_from_this<Entity>
{
    std::string name;
    bool loaded;
    Model model;
    Transform transform;
    std::shared_ptr<BoxCollider> collider;
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
        updateModel(model, transform, *scene.camera);
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
        const Input& input = scene.input;
        bool leftButtonPressed = input.mouseClickInput.fresh && input.mouseClickInput.button == GLFW_MOUSE_BUTTON_LEFT && input.mouseClickInput.action == GLFW_PRESS;
        if (leftButtonPressed)
        {
            glm::vec2 mousePos = input.mouseClickInput.mousePos;
            Raycast ray = getRayFromMouse(mousePos.x, mousePos.y, model.material.uniforms.viewMatrix, model.material.uniforms.projectionMatrix);
            auto colliders = scene.colliders.getRayCollisions(ray);
            if (!colliders.empty()) {
                RayCollision collision = colliders.front();
                if (collision.collider->layerMask[(int)Layer::WALKABLE] && areClose(collision.intersection.near.y, collision.collider->box.max.y)) {
                    auto path = aStar(shared_from_this(), collision.intersection.near, scene);
                    movement.targetPath = path;
                }
            }
        }

        transform.position = movement.move(transform.position);
        updateModel(model, transform, *scene.camera);
    }

    std::optional<Model> getModel() override {
        return model;
    }
};