// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#include "precomp.h"
#include "Application.h"

#include "UIManager.h"
#include "Renderables/LineSegment.h"

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Application::Init()
{
	Engine::Logger::Init();
	Engine::UIManager::Init();
	m_world.Init(screen, &m_inputManager);
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void Application::Tick( const float deltaTime )
{
	m_frameTimer.reset();
	screen->Clear(0x2f3e46);

	// Update logic
	m_world.Update(deltaTime);
	m_inputManager.Update(deltaTime);

	//DEBUG BEHAVIOUR
	if (m_inputManager.IsKeyDown(GLFW_KEY_B))
	{
		Engine::UIManager::settings.debugMode = false;
	}
	else if (m_inputManager.IsKeyDown(GLFW_KEY_V)) Engine::UIManager::settings.debugMode = true;
}

void Tmpl8::Application::UI()
{
	if (Engine::UIManager::BeginDebugWindow("Window"))
	{
		ImGui::Text("Frametime: %fms", m_frameTimer.elapsed() * 1000);
	}
	Engine::UIManager::EndDebugWindow();

	m_world.UI();
}
