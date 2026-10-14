#include <player/player.h>

Player::Player(EngineContext* context) : m_Context(context) {
	m_MainShader = m_Context->shaderManager->GetShader(ShaderHandle::Main);

    glfwSetInputMode(m_Context->window, GLFW_CURSOR, Input::GetMouseState() == MouseState::GAME ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Player::Update() {
    Player::MouseInput();
	Player::KeyboardInput();

	m_ChunkPosition = glm::ivec3(
		static_cast<int>(floor(m_Position.x / 32.0f)),
		static_cast<int>(floor(m_Position.y / 32.0f)),
		static_cast<int>(floor(m_Position.z / 32.0f))
	);

    m_View = glm::lookAt(m_Position, m_Position + m_Forward, glm::vec3(0.0f, 1.0f, 0.0f));
    m_Projection = glm::perspective(glm::radians(70.0f), (float)m_Context->width / (float)m_Context->height, 0.1f, 9999.9f);

    m_MainShader->use();
    m_MainShader->setMat4("view", m_View);
    m_MainShader->setMat4("projection", m_Projection);
    m_MainShader->setVec3("camPos", m_Position);
}

void Player::MouseInput() {
    if (Input::GetMouseState() == MouseState::BOTH || Input::GetMouseState() == MouseState::UI) return;

    double mouseX, mouseY;
    glfwGetCursorPos(m_Context->window, &mouseX, &mouseY);

    if (m_FirstMouse) {
        m_LastMouseX = mouseX;
        m_LastMouseY = mouseY;
        m_FirstMouse = false;
    }

    float deltaX = static_cast<float>(mouseX) - static_cast<float>(m_LastMouseX);
    float deltaY = static_cast<float>(m_LastMouseY) - static_cast<float>(mouseY);
    m_LastMouseX = mouseX;
    m_LastMouseY = mouseY;

    deltaX *= 0.1f;
    deltaY *= 0.1f;

    m_Yaw += deltaX;
    m_Pitch += deltaY;

    if (m_Pitch > 89.0f)
        m_Pitch = 89.0f;
    if (m_Pitch < -89.0f)
        m_Pitch = -89.0f;

    glm::vec3 front = glm::vec3(
        cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch)),
        sin(glm::radians(m_Pitch)),
        sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch))
    );

    m_Forward = glm::normalize(front);
}

void Player::KeyboardInput() {
    float velocity = 5.0f * m_Context->dt;
    glm::vec3 direction = glm::normalize(glm::vec3(m_Forward.x, 0.0f, m_Forward.z));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    if (Input::GetMouseState() == MouseState::GAME || Input::GetMouseState() == MouseState::BOTH)
    {
        if (Input::KeyDown(GLFW_KEY_W))
            m_Position += velocity * direction;
        if (Input::KeyDown(GLFW_KEY_S))
            m_Position -= velocity * direction;
        if (Input::KeyDown(GLFW_KEY_A))
            m_Position -= velocity * glm::normalize(glm::cross(m_Forward, up));
        if (Input::KeyDown(GLFW_KEY_D))
            m_Position += velocity * glm::normalize(glm::cross(m_Forward, up));
        if (Input::KeyDown(GLFW_KEY_Q))
            m_Position += velocity * up;
        if (Input::KeyDown(GLFW_KEY_E))
            m_Position -= velocity * up;
    }

    if (Input::KeyPressed(GLFW_KEY_LEFT_ALT))
    {
		if (Input::GetMouseState() == MouseState::GAME)
		{
            Input::SetMouseState(MouseState::BOTH);
			glfwSetInputMode(m_Context->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
            Input::SetMouseState(MouseState::GAME);
			glfwSetInputMode(m_Context->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            glfwSetCursorPos(m_Context->window, m_Context->width / 2.0, m_Context->height / 2.0);
            m_FirstMouse = true;
		}
    }

    if (glfwGetKey(m_Context->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_Context->window, true);

    if (glfwGetKey(m_Context->window, GLFW_KEY_R) == GLFW_PRESS && !m_ReloadShadersKeyPressed)
    {
        m_ReloadShadersKeyPressed = true;
        //m_MainShader->Reload();
        std::cout << "Reloaded shaders" << std::endl;
    }
    if (glfwGetKey(m_Context->window, GLFW_KEY_R) == GLFW_RELEASE)
        m_ReloadShadersKeyPressed = false;
}

void Player::Close()
{
    
}