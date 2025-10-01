#pragma once
#include "includes_fwd.h"

#include "shared.h"

struct Entity
{
    bool loaded;
    Model model;
    virtual void onFrame() {}
    virtual void onLoad() {}
    virtual std::optional<Model> getModel() {
        return std::nullopt;
    }
};

struct Terrain : Entity
{
    Transform transform;
    void onFrame() override
    {
        updateTransform(model, transform);
    }
    std::optional<Model> getModel() override {
        return model;
    }
};

struct Player : Entity
{
    Transform transform;
    Movement movement;
    void onFrame() override
    {
        if (getInput()->keyboardInput.fresh)
        {
            glm::vec2 mousePos = glm::vec2(getInput()->keyboardInput.mousePos);
            auto clickLocation = getMouseWorld(mousePos.x, mousePos.y, model.material.uniforms.viewMatrix, model.material.uniforms.projectionMatrix);
            movement.targetPosition = clickLocation;
            // for (Model m : models)
            // {
            //     // if (pointInAABB(clickLocation - glm::vec3(0.0, -0.1, 0.0), m.adjustedBox))
            //     // {
            //     //     moveable.targetPosition = clickLocation;
            //     //     break;
            //     // }
            // }
        }

        transform.position = move(movement, transform.position);
        updateTransform(model, transform);
    }
    std::optional<Model> getModel() override {
        return model;
    }
};
