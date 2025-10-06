#pragma once
#define WEBGPU_CPP_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "renderer.h"
#include "input.h"
#include "entity.h"
#include "collision.h"
#include "camera.h"
#include "scene.h"

int main()
{
	Renderer renderer;
	GLFWwindow* window = renderer.window;
	int count = 0;

	auto camera = std::make_unique<CircleBoundCamera>(20.0f);
	Scene scene(std::move(camera), window);

	auto player = std::make_shared<Player>();
	player->name = "player";
	auto terrain = std::make_shared<Terrain>();
	terrain->name = "ground";
	auto terrain2 = std::make_shared<Terrain>();
	terrain2->name = "low ground";
	auto terrain3 = std::make_shared<Terrain>();
	terrain3->name = "wall";
	Material basic = renderer.createMaterial(Constants::sDEFAULT, Uniforms(*scene.camera));
	ObjResult cubeObj = loadObj(Constants::mCUBE);
	Model cube = renderer.createModel(cubeObj.vertexData, basic);

	auto playerCollider = std::make_shared<BoxCollider>();
	playerCollider->createCollider(cubeObj.box, std::vector{ Layer::PLAYER }, player);
	auto collider = std::make_shared<BoxCollider>();
	collider->createCollider(cubeObj.box, std::vector{ Layer::WALKABLE }, terrain);
	auto collider2 = std::make_shared<BoxCollider>();
	collider2->createCollider(cubeObj.box, std::vector{ Layer::WALKABLE }, terrain2);
	auto collider3 = std::make_shared<BoxCollider>();
	collider3->createCollider(cubeObj.box, std::vector{ Layer::IMPASSABLE }, terrain3);


	player->model = cube;
	player->transform = Transform(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	player->model.material.uniforms.color = { 0.7f, 0.3f, 0.3f, 1.0 };

	terrain->model = cube;
	terrain->transform = Transform(glm::vec3(10.0, 1.0, 10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-10.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	terrain->model.material.uniforms.color = { 0.3f, 1.0f, 0.0, 1.0 };

	terrain2->model = cube;
	terrain2->transform = Transform(glm::vec3(10.0, 1.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(10.0, -14.0, 3.0), glm::vec3(0.0, -1.0, 0.0));
	terrain2->model.material.uniforms.color = { 0.3f, 0.6f, 0.5f, 1.0 };

	terrain3->model = cube;
	terrain3->transform = Transform(glm::vec3(1.0, 6.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-10.0, 4.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	terrain3->model.material.uniforms.color = { 0.8f, 0.8f, 0.8f, 1.0 };

	scene.entities.push_back(terrain);
	scene.entities.push_back(player);
	scene.entities.push_back(terrain2);
	scene.entities.push_back(terrain3);
	scene.colliders.add(collider);
	scene.colliders.add(collider2);
	scene.colliders.add(collider3);
	scene.colliders.add(playerCollider);

	for (const auto& entity : scene.entities)
	{
		entity->onFrame(scene);
	}
	for (auto& c : scene.colliders.colliders) {
		c->onFrame();
	}

	scene.colliders.bakeNavMesh();

	while (renderer.isRunning())
	{
		count = count + 1;
		renderer.beginFrame();
		for (const auto& entity : scene.entities)
		{
			entity->onFrame(scene);
			auto model = entity->getModel();
			if (model) {
				renderer.draw(*model);
			}
		}
		for (auto& c : scene.colliders.colliders) {
			c->onFrame();
		}
		scene.camera->onFrame(scene);
		renderer.endFrame();
		scene.input.clear();
	}

	renderer.terminate();

	return 0;
}
