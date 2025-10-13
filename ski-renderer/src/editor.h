#pragma once
#include "includes_fwd.h"

#include "input.h"

enum struct EditState {
    NONE,
    DRAG,
};

struct Editor {
    EditState state = EditState::NONE;
    std::shared_ptr<Entity> selectedForEdit;
    glm::vec2 lastFrameMousePos;

    void endEditMode(Scene& scene);
    void onFrame(Scene& scene, const Input& input);
};
