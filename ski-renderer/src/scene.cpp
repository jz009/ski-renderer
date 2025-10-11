#pragma once
#include "includes_fwd.h"

#include "scene.h"
#include "entity.h"

void Scene::addEntity(std::shared_ptr<Entity> entity) {
	entities.push_back(entity);
	colliders.add(entity->collider);
}

void Scene::useCamera(CameraType cameraType, const Input& input) {
	camera = cameras[(int)cameraType];
	if (cameraType == CameraType::FirstPersonCamera) {
		input.setMouseModeInvisible();
		camera->lastFrameMousePos = input.mousePosition;
		camera->state = CameraState::FIRST_PERSON;
	}
}