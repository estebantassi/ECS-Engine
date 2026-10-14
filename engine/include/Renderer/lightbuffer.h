#pragma once

#include <renderer/lighttraits.h>
#include <scene/scene.h>

template<typename T>
class LightBuffer
{
public:
    //Both not necessary but cleaner
    using Traits = LightTraits<T>;
    using ShaderType = typename Traits::ShaderType;

    //probably store the shader and re-use it later
    void Initialize(GLuint shaderProgram)
    {
        glCreateBuffers(1, &m_Buffer);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Traits::Binding, m_Buffer);

        m_CountLocation = glGetUniformLocation(shaderProgram, Traits::CountUniform);
    }

    void Resize(Scene* scene)
    {
        const auto& lights = scene->GetAllComponents<T>();
        const size_t count = lights.size();

        std::vector<ShaderType> convertedData;
        convertedData.reserve(count);

        m_BufferSize = ((count + 10 - 1) / 10) * 10;

        for (size_t i = 0; i < count; ++i)
        {
            EntityID id = scene->GetEntityAtIndexInComponentPool<T>(i);
            Transform transform = scene->GetComponent<Transform>(id);
            convertedData.push_back(Traits::Convert(lights[i], id, transform));
        }

        glNamedBufferData(
            m_Buffer,
            m_BufferSize * sizeof(ShaderType),
            convertedData.data(),
            GL_DYNAMIC_DRAW
        );

        glUniform1i(m_CountLocation, static_cast<GLint>(count));
    }

    void Update(Scene* scene, EntityID id)
    {
        const size_t index = scene->GetIndexOfEntityInComponentPool<T>(id);
        T light = scene->GetComponent<T>(id);

        Transform transform = scene->GetComponent<Transform>(id);

        ShaderType shaderLight = Traits::Convert(light, id, transform);

        glNamedBufferSubData(
            m_Buffer,
            index * sizeof(ShaderType),
            sizeof(ShaderType),
            &shaderLight
        );
    }

    void MarkDirty()
    {
        m_Dirty = true;
    }

    void ResizeIfDirty(Scene* scene)
    {
        if (!m_Dirty) return;

        Resize(scene);
        m_Dirty = false;
    }

    void Destroy()
    {
        glDeleteBuffers(1, &m_Buffer);
        m_Buffer = 0;
    }

private:
    size_t m_BufferSize = 0;
    bool m_Dirty = false;
    GLuint m_Buffer = 0;
    GLint m_CountLocation = -1;
};