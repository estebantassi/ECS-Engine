#pragma once

#include <core/enginecontext.h>
#include <core/imgui.h>
#include <core/types.h>
#include <core/engine.h>
#include <imguizmo/imguizmo.h>

#include <scene/scene.h>
#include <renderer/renderer.h>
#include <player/player.h>
#include <ui/uimanager.h>
#include <renderer/lightsmanager.h>
#include <prefab/prefabregistry.h>

class Core
{
public:
    Core();

private:
    EngineContext m_Context;
};