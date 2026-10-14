#pragma once
#include <core/imgui.h>
#include <imguizmo/imguizmo.h>
#include <core/types.h>


#include <scene/scene.h>


//MOVE ALL OF THIS TO UI SPECIFIC HEADER

template<typename T>
bool DrawProperties(ComponentPool<T>& pool, EntityID id, Scene* scene) {
	return false;
}

template<>
inline bool DrawProperties<PointLight>(ComponentPool<PointLight>& pool, EntityID id, Scene* scene)
{
	if (!pool.Has(id)) return false;
	PointLight& light = pool.Get(id);

	bool edited = false;

	if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_None))
	{
		edited |= ImGui::DragFloat3("Color", &light.color.x, 0.01f, 0.0f, 1.0f);
		edited |= ImGui::DragFloat3("Reflection Color", &light.reflectionColor.x, 0.01f, 0.0f, 1.0f);
		edited |= ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f, FLT_MAX);
		edited |= ImGui::DragFloat("Radius", &light.radius, 0.01f, 0.0f, FLT_MAX);

		edited |= ImGuizmo::IsUsing();

		if (ImGui::Button("X", ImVec2(25.0f, 0)))
			scene->RemoveComponent<PointLight>(id);
	}
		
	return edited;
}

template<>
inline bool DrawProperties<SpotLight>(ComponentPool<SpotLight>& pool, EntityID id, Scene* scene)
{
	if (!pool.Has(id)) return false;
	SpotLight& light = pool.Get(id);

	bool edited = false;

	if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_None))
	{
		edited |= ImGui::DragFloat3("Color", &light.color.x, 0.01f, 0.0f, 1.0f);
		edited |= ImGui::DragFloat3("Reflection Color", &light.reflectionColor.x, 0.01f, 0.0f, 1.0f);
		edited |= ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f, FLT_MAX);
		edited |= ImGui::DragFloat("Radius", &light.radius, 0.01f, 0.0f, FLT_MAX);
		edited |= ImGui::DragFloat("Inner Cut Off", &light.innerCutoff, 0.01f, 0.0f, FLT_MAX);
		edited |= ImGui::DragFloat("Outer Cut Off", &light.outerCutoff, 0.01f, 0.0f, FLT_MAX);

		edited |= ImGuizmo::IsUsing();

		if (ImGui::Button("X", ImVec2(25.0f, 0)))
			scene->RemoveComponent<SpotLight>(id);
	}

	return edited;
}

template<>
inline bool DrawProperties<DirectionalLight>(ComponentPool<DirectionalLight>& pool, EntityID id, Scene* scene)
{
	if (!pool.Has(id)) return false;
	DirectionalLight& light = pool.Get(id);

	bool edited = false;

	if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_None))
	{
		edited |= ImGui::DragFloat3("Color", &light.color.x, 0.01f, 0.0f, 1.0f);
		edited |= ImGui::DragFloat3("Reflection Color", &light.reflectionColor.x, 0.01f, 0.0f, 1.0f);
		edited |= ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f, FLT_MAX);

		edited |= ImGuizmo::IsUsing();

		if (ImGui::Button("X", ImVec2(25.0f, 0)))
			scene->RemoveComponent<DirectionalLight>(id);
	}

	return edited;
}


template<>
inline bool DrawProperties<Transform>(ComponentPool<Transform>& pool, EntityID id, Scene* scene)
{
	if (!pool.Has(id)) return false;
	Transform& transform = pool.Get(id);

	bool edited = false;

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
	{
		edited |= ImGui::DragFloat3("Position", &transform.position.x, 0.01f);

		if (ImGui::DragFloat3("Rotation", &transform.eulerRotation.x, 0.1f))
		{
			edited = true;
			transform.SetEulerRotation(transform.eulerRotation);
		}

		edited |= ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f, 0.01f, FLT_MAX);
	}

	return edited;
}

template<>
inline bool DrawProperties<Name>(ComponentPool<Name>& pool, EntityID id, Scene* scene)
{
	if (!pool.Has(id)) return false;

	Name& name = pool.Get(id);

	char buffer[256];
	strncpy(buffer, name.value.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	
	if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
	{
		name.value = buffer;
		return true;
	}

	return false;
}