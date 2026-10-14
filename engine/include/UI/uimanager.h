#pragma once

#include <core/enginecontext.h>
#include <core/imgui.h>
#include <core/engine.h>
#include <core/input.h>

#include <ui/uimanager.h>
#include <renderer/lightsmanager.h>
#include <player/player.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <core/shapes.h>

#include <string>

#include <imguizmo/imguizmo.h>

#include <entitymanager/entitymanager.h>
#include <scene/scene.h>

#include <core/components.h>
#include <core/properties.h>

#include <Prefab/prefabregistry.h>

#include <cstdarg>

class UIManager
{
public:
    UIManager(EngineContext* context);
    void Update();
    void Close();
    void DrawEntityNode(EntityID entity);
    void CreateChild(EntityID parententity, PrefabDefinition prefab);
    void ReparentChild(EntityID child, EntityID newparent);
    void RemoveChildFromParent(EntityID child);
    bool IsDescendantOf(EntityID entity, EntityID potentialAncestor);

private:
    MouseState m_LastMouseState;
    ImGuiIO* m_IO = nullptr;
    EngineContext* m_Context = nullptr;

    EntityID m_CurrentObjectID = {INVALID_ID};
    ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
};