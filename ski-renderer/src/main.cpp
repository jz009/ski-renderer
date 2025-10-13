#pragma once
#define WEBGPU_CPP_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "renderer.h"
#include "input.h"
#include "entity.h"
#include "collision.h"
#include "camera.h"
#include "scene.h"

std::shared_ptr<Player> createPlayer(const std::string& name, const AABB& boundingBox, std::bitset<32> layers, Model& model, const Transform& transform, std::array<float, 4Ui64> color, const Scene& scene) {
	auto player = std::make_shared<Player>(BoxCollider(boundingBox, layers));
	player->name = name;
	player->model = model;
	player->transform = transform;
	player->model.material.uniforms.color = color;
	model.material.uniforms.modelMatrix = calculateModelMatrix(transform);
	model.material.uniforms.viewMatrix = calculateViewMatrix(*scene.camera);
	player->collider.transformBox(model.material.uniforms.modelMatrix);
	return player;
}

std::shared_ptr<Terrain> createTerrain(const std::string& name, const AABB& boundingBox, std::bitset<32> layers, Model& model, const Transform& transform, std::array<float, 4Ui64> color, const Scene& scene) {
	auto terrain = std::make_shared<Terrain>(BoxCollider(boundingBox, layers));
	terrain->name = name;
	terrain->model = model;
	terrain->transform = transform;
	terrain->model.material.uniforms.color = color;
	model.material.uniforms.modelMatrix = calculateModelMatrix(transform);
	model.material.uniforms.viewMatrix = calculateViewMatrix(*scene.camera);
	terrain->collider.transformBox(model.material.uniforms.modelMatrix);
	return terrain;
}

int main()
{
	Renderer renderer;
	GLFWwindow* window = renderer.window;
	int count = 0;
	Input input(window);

	Scene scene;

	Material basic = renderer.createMaterial(Constants::sDEFAULT, Uniforms(*scene.camera));
	ObjResult cubeObj = loadObj(Constants::mCUBE);
	Model cube = renderer.createModel(cubeObj.vertexData, basic);

	auto player = createPlayer("player", cubeObj.box, Layer::PLAYER, cube, Transform(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)), { 0.7f, 0.3f, 0.3f, 1.0 }, scene);
	auto terrain = createTerrain("ground", cubeObj.box, Layer::WALKABLE, cube, Transform(glm::vec3(20.0, 1.0, 20.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-10.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)), { 0.3f, 1.0f, 0.0, 1.0 }, scene);
	auto terrain2 = createTerrain("wall1", cubeObj.box, Layer::IMPASSABLE, cube, Transform(glm::vec3(1.0, 6.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-10.0, 4.0, 0.0), glm::vec3(0.0, -1.0, 0.0)), { 0.8f, 0.8f, 0.8f, 1.0 }, scene);
	auto terrain3 = createTerrain("wall2", cubeObj.box, Layer::IMPASSABLE, cube, Transform(glm::vec3(1.0, 6.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-20.0, 4.0, 0.0), glm::vec3(0.0, -1.0, 0.0)), { 0.8f, 0.8f, 0.8f, 1.0 }, scene);

	scene.addEntity(player);
	scene.addEntity(terrain);
	scene.addEntity(terrain2);
	scene.addEntity(terrain3);

	for (const auto& entity : scene.entities)
	{
		entity->onFrame(scene, input);
	}

	scene.navMesh = NavMesh(scene.entities);

	scene.viewMatrix = calculateViewMatrix(*scene.camera);
	scene.projectionMatrix = calculateProjectionMatrix(*scene.camera);

	Time::lastFrame = std::chrono::high_resolution_clock::now();

	while (renderer.isRunning())
	{
		Time::thisFrame = std::chrono::high_resolution_clock::now();
		Time::deltaTime = (float)(std::chrono::duration_cast<std::chrono::milliseconds>(Time::thisFrame - Time::lastFrame).count() / 1000.0f);
		//printf("fps: %f\n", 1.0f / Time::deltaTime);

		count = count + 1;
		renderer.beginFrame();
		scene.onFrame(input);
		scene.camera->onFrame(scene, input);
		for (const auto& entity : scene.entities)
		{
			if (scene.state == SceneState::GAME) {
				entity->onFrame(scene, input);
			} 
			else if (scene.state == SceneState::EDIT) {
				scene.editor.onFrame(scene, input);
				entity->applyMatrices(scene);
			}

			if (Model* model = entity->getModel()) {
				renderer.draw(*model);
			}
		}
		renderer.endFrame();
		input.clear();
		Time::lastFrame = Time::thisFrame;
	}

	renderer.terminate();

	return 0;
}
