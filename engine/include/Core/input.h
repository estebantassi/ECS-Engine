#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <core/types.h>

class Input
{
public:
    static void Update(GLFWwindow* window);

    static bool KeyDown(int key);
    static bool KeyPressed(int key);
    static bool KeyReleased(int key);
    static MouseState GetMouseState();
    static void SetMouseState(MouseState state);

private:
    static bool m_CurrentKeys[GLFW_KEY_LAST + 1];
    static bool m_PreviousKeys[GLFW_KEY_LAST + 1];
    static MouseState m_MouseState;
};