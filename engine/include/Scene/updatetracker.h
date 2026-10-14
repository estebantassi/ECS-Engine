#pragma once

#include <core/types.h>
#include <core/components.h>
#include <unordered_map>

class UpdateTracker
{
public:
	template<typename T>
	void MarkUpdate(EntityID id)
	{
		m_UpdateState[id] |= (1ull << GetComponentID<T>());
	}

	template<typename T>
	bool HasUpdate(EntityID id)
	{
		auto it = m_UpdateState.find(id);
		if (it == m_UpdateState.end()) return false;
		return (it->second & (1ull << GetComponentID<T>())) != 0;
	}

	void ClearUpdates() {
		m_UpdateState.clear();
	}

	const std::unordered_map<EntityID, uint64_t>& GetUpdates() const { return m_UpdateState; }

private:
	std::unordered_map<EntityID, uint64_t> m_UpdateState;
};