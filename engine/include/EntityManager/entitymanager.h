#pragma once

#include <core/engine.h>

#include <vector>
#include <string>

class EntityManager
{
public:
	Entity CreateEntity();
	void RemoveEntity(EntityID id);
	const std::vector<EntityID>& Entities() const { return m_Entities; };
	const EntityID GetTotalIDs() { return m_TotalIDs; }

private:
	std::vector<EntityID> m_Entities;
	EntityID m_NextID = 1;
	EntityID m_TotalIDs = 0;
	std::vector<EntityID> m_FreeIDs;
};