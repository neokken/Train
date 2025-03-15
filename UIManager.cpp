#include "precomp.h"
#include "UIManager.h"

void Engine::UIManager::Init()
{
	ImGuiIO IO = ImGui::GetIO();

	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
}

void Engine::UIManager::DrawMainWindow( const uint texture )
{
	static bool open = true;
	ImGui::Begin("Main", &open);
	ImGui::Image(texture, ImVec2(SCRWIDTH, SCRHEIGHT));
}
