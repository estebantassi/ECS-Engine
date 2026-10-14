#pragma once

#include <core/imgui.h>
#include <core/enginecontext.h>
#include <shaders/shaders.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <core/engine.h>

#include <scene/scene.h>
#include <renderer/lightsmanager.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>

#include <core/event.h>

class Renderer {
	public:
		Renderer(EngineContext* context, const char* title);
		void Update();
		void Close();

	private:
		EngineContext* m_Context = nullptr;
	
	private:
		Shader* m_MainShader = nullptr;

		GLuint m_Tex = 0;
		GLuint m_TexSpec = 0;
	private:
};