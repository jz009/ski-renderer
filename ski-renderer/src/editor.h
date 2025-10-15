#pragma once
#include "includes_fwd.h"

struct EditorTool {
    glm::vec3 offset;

    virtual bool handleMouseDown(Scene&, const Input&, Editor&) {
        return false;
    };
    virtual bool handleMouseUp(Scene&, const Input&, Editor&) {
        return false;
    };
    virtual bool handleDrag(Scene&, const Input&, Editor&) {
        return false;
    };
};

struct SelectionTool : EditorTool {
    bool handleMouseDown(Scene& scene, const Input& input, Editor& editor) override;
};

struct XZDragTool : EditorTool {
    bool handleMouseDown(Scene& scene, const Input& input, Editor& editor) override;
    bool handleDrag(Scene& scene, const Input& input, Editor& editor) override;
};

struct YDragTool : EditorTool {
    bool handleMouseDown(Scene& scene, const Input& input, Editor& editor) override;
    bool handleDrag(Scene& scene, const Input& input, Editor& editor) override;
};

struct Editor {
    std::shared_ptr<Entity> selectedForEdit;
    glm::vec2 lastFrameMousePos;
    SelectionTool selectionTool;
    std::unique_ptr<EditorTool> currentTool = std::make_unique<XZDragTool>();
    bool mouseDown = false;

    void endEditMode(Scene& scene);
    bool isDragging() { return mouseDown; }
    void onFrame(Scene& scene, const Input& input);
};

