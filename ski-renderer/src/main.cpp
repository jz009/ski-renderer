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
	Input input(window);

	Scene scene;

	Material basic = renderer.createMaterial(Constants::sDEFAULT, Uniforms(*scene.camera));
	ObjResult cubeObj = loadObj(Constants::mCUBE);
	Model cube = renderer.createModel(cubeObj.vertexData, basic);

	scene.createEntity(EntityType::PLAYER, cubeObj.box, Layer::PLAYER, cube, Transform(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)), { 0.7f, 0.3f, 0.3f, 1.0 });
	scene.createEntity(EntityType::TERRAIN, cubeObj.box, Layer::WALKABLE, cube, Transform(glm::vec3(20.0, 1.0, 20.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-10.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)), { 0.3f, 1.0f, 0.0, 1.0 });
	scene.createEntity(EntityType::TERRAIN, cubeObj.box, Layer::IMPASSABLE, cube, Transform(glm::vec3(1.0, 6.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-10.0, 4.0, 0.0), glm::vec3(0.0, -1.0, 0.0)), { 0.8f, 0.8f, 0.8f, 1.0 });
	scene.createEntity(EntityType::TERRAIN, cubeObj.box, Layer::IMPASSABLE, cube, Transform(glm::vec3(1.0, 6.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-20.0, 4.0, 0.0), glm::vec3(0.0, -1.0, 0.0)), { 0.8f, 0.8f, 0.8f, 1.0 });


	for (auto entity : scene.entities)
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
		if (scene.isEditing()) {
			scene.editor.onFrame(scene, input);
		}
		for (auto entity : scene.entities)
		{
			if (scene.isGame()) {
				entity->onFrame(scene, input);
			}
			else if (scene.isEditing()) {
				entity->applyMatrices(scene);
			}

			renderer.draw(entity->model);
		}
		renderer.endFrame();
		input.clear();
		Time::lastFrame = Time::thisFrame;
	}

	renderer.terminate();

	return 0;
}
