// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#include "precomp.h"
#include "Application.h"

#include "LineSegment.h"

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Application::Init()
{
	m_camera.SetResolution(int2(screen->width, screen->height));
	m_camera.SetZoomLevel(1.f);


	//grid.AddGrid(0x1d2d44, 1, .9f);
	grid.AddGrid(0x354f52, 25 /*, .7f*/);
	grid.AddGrid(0x52796f, 100 /*, .33f*/);
	//grid.AddGrid(0x748cab, 100, .1f);
	//grid.AddGrid(0xf0ebd8, 1000);
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void Application::Tick(const float deltaTime)
{
	screen->Clear(0x2f3e46);

	/*
	 *	Update logic
	 */

	float2 dir{ 0.f };

	float zoomDir = 1;

	if (m_inputManager.IsKeyDown(GLFW_KEY_W))
		dir.y -= 1.f;
	if (m_inputManager.IsKeyDown(GLFW_KEY_S))
		dir.y += 1.f;
	if (m_inputManager.IsKeyDown(GLFW_KEY_A))
		dir.x -= 1.f;
	if (m_inputManager.IsKeyDown(GLFW_KEY_D))
		dir.x += 1.f;

	if (m_inputManager.IsKeyDown(GLFW_KEY_R))
		zoomDir += .5f * deltaTime;
	if (m_inputManager.IsKeyDown(GLFW_KEY_F))
		zoomDir -= .5f * deltaTime;


	if (sqrLength(dir)>0.f)
		dir = normalize(dir);

	const float2 newPos = m_camera.GetPosition() + dir * 50.f * deltaTime;

	const float newZoom = m_camera.GetZoomLevel() * zoomDir;


	m_camera.SetPosition(newPos);
	m_camera.SetZoomLevel(newZoom);


	printf("cameraPosition: %.3f %.3f\n", newPos.x, newPos.y);
	printf("cameraZoom: %.3f\n", newZoom);

	/*
	 *	Render logic
	 */

	grid.Render(m_camera, *screen);
	


	m_inputManager.Update(deltaTime);
}
