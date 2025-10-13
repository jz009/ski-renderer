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
    BoxCollider collider;
    int id;
    std::array<float, 4> color = {0.0f, 1.0f, 0.0f, 1.0f};
    virtual void onFrame(Scene&, const Input&) {}
    void applyMatrices(const Scene& scene) {
        model.material.uniforms.modelMatrix = calculateModelMatrix(transform);
        model.material.uniforms.viewMatrix = scene.viewMatrix;
        collider.transformBox(model.material.uniforms.modelMatrix);
    }
    virtual void onLoad() {}
    virtual Model* getModel()
    {
        return nullptr;
    }
    Entity::Entity(BoxCollider box) : collider(box) {}
};

struct Terrain : Entity
{
    void onFrame(Scene& scene, const Input& input) override
    {
        (void)scene;
        (void)input;
        applyMatrices(scene);
    }
    Model* getModel() override {
        return &model;
    }
    Terrain::Terrain(BoxCollider box) : Entity::Entity(box) {}
};

struct Player : Entity
{
    Movement movement;
    void onFrame(Scene& scene, const Input& input) override
    {
        if (scene.camera->type == CameraType::FirstPersonCamera) {
            firstPersonOnFrame(scene, input);
        }
        else if (scene.camera->type == CameraType::CircleBoundCamera) {
            thirdPersonOnFrame(scene, input);
        }
        transform.position = movement.move(transform.position);
        applyMatrices(scene);
    }

    void firstPersonOnFrame(Scene& scene, const Input& input);
    void thirdPersonOnFrame(Scene& scene, const Input& input);

    Model* getModel() override {
        return &model;
    }

    Player::Player(BoxCollider box) : Entity::Entity(box) {}

};