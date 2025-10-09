#pragma once
#include "includes_fwd.h"

#include "scene.h"
#include "entity.h"

void Scene::addEntity(std::shared_ptr<Entity> entity) {
	entities.push_back(entity);
	colliders.add(entity->collider);
}