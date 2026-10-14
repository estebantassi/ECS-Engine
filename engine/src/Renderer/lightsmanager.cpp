#include <renderer/lightsmanager.h>

LightsManager::LightsManager(EngineContext* context) : m_Context(context) {
	Shader* shader = m_Context->shaderManager->GetShader(ShaderHandle::Main);
	glUseProgram(shader->ID);

	m_PointLights.Initialize(shader->ID);
	m_SpotLights.Initialize(shader->ID);

	m_DirectionalLightLocations.ColorLocation = glGetUniformLocation(shader->ID, "directionalLight.color");
	m_DirectionalLightLocations.DirectionLocation = glGetUniformLocation(shader->ID, "directionalLight.direction");
	m_DirectionalLightLocations.IntensityLocation = glGetUniformLocation(shader->ID, "directionalLight.intensity");
	m_DirectionalLightLocations.ReflectionColorLocation = glGetUniformLocation(shader->ID, "directionalLight.reflectionColor");

	//Create a directional light
	//Probably move this to an initializer
	Entity directionalLightEntity = m_Context->scene->CreateEntity("DirectionalLight");
	DirectionalLight& directionalLight = m_Context->scene->AddComponent<DirectionalLight>(directionalLightEntity.ID());
	Transform& directionalLightTransform = m_Context->scene->GetComponent<Transform>(directionalLightEntity.ID());
	directionalLightTransform.SetEulerRotation(glm::vec3(65.0f, 70.0f, 0.0f));
	SetDirectionalLight(directionalLight);

	//A lambda function is written in c++ like so:
	// [values] (args) { code }
	//Here we send [this] to give access to the class' private variables
	size_t onComponentAdded = m_Context->scene->OnComponentAdded(typeid(PointLight)).Subscribe(
		[this](EntityID id)
		{
			m_PointLights.MarkDirty();
		}
	);

	//We can later use the onComponentAdded variable to unsubscribe, instead of creating dangling subscribes if destroying the parent.
	m_Context->scene->OnComponentRemoved(typeid(PointLight)).Subscribe(
		[this](EntityID id)
		{
			m_PointLights.MarkDirty();
		}
	);

	m_Context->scene->OnComponentAdded(typeid(SpotLight)).Subscribe(
		[this](EntityID id)
		{
			m_SpotLights.MarkDirty();
		}
	);

	m_Context->scene->OnComponentRemoved(typeid(SpotLight)).Subscribe(
		[this](EntityID id)
		{
			m_SpotLights.MarkDirty();
		}
	);

	m_Context->scene->OnComponentRemoved(typeid(DirectionalLight)).Subscribe(
		[this](EntityID id)
		{
			Shader* shader = m_Context->shaderManager->GetShader(ShaderHandle::Main);
			glUseProgram(shader->ID);

			glUniform3f(m_DirectionalLightLocations.DirectionLocation, 0.0f, 0.0f, 0.0f);
			glUniform3f(m_DirectionalLightLocations.ReflectionColorLocation, 0.0f, 0.0f, 0.0f);
			glUniform3f(m_DirectionalLightLocations.ColorLocation, 0.0f, 0.0f, 0.0f);
			glUniform1f(m_DirectionalLightLocations.IntensityLocation, 0.0f);
		}
	);
}

void LightsManager::Update() {
	Shader* shader = m_Context->shaderManager->GetShader(ShaderHandle::Main);
	glUseProgram(shader->ID);

	m_SpotLights.ResizeIfDirty(m_Context->scene);
	m_PointLights.ResizeIfDirty(m_Context->scene);
}

void LightsManager::Close() {}