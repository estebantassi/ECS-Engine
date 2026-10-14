#pragma once

#include <core/types.h>

#include <unordered_map>
#include <any>
#include <type_traits>
#include <utility>
#include <vector>
#include <iostream>

#include <core/event.h>

using ComponentID = uint32_t;

inline ComponentID GetNewComponentID()
{
	static ComponentID counter = 0;
	return counter++;
}

template<typename T>
ComponentID GetComponentID()
{
	static ComponentID id = GetNewComponentID();
	return id;
}

template<typename T>
struct ComponentInfo
{
	static constexpr uint8_t DrawPriority = 2;
	//typeid(T) is runtime, so we can't use constexpr since it's not builtime.
	static const char* Name()
	{
		return typeid(T).name();
	}
};

template<>
struct ComponentInfo<Transform>
{
	static constexpr uint8_t DrawPriority = 1;
	static constexpr const char* Name() { return "Transform"; };
};

template<>
struct ComponentInfo<Name>
{
	static constexpr uint8_t DrawPriority = 0;
	static constexpr const char* Name() { return "Name"; };
};


class IComponentPool
{
public:
	virtual ~IComponentPool() = default;
	virtual void Remove(EntityID id) = 0;
	virtual bool Has(EntityID id) = 0;

	virtual std::type_index GetType() const = 0;

	virtual uint8_t GetDrawPriority() const = 0;
	virtual const char* GetName() const = 0;
};

//Args&& preserves lvalue/rvalue info. Using std::forward sends the args with && preserved too.
//Using Args& prevents us from sending a value made up inside the arguments. Ex: do(value) works, not do("iamanarg")
//The cast isn't really useful, but if we mess up and assing the wrong type to the map, it's a good practice.
template<typename T>
class ComponentPool : public IComponentPool
{
public:
	uint8_t GetDrawPriority() const override { return ComponentInfo<T>::DrawPriority; }
	const char* GetName() const override { return ComponentInfo<T>::Name(); }

	std::type_index GetType() const override { return typeid(T); }

	template<typename... Args>
	T& Add(EntityID id, Args&&... args)
	{
		size_t index = components.size();
		entityToIndex[id] = index;
		indexToEntity.push_back(id);
		return components.emplace_back(std::forward<Args>(args)...);
	}

	T& Get(EntityID id)
	{
		auto it = entityToIndex.find(id);
		assert(it != entityToIndex.end() && "Entity does not possess component");
		return components[it->second];
	}

	size_t GetIndexOfEntity(EntityID id)
	{
		auto it = entityToIndex.find(id);
		assert(it != entityToIndex.end() && "Entity does not possess component");
		return it->second;
	}

	EntityID GetEntityAtIndex(size_t index)
	{
		//assert(indexToEntity.size() < index && "Index is out of range");
		if (indexToEntity.size() > index) return indexToEntity[index];
		else return INVALID_ID;
	}

	std::vector<T>& GetAll()
	{
		return components;
	}

	T* TryGet(EntityID id)
	{
		auto it = entityToIndex.find(id);
		if (it == entityToIndex.end()) return nullptr;
		return &components[it->second];
	}

	bool Has(EntityID id) override
	{
		auto it = entityToIndex.find(id);
		return it != entityToIndex.end();
	}

	void Remove(EntityID id) override
	{
		auto it = entityToIndex.find(id);
		if (it == entityToIndex.end()) return;

		size_t index = it->second;
		size_t last = components.size() - 1;

		if (index != last)
		{
			components[index] = std::move(components[last]);

			EntityID movedEntity = indexToEntity[last];
			indexToEntity[index] = movedEntity;
			entityToIndex[movedEntity] = index;
		}

		components.pop_back();
		indexToEntity.pop_back();
		entityToIndex.erase(it);
	}

private:
	std::vector<T> components;
	std::vector<EntityID> indexToEntity;
	std::unordered_map<EntityID, size_t> entityToIndex;
};