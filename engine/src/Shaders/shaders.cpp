#include <shaders/shaders.h>

ShaderManager::ShaderManager() {
	m_Shaders.emplace(ShaderHandle::Main, new Shader(SHADER_DIR "main.vert", SHADER_DIR "main.frag"));
}

Shader* ShaderManager::GetShader(ShaderHandle handle)
{
	auto it = m_Shaders.find(handle);
	if (it != m_Shaders.end()) {
		return it->second;
	}
	else {
		throw std::runtime_error("Shader not found for handle");
	}
}