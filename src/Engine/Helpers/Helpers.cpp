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

float3 Engine::RGB8ToRGB32( uint color )
{
	float r = static_cast<float>((color >> 16) & 0xff);
	float g = static_cast<float>((color >> 8) & 0xff);
	float b = static_cast<float>(color & 0xff);
	return float3(r, g, b) / 255.f;
}

Engine::ScopedTimer::~ScopedTimer()
{
	Engine::Logger::Info("{} tooks {}ms.", name, t.elapsed() * 1000);
}
