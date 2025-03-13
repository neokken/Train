// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#pragma once
#include "Camera.h"
#include "Grid.h"
#include "InputManager.h"

namespace Tmpl8
{

class Application final: public TheApp
{
public:
	Application() = default;
	~Application() override = default;

	void Init() override;
	void Tick( float deltaTime ) override;
	void Shutdown() override {}

	// input handling
	void MouseUp(const int button) override { m_inputManager.HandleMouseUp(button); }
	void MouseDown(const int button) override { m_inputManager.HandleMouseDown(button); }
	void MouseMove(const int x, const int y) override { m_inputManager.UpdateMousePosition(int2(x, y)); }
	void MouseWheel(const float scrollDelta) override { m_inputManager.UpdateScrollDelta(scrollDelta); }

	void KeyUp(const int key) override		{ m_inputManager.HandleKeyUp(key); }
	void KeyDown(const int key) override	{ m_inputManager.HandleKeyDown(key); }


private:

	InputManager m_inputManager{};
	Camera m_camera{int2(0,0)};


	Grid grid;
};

} // namespace Tmpl8