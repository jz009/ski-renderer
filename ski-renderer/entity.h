#pragma once
#include "includes_fwd.h"

#include "shared.h"

struct Entity
{
    bool loaded;
    virtual void onFrame() {}
    virtual void onLoad() {}
};

struct Terrain : Entity
{
    Model model;
    Transform transform;
    void onFrame() override
    {
        //updateModel(model);
        // models.push_front(model);
    }
};

struct Player : Entity
{
    Model model;
    Transform transform;
    Movement movement;
    void onFrame() override
    {
        if (getInput()->keyboardInput.fresh)
        {
            glm::vec2 mousePos = glm::vec2(getInput()->keyboardInput.mousePos);
            auto clickLocation = getMouseWorld(mousePos.x, mousePos.y, model.material.uniforms.viewMatrix, model.material.uniforms.projectionMatrix);
            // for (Model m : models)
            // {
            //     // if (pointInAABB(clickLocation - glm::vec3(0.0, -0.1, 0.0), m.adjustedBox))
            //     // {
            //     //     moveable.targetPosition = clickLocation;
            //     //     break;
            //     // }
            // }
        }

        // move(moveable, model.position);
        // updateModel(model);
        //models.push_front(model);
    }
};