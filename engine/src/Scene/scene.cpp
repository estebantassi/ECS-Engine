#include <scene/scene.h>

Scene::Scene() : m_EntityManager(new EntityManager()) {
}

Entity Scene::CreateEntity(std::string name = "")
{
    Entity entity = m_EntityManager->CreateEntity();

    if (name == "") name = m_EntityManager->GetTotalIDs();
    else if (entity.ID() > 1) name += std::to_string(m_EntityManager->GetTotalIDs());

    AddComponent<Name>(entity.ID()).value = name;
    AddComponent<Transform>(entity.ID());
    return entity;
}

void Scene::RemoveEntity(EntityID id)
{
    Hierarchy* h = TryGetComponent<Hierarchy>(id);

    if (h)
    {
        EntityID child = h->firstChild;

        while (child != INVALID_ID)
        {
            EntityID next = GetComponent<Hierarchy>(child).nextSibling;

            RemoveEntity(child);

            child = next;
        }
    }

    for (IComponentPool* pool : m_OrderedPools)
    {
        if (!pool->Has(id))
            continue;

        std::type_index typeID = pool->GetType();

        pool->Remove(id);

        OnComponentRemoved(typeID).Invoke(id);
    }
    m_EntityManager->RemoveEntity(id);
}