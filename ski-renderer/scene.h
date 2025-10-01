#pragma once
#include "includes_fwd.h"

#include "entity.h"
#include "shared.h"

struct Scene {
    std::vector<std::unique_ptr<Entity>> entities;

    void addEntity(std::unique_ptr<Entity> entity);
};
