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
	const Timer t;
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

	constexpr float alpha = .5f;
	m_averageMS = (1.f - alpha) * m_averageMS + alpha * t.elapsed() * 1000.f;
}

void Tmpl8::Application::UI()
{
	if (ImGui::BeginMainMenuBar())
	{
		m_world.ImGuiBar();

		const std::string timingsText = std::format("{:.2f}ms ({:.1f}fps)", m_averageMS, 1.f / m_averageMS * 1000.f);

		const float spacing = ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(timingsText.c_str()).x;
		ImGui::SetCursorPosX(spacing);

		ImGui::Text(timingsText.c_str());
	}

	ImGui::EndMainMenuBar();

	m_world.UI();
}
