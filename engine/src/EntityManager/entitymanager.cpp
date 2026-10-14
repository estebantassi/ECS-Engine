#include <entitymanager/entitymanager.h>

Entity EntityManager::CreateEntity() {
	EntityID id;
	m_TotalIDs++;
	if (!m_FreeIDs.empty())
	{
		id = m_FreeIDs.front();
		m_FreeIDs.erase(m_FreeIDs.begin());
	}
	else
	{
		id = m_NextID;
		m_NextID++;
	}

	Entity entity = Entity(id);
	m_Entities.push_back(entity.ID());
	return entity;
}

void EntityManager::RemoveEntity(EntityID id) {
	m_FreeIDs.push_back(id);
	std::erase(m_Entities, id);
}