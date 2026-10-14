#pragma once

#include <core/types.h>

template<typename T>
struct LightTraits;

template<>
struct LightTraits<PointLight>
{
    //Important to access the type outside of the struct when it's unknown
    using ShaderType = PointLightShader;

    static constexpr GLuint Binding = 0;
    static constexpr const char* CountUniform = "pointLightCount";

    static PointLightShader Convert(const PointLight& light, EntityID id, const Transform& transform)
    {
        return ConvertPointLight(light, id, transform);
    }
};

template<>
struct LightTraits<SpotLight>
{
    using ShaderType = SpotLightShader;

    static constexpr GLuint Binding = 1;
    static constexpr const char* CountUniform = "spotLightCount";

    static SpotLightShader Convert(const SpotLight& light, EntityID id, const Transform& transform)
    {
        return ConvertSpotLight(light, id, transform);
    }
};