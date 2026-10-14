#include <renderer/renderer.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

int swidth;
int sheight;
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	swidth = width;
	sheight = height;
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

Renderer::Renderer(EngineContext* context, const char* title)
	: m_Context(context)
{
	if (!glfwInit())
		throw std::runtime_error("GLFW init failed");
	
	m_Context->monitor = glfwGetPrimaryMonitor();
	m_Context->mode = glfwGetVideoMode(m_Context->monitor);

	m_Context->window = glfwCreateWindow(m_Context->width, m_Context->height, title, NULL, NULL);
	if (!m_Context->window)
	{
		glfwTerminate();
		throw std::runtime_error("GLFW create window failed");
	}

	glfwMakeContextCurrent(m_Context->window);

	glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Failed to initialize GLAD");
	}

	glfwSetFramebufferSizeCallback(m_Context->window, framebuffer_size_callback);
	framebuffer_size_callback(m_Context->window, m_Context->width, m_Context->height);

	glfwSetInputMode(m_Context->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(m_Context->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	m_Context->shaderManager = new ShaderManager();
	m_MainShader = m_Context->shaderManager->GetShader(ShaderHandle::Main);
	m_MainShader->use();

	m_Tex = loadTexture(SHADER_DIR "container2.png");
	m_TexSpec = loadTexture(SHADER_DIR "container2_specular.png");

	m_MainShader->use();
	m_MainShader->setMat4("transform", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)));
	//Texture is set to its index. The texture index is one above the texture unit. So if the texture is bound to GL_TEXTURE0, the index is 1.
	m_MainShader->setInt("material.diffuse", m_Tex - 1);
	m_MainShader->setInt("material.specular", m_TexSpec - 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
};

void Renderer::Update() {
	m_Context->scene->CommandBuffer.RunCommands();

	m_Context->width = swidth;
	m_Context->height = sheight;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.68f, 0.85f, 0.90f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Tex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_TexSpec);

	m_MainShader->use();

	m_MainShader->setFloat("material.shininess", 64.0f);

	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f);

	for (size_t i = 0; i < m_Context->scene->GetAllComponents<Mesh>().size(); i++)
	{
		EntityID entity = m_Context->scene->GetEntityAtIndexInComponentPool<Mesh>(i);
		m_MainShader->setMat4("transform", m_Context->scene->GetComponent<Transform>(entity).GetMatrix());
		m_Context->scene->GetComponent<Mesh>(entity).Draw();
	}

	for (const auto& [entity, mask] : m_Context->scene->UpdateTracker.GetUpdates())
	{
		//POINT LIGHT
		if ((m_Context->scene->UpdateTracker.HasUpdate<PointLight>(entity) || m_Context->scene->UpdateTracker.HasUpdate<Transform>(entity))
			&& m_Context->scene->HasComponent<PointLight>(entity)
			)
		{
			m_Context->lightsManager->UpdatePointLight(entity);
		}

		//SPOT LIGHT
		if ((m_Context->scene->UpdateTracker.HasUpdate<SpotLight>(entity) || m_Context->scene->UpdateTracker.HasUpdate<Transform>(entity))
			&& m_Context->scene->HasComponent<SpotLight>(entity)
			)
		{
			m_Context->lightsManager->UpdateSpotLight(entity);
		}

		//DIRECTIONAL LIGHT
		if ((m_Context->scene->UpdateTracker.HasUpdate<Transform>(entity) || m_Context->scene->UpdateTracker.HasUpdate<DirectionalLight>(entity))
			&& m_Context->scene->HasComponent<DirectionalLight>(entity))
		{
			DirectionalLight* light = m_Context->scene->TryGetComponent<DirectionalLight>(entity);
			if (light) m_Context->lightsManager->SetDirectionalLight(*light);
		}
	}

	m_Context->scene->UpdateTracker.ClearUpdates();


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_Context->window);
	glfwPollEvents();
}

void Renderer::Close() {
	glfwTerminate();
}