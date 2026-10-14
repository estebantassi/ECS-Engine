#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <core/input.h>
#include <core/enginecontext.h>
#include <core/types.h>
#include <core/imgui.h>
#include <imguizmo/imguizmo.h>

#include <shaders/shaders.h>

#include <core/engine.h>

class Player {
public:  
	Player(EngineContext* context);
	void Update();
	void KeyboardInput();
	void MouseInput();
	void Close();

private:
	EngineContext* m_Context = nullptr;
	Shader* m_MainShader = nullptr;

private:
	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::ivec3 m_ChunkPosition = glm::ivec3(0, 0, 0);
	glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, -1.0f);

private:
	bool m_FirstMouse = true;
	double m_LastMouseX = 0.0f;
	double m_LastMouseY = 0.0f;
	float m_Yaw = -90.0f;
	float m_Pitch = 0.0f;

public:
	glm::mat4 GetView() { return m_View; }
	glm::mat4 GetProjection() { return m_Projection; }

private:
	glm::mat4 m_View;
	glm::mat4 m_Projection;

private:
	bool m_ReloadShadersKeyPressed = false;
};