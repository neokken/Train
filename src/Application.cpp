// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#include "precomp.h"
#include "Application.h"

#include "UI/UIManager.h"

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Application::Init()
{
	Engine::Logger::Init();
	Engine::UIManager::Init();
	m_world.Init(screen);
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void Application::Tick( const float deltaTime )
{
	m_frameTimer.reset();
	screen->Clear(GetColor(Color::BackGround));

	// Update logic
	m_world.Update(deltaTime);

	//DEBUG BEHAVIOUR
	if (Input::get().IsKeyJustDown(GLFW_KEY_F1))
	{
		Engine::UIManagerSettings settings = Engine::UIManager::GetSettings();
		settings.debugMode = !settings.debugMode;
		Engine::UIManager::SetSettings(settings);
	}

	Input::get().Update(deltaTime);
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
