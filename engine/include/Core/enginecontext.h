#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class ShaderManager;
class Player;
class Renderer;
class UIManager;
class LightsManager;
class Scene;
class PrefabRegistry;

struct EngineContext
{
    GLFWmonitor* monitor;
    const GLFWvidmode* mode;
    GLFWwindow* window;
    float dt;
    int width;
    int height;
    int fps;

    Scene* scene;

	PrefabRegistry* prefabRegistry;
	LightsManager* lightsManager;
	UIManager* uiManager;
    Player* player;
    Renderer* renderer;
    ShaderManager* shaderManager;
};