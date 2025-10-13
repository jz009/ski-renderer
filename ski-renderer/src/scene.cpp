#pragma once
#include "includes_fwd.h"

#include "scene.h"
#include "entity.h"

void Scene::addEntity(std::shared_ptr<Entity> entity) {
	entity->id = nextId;
	entities.push_back(entity);
	nextId++;
}

void Scene::useCamera(CameraType cameraType, const Input& input) {
	camera = cameras[(int)cameraType];
	if (cameraType == CameraType::FirstPersonCamera) {
		input.setMouseModeInvisible();
		camera->lastFrameMousePos = input.mousePosition;
		camera->type = CameraType::FirstPersonCamera;
	}
	else if (cameraType == CameraType::CircleBoundCamera) {
		input.setMouseModeVisible();
		camera->type = CameraType::CircleBoundCamera;
	}
}

void Scene::onFrame(const Input& input) {
	if (input.wasPressed(GLFW_KEY_E)) {
		if (state == SceneState::GAME) {
			state = SceneState::EDIT;
		}
		else if (state == SceneState::EDIT) {
			editor.endEditMode(*this);
			state = SceneState::GAME;
		}

		useCamera(CameraType::CircleBoundCamera, input);
	}

	viewMatrix = calculateViewMatrix(*camera);
}

std::vector<RayCollision> Scene::getRayCollisions(const Raycast& ray)
{
	std::vector<RayCollision> collisions;
	for (std::shared_ptr<Entity> entity : entities)
	{
		RayIntersection intersection = entity->collider.rayBoxIntersect(ray);
		if (intersection.intersected)
		{
			collisions.push_back(RayCollision{ entity, intersection });
		}
	}
	return collisions;
}