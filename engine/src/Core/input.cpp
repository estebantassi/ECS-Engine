#include <core/input.h>

bool Input::m_CurrentKeys[GLFW_KEY_LAST + 1]{};
bool Input::m_PreviousKeys[GLFW_KEY_LAST + 1]{};
MouseState Input::m_MouseState{ MouseState::GAME };

void Input::Update(GLFWwindow* window)
{
    for (int i = 0; i <= GLFW_KEY_LAST; i++)
    {
        m_PreviousKeys[i] = m_CurrentKeys[i];

        m_CurrentKeys[i] = glfwGetKey(window, i) == GLFW_PRESS;
    }
}

bool Input::KeyDown(int key)
{
    return m_CurrentKeys[key];
}

bool Input::KeyPressed(int key)
{
    return m_CurrentKeys[key] && !m_PreviousKeys[key];
}

bool Input::KeyReleased(int key)
{
    return !m_CurrentKeys[key] && m_PreviousKeys[key];
}

MouseState Input::GetMouseState()
{
    return m_MouseState;
}

void Input::SetMouseState(MouseState state)
{
    m_MouseState = state;
}