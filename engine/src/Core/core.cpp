#include <core/core.h>

Core::Core() {
	m_Context.width = 1000;
	m_Context.height = 500;

	m_Context.scene = new Scene(); //This is probably where i would load a saved scene
	m_Context.prefabRegistry = new PrefabRegistry();
	m_Context.renderer = new Renderer(&m_Context, "Hello World");
	m_Context.lightsManager = new LightsManager(&m_Context);
	m_Context.player = new Player(&m_Context);
	m_Context.uiManager = new UIManager(&m_Context);

	double m_PreviousFrame = 0;
	while (!glfwWindowShouldClose(m_Context.window))
	{	
		//FPS
		m_Context.dt = glfwGetTime() - m_PreviousFrame;
		m_PreviousFrame = glfwGetTime();
		m_Context.fps = m_Context.fps * 0.9f + (1.0f / m_Context.dt) * 0.1f;

		m_Context.uiManager->Update();

		Input::Update(m_Context.window);
		m_Context.player->Update();
		m_Context.lightsManager->Update();
		m_Context.renderer->Update();
	}

	m_Context.uiManager->Close();
	m_Context.player->Close();
	m_Context.lightsManager->Close();
	m_Context.renderer->Close();
}