#pragma once
#include "includes_fwd.h"

#include "scene.h"
#include "entity.h"
#include "camera.h"
#include "input.h"

Scene::Scene() {
	cameras[(int)CameraType::CircleBoundCamera] = std::make_shared<CircleBoundCamera>(20.0f);
	cameras[(int)CameraType::FirstPersonCamera] = std::make_shared<FirstPersonCamera>();
	camera = cameras[(int)CameraType::CircleBoundCamera];
}

void Scene::createEntity(EntityType type, const AABB& boundingBox, std::bitset<32> layers, Model& model, const Transform& transform, glm::vec4 color) {
    std::shared_ptr<Entity> entity = std::make_shared<Entity>(BoxCollider(boundingBox, layers), *camera);
    entity->type = type;
    entity->model = model;
    entity->transform = transform;
	entity->color = color;
    entity->model.material.uniforms.color = color;
    entity->model.transforms.modelMatrix = calculateModelMatrix(transform);
    entity->model.transforms.viewMatrix = calculateViewMatrix(*camera);
    entity->collider.transformBox(model.transforms.modelMatrix);
    entities.push_back(entity);
}

std::shared_ptr<Entity> Scene::copyEntity(Entity entity) {
    std::shared_ptr<Entity> entityCopy = std::make_shared<Entity>(BoxCollider(entity.collider.unscaledBox, entity.collider.layerMask), *camera);
    entityCopy->type = entity.type;
    entityCopy->model = entity.model;
    entityCopy->transform = entity.transform;
	entityCopy->color = entity.color;
    entityCopy->model.material.uniforms.color = entity.color;
    entityCopy->model.transforms.modelMatrix = calculateModelMatrix(entityCopy->transform);
    entityCopy->model.transforms.viewMatrix = calculateViewMatrix(*camera);
    entityCopy->collider.transformBox(entityCopy->model.transforms.modelMatrix);
    entities.push_back(entityCopy);
	return entityCopy;
}

void Scene::deleteEntity(std::shared_ptr<Entity> entity) {
	entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

bool Scene::isEditing() {
	return state == SceneState::EDIT;
}

bool Scene::isGame() {
	return state == SceneState::GAME;
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
	if (input.wasKeyPressed(GLFW_KEY_E)) {
		if (isGame()) {
			state = SceneState::EDIT;
		}
		else if (isEditing()) {
			editor.endEditMode(*this);
			state = SceneState::GAME;
		}

		useCamera(CameraType::CircleBoundCamera, input);
	}
	// currentCursor->material.uniforms.position = glm::vec4(input.mousePosition / glm::vec2(Constants::WIDTH, Constants::HEIGHT), 0.0f, 1.0f);
	viewMatrix = calculateViewMatrix(*camera);
}

std::vector<RayCollision> Scene::getRayCollisions(const Raycast& ray)
{
	std::vector<RayCollision> collisions;
	collisions.reserve(entities.size());
	for (const auto& entity : entities)
	{
		RayIntersection intersection = entity->collider.rayBoxIntersect(ray);
		if (intersection.intersected)
		{
			collisions.push_back(RayCollision{ entity, intersection });
		}
	}
	std::sort(collisions.begin(), collisions.end(), [this](RayCollision a, RayCollision b){ return glm::distance(this->camera->position, a.intersection.near) < glm::distance(this->camera->position, b.intersection.near); });
	return collisions;
}

Raycast Scene::getRayFromMouse(glm::vec2 mousePos)
{
	glm::vec4 viewport(0, 0, Constants::WIDTH, Constants::HEIGHT);

	glm::vec3 nearPoint = glm::unProject(glm::vec3(mousePos.x, Constants::HEIGHT - mousePos.y, 0.0f), viewMatrix, projectionMatrix, viewport);
	glm::vec3 farPoint = glm::unProject(glm::vec3(mousePos.x, Constants::HEIGHT - mousePos.y, 1.0f), viewMatrix, projectionMatrix, viewport);

	glm::vec3 direction = glm::normalize(farPoint - nearPoint);
	return Raycast(nearPoint, direction, 100.0f);
}

glm::vec3 Scene::getWorldPosFromMouseAtY(glm::vec2 mousePos, float y) {
	Raycast ray = getRayFromMouse(mousePos);
	float t = (y - ray.origin.y) / ray.direction.y;
	float x = ray.origin.x + (t * ray.direction.x);
	float z = ray.origin.z + (t * ray.direction.z);
	return glm::vec3(x, y, z);
}

glm::vec3 Scene::getWorldPosFromMouseAtXZ(glm::vec2 mousePos, float x, float z) {
	Raycast ray = getRayFromMouse(mousePos);
	float t;
	if (std::abs(ray.direction.x) > std::abs(ray.direction.z)) {
		t = (x - ray.origin.x) / ray.direction.x;
	}
	else {
		t = (z - ray.origin.z) / ray.direction.z;
	}

	float y = ray.origin.y + t * ray.direction.y;
	return glm::vec3(x, y, z);
}