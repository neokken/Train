// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#include "precomp.h"
#include "Application.h"

#include "Renderables/LineSegment.h"

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Application::Init()
{
	m_world.Init( screen, &m_inputManager );
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void Application::Tick(const float deltaTime)
{
	screen->Clear(0x2f3e46);

	// Update logic
	m_world.Update( deltaTime );
	m_inputManager.Update(deltaTime);
}
