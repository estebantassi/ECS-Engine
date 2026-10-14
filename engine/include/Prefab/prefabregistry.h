#pragma once

#include <string>
#include <vector>
#include <functional>
#include <core/types.h>
#include <scene/scene.h>
#include <unordered_map>
#include <core/shapes.h>

#include <core/enginecontext.h>
#include <renderer/lightsmanager.h>

struct PrefabDefinition
{
    //Name is used for display
    std::string name;
    //Instantiate is the function we use to actual create the prefab
    std::function<EntityID(EngineContext*)> instantiate;
};

class PrefabRegistry
{
public:
    PrefabRegistry();
    const std::vector<PrefabDefinition>& GetPrefabs() const { return m_Prefabs; }

private:
    void Register(const std::string& name, std::function<EntityID(EngineContext*)> creator) {
        m_Prefabs.push_back({ name, creator });
    }
    
    std::vector<PrefabDefinition> m_Prefabs;
};