#pragma once

#include <core/enginecontext.h>
#include <core/types.h>
#include <glad/glad.h>

#include <shaders/shaders.h>

#include <core/engine.h>

#include <entitymanager/entitymanager.h>
#include <scene/scene.h>

#include <vector>

#include <renderer/lightbuffer.h>

struct DirectionalLightLocations {
	GLuint ColorLocation = 0;
	GLuint DirectionLocation = 0;
	GLuint IntensityLocation = 0;
	GLuint ReflectionColorLocation = 0;
};

class LightsManager {
public:
	LightsManager(EngineContext* context);

	void Update();
	void Close();

	void UpdatePointLight(EntityID id)
	{
		m_PointLights.Update(m_Context->scene, id);
	}

	void UpdateSpotLight(EntityID id)
	{
		m_SpotLights.Update(m_Context->scene, id);
	}

	void SetDirectionalLight(DirectionalLight light)
	{
		Shader* shader = m_Context->shaderManager->GetShader(ShaderHandle::Main);
		glUseProgram(shader->ID);

		EntityID id = m_Context->scene->GetEntityAtIndexInComponentPool<DirectionalLight>(0);
		Transform transform = m_Context->scene->GetComponent<Transform>(id);
		glm::vec3 direction = transform.rotation * glm::vec3(0, 0, 1);
		glUniform3f(m_DirectionalLightLocations.DirectionLocation, direction.x, direction.y, direction.z);
		glUniform3f(m_DirectionalLightLocations.ReflectionColorLocation, light.reflectionColor.x, light.reflectionColor.y, light.reflectionColor.z);
		glUniform3f(m_DirectionalLightLocations.ColorLocation, light.color.x, light.color.y, light.color.z);
		glUniform1f(m_DirectionalLightLocations.IntensityLocation, light.intensity);
	}

	bool DirectionalLightExists()
	{
		return m_Context->scene->GetEntityAtIndexInComponentPool<DirectionalLight>(0) != INVALID_ID;
	}

private:
	EngineContext* m_Context = nullptr;

private:
	DirectionalLightLocations m_DirectionalLightLocations = DirectionalLightLocations();
	LightBuffer<PointLight> m_PointLights;
	LightBuffer<SpotLight> m_SpotLights;
};