#pragma once

#include <core/types.h>

#include <utility>


class Entity
{
public:
	const EntityID ID() { return m_ID; };
	Entity(EntityID id) : m_ID(id) {}

private:
	EntityID m_ID = INVALID_ID;
};