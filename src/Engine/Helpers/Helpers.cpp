#include "precomp.h"
#include "Helpers.h"

#include "imgui_internal.h"

bool Engine::IsMouseOverUI()
{
	ImGuiContext& g = *GImGui;
	if (g.HoveredWindow)
	{
		const bool isMain = strcmp(g.HoveredWindow->Name, "Main") == 0;
		return !isMain;
	}

	return false;
}

Engine::ScopedTimer::~ScopedTimer()
{
	Engine::Logger::Info("{} tooks {}ms.", name, t.elapsed() * 1000);
}
