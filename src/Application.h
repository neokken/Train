// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#pragma once
#include "Input/InputManager.h"

#include "World/World.h"

namespace Tmpl8
{
	class Application final : public TheApp
	{
	public:
		Application() = default;
		~Application() override = default;

		void Init() override;
		void Tick( float deltaTime ) override;

		void Shutdown() override
		{
		}

		void UI( float deltaTime ) override;

		// input handling
		void MouseUp( const int button ) override { Input::get().HandleMouseUp(button); }
		void MouseDown( const int button ) override { Input::get().HandleMouseDown(button); }
		void MouseMove( const int x, const int y ) override { Input::get().UpdateMousePosition(int2(x, y)); }
		void MouseWheel( const float scrollDelta ) override { Input::get().UpdateScrollDelta(scrollDelta); }

		void KeyUp( const int key ) override { Input::get().HandleKeyUp(key); }
		void KeyDown( const int key ) override { Input::get().HandleKeyDown(key); }

	private:
		Engine::World m_world;

		Timer m_frameTimer;
	};
} // namespace Tmpl8
