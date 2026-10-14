#include <prefab/prefabregistry.h>

PrefabRegistry::PrefabRegistry() {
    Register(
        "Cube",
        [](EngineContext* context)
        {
            EntityID e = context->scene->CreateEntity("Cube").ID();
            context->scene->AddComponent<Mesh>(e, Cube::vertices, Cube::indices);
            return e;
        }
    );

    Register(
        "Ico Sphere",
        [](EngineContext* context)
        {
            EntityID e = context->scene->CreateEntity("Ico Sphere").ID();
            context->scene->AddComponent<Mesh>(e, IcoSphere::vertices, IcoSphere::indices);
            return e;
        }
    );

    Register(
        "Directional Light",
        [](EngineContext* context)
        {
            if (context->lightsManager->DirectionalLightExists()) return context->scene->GetEntityAtIndexInComponentPool<DirectionalLight>(0);
            EntityID e = context->scene->CreateEntity("Directional Light").ID();
            DirectionalLight directionalLight = context->scene->AddComponent<DirectionalLight>(e);
            context->lightsManager->SetDirectionalLight(directionalLight);
            return e;
        }
    );

    Register(
        "Point Light",
        [](EngineContext* context)
        {
            EntityID e = context->scene->CreateEntity("Point Light").ID();
            context->scene->AddComponent<PointLight>(e);
            return e;
        }
    );

    Register(
        "Spot Light",
        [](EngineContext* context)
        {
            EntityID e = context->scene->CreateEntity("Spot Light").ID();
            context->scene->AddComponent<SpotLight>(e);
            return e;
        }
    );
}