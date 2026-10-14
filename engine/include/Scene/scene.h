#pragma once

#include <unordered_map>
#include <typeindex>
#include <core/components.h>
#include <memory>

#include <vector>

#include <scene/updatetracker.h>

#include <core/event.h>
#include <utility>

#include <core/engine.h>

#include <entitymanager/entitymanager.h>
#include <functional>

#include <scene/commandbuffer.h>

class Scene
{
public:
    Scene();
    Entity CreateEntity(std::string name);
    void RemoveEntity(EntityID id);

    UpdateTracker UpdateTracker;
    CommandBuffer CommandBuffer;

    const std::vector<EntityID>& GetEntities() const { return m_EntityManager->Entities(); }
private:
    EntityManager* m_EntityManager;

public:

    Event<EntityID>& OnComponentAdded(std::type_index id) { return m_ComponentAddedEvents[id]; }
    Event<EntityID>& OnComponentRemoved(std::type_index id) {
        //Try emplace doesn't create the value if it doesnt exist, unlike []
        //It returns an iterator, same as find(), so we grab the second, which is the Event
        return m_ComponentRemovedEvents.try_emplace(id).first->second; }

private:
    std::unordered_map<std::type_index, Event<EntityID>> m_ComponentRemovedEvents;
    std::unordered_map<std::type_index, Event<EntityID>> m_ComponentAddedEvents;

public:
//    __   ___   ___ ___  ____   ___   ____     ___  ____   ______      __    __  ____    ____  ____  ____   ___  ____  
//   /  ] /   \ |   |   ||    \ /   \ |    \   /  _]|    \ |      |    |  |__|  ||    \  /    ||    \|    \ /  _]|    \ 
//  /  / |     || _   _ ||  o  )     ||  _  | /  [_ |  _  ||      |    |  |  |  ||  D  )|  o  ||  o  )  o  )  [_ |  D  )
// /  /  |  O  ||  \_/  ||   _/|  O  ||  |  ||    _]|  |  ||_|  |_|    |  |  |  ||    / |     ||   _/|   _/    _]|    / 
///   \_ |     ||   |   ||  |  |     ||  |  ||   [_ |  |  |  |  |      |  `  '  ||    \ |  _  ||  |  |  | |   [_ |    \ 
//\     ||     ||   |   ||  |  |     ||  |  ||     ||  |  |  |  |       \      / |  .  \|  |  ||  |  |  | |     ||  .  \
// \____| \___/ |___|___||__|   \___/ |__|__||_____||__|__|  |__|        \_/\_/  |__|\_||__|__||__|  |__| |_____||__|\_|
//
    template<typename T, typename... Args>
    T& AddComponent(EntityID id, Args&&... args)
    {
        T& component = GetPool<T>().Add(id, std::forward<Args>(args)...);

        OnComponentAdded(typeid(T)).Invoke(id);

        return component;
    }

    template<typename T>
    void RemoveComponent(EntityID id)
    {
        GetPool<T>().Remove(id);
        OnComponentRemoved(typeid(T)).Invoke(id);
    }

    template<typename T>
    T& GetComponent(EntityID id)
    {
        return GetPool<T>().Get(id);
    }

    template<typename T>
    T* TryGetComponent(EntityID id)
    {
        return GetPool<T>().TryGet(id);
    }

    template<typename T>
    bool HasComponent(EntityID id)
    {
        return GetPool<T>().Has(id);
    }

    template<typename T>
    std::vector<T>& GetAllComponents()
    {
        return GetPool<T>().GetAll();
    }

    template<typename T>
    size_t GetIndexOfEntityInComponentPool(EntityID id)
    {
        return GetPool<T>().GetIndexOfEntity(id);
    }

    template<typename T>
    EntityID GetEntityAtIndexInComponentPool(size_t index)
    {
        return GetPool<T>().GetEntityAtIndex(index);
    }

    //Const makes it readonly, & gives a reference and not a copy.
    //The first const states that the data won't change, the second const states that the pointer won't change.
    const std::vector<IComponentPool*>& GetComponentPools() const { return m_OrderedPools; }

private:

    ////////////////////////
    // Get Component Pool //
    ////////////////////////
    template<typename T>
    ComponentPool<T>& GetPool()
    {
        auto it = m_Pools.find(typeid(T));

        if (it == m_Pools.end())
        {
            //The unique_ptr is created here, so the memory management is owned by this variable.
            std::unique_ptr<ComponentPool<T>> pool = std::make_unique<ComponentPool<T>>();
            //get is used to get a pointer to the unique_ptr, using delete on it might break when the unique_ptr later deletes it.
            //The reason we use this ptr is because pool becomes nullptr when moved, so we need a pointer to return/give the vector of components.
            ComponentPool<T>* ptr = pool.get();

            //Using move, we move the ownership to the map's unique_ptr, pool becomes nullptr
            m_Pools.emplace(typeid(T), std::move(pool));
            auto insertPos = std::lower_bound(
                m_OrderedPools.begin(),
                m_OrderedPools.end(),
                ptr,
                [](const IComponentPool* a, const IComponentPool* b)
                {
                    return a->GetDrawPriority() < b->GetDrawPriority();
                });

            m_OrderedPools.insert(insertPos, ptr);

            //ptr is a pointer, we de-reference it using *, to return a reference as requested per the function type.
            return *ptr;
        }

        //we cast the retrieved unique_ptr to a regular pointer, then de-reference it to match the function type.
        return *static_cast<ComponentPool<T>*>(it->second.get());
    }

    //Store only pointer and not unique_ptr since the memory's managed by the unordered map directly.
    std::vector<IComponentPool*> m_OrderedPools;

    //Type index gives an id per type in the engine.
    //a unique_ptr is created using make_unique. Its advantage is that instead of needing to use
    //delete for every single item in the map, destroying the map frees the memory of all its components.
    //unique_ptr owns the memory
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_Pools;
};