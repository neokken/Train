#include "precomp.h"
#include "UIManager.h"

#include <imgui_internal.h>

void Engine::UIManager::Init()
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Engine::UIManager::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (settings.debugMode)
		ImGui::DockSpaceOverViewport(1, 0);
	else
		ImGui::DockSpaceOverViewport(1, 0, ImGuiDockNodeFlags_PassthruCentralNode);

}

void Engine::UIManager::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Engine::UIManager::DrawMainWindow( const uint texture )
{
	static bool open = true;
	ImGui::SetNextWindowSize(ImVec2(SCRWIDTH, SCRHEIGHT), ImGuiCond_Appearing);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	if (!settings.debugMode)
	{
		flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGui::SetNextWindowSize(ImVec2(SCRWIDTH, SCRHEIGHT), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	}
	else
	{
		ImGui::SetNextWindowDockID(1, ImGuiCond_FirstUseEver);
	}

	ImGui::Begin("Main", &open, flags);
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImVec2 imageSize;
	if ((ImGui::GetWindowHeight()) * ASPECT_RATIO < ImGui::GetWindowWidth())
	{
		imageSize = ImVec2((ImGui::GetWindowSize().y) * ASPECT_RATIO, (ImGui::GetWindowSize().y));
	}
	else
	{
		imageSize = ImVec2((ImGui::GetWindowSize().x), (ImGui::GetWindowSize().x) * 1 / ASPECT_RATIO);
	}
	ImGui::Image(texture, imageSize);
	ImVec2 windowPos = ImGui::GetWindowPos();
	m_mainWindowPos = int2(windowPos.x, windowPos.y);
	ImVec2 windowSize = ImGui::GetWindowSize();
	m_mainWindowSize = int2(imageSize.x, imageSize.y);
	ImGui::End();
}

int2 Engine::UIManager::GetMainWindowCursorPos( const int2& mousePos )
{
	float2 cursorPos = (mousePos - m_mainWindowPos) / (m_mainWindowSize / float2(SCRWIDTH, SCRHEIGHT));
	return int2(cursorPos.x, cursorPos.y);
}

bool Engine::UIManager::BeginGameplayWindow( const char* name, ImGuiWindowFlags flags )
{
	static bool open = true;
	return ImGui::Begin(name, &open, flags);
}

bool Engine::UIManager::BeginDebugWindow( const char* name, ImGuiWindowFlags flags )
{
	if (!settings.debugMode) return false;
	static bool open = true;
	return ImGui::Begin(name, &open, flags);
}

void Engine::UIManager::EndDebugWindow()
{
	if (!settings.debugMode) return;
	ImGui::End();
}

Engine::UIManagerSettings Engine::UIManager::settings;
int2 Engine::UIManager::m_mainWindowPos;
int2 Engine::UIManager::m_mainWindowSize;
