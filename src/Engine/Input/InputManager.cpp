#include "precomp.h"
#include "InputManager.h"

#include "UI/UIManager.h"

void Engine::InputManager::UpdateMousePosition( const int2& mousePos )
{
	const int2 actualMousePos = Engine::UIManager::GetMainWindowCursorPos(mousePos);
	m_mouseDelta = m_mousePosition - actualMousePos;
	m_mousePosition = actualMousePos;
}

void Engine::InputManager::Update( const float deltaTime )
{
	for (auto& key : m_keys)
	{
		if (key == KeyState::Just_Down)
		{
			key = KeyState::Down;
		}
		else if (key == KeyState::Just_Up)
		{
			key = KeyState::Up;
		}
	}

	for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
	{
		if (m_mouseButtons[i] == KeyState::Just_Down || m_mouseButtons[i] == KeyState::Down)
		{
			m_mouseButtonDownTime[i] += deltaTime;
		}
		else if (m_mouseButtons[i] == KeyState::Up)
		{
			m_mouseButtonDownTime[i] = 0.f;
		}
	}

	for (auto& button : m_mouseButtons)
	{
		if (button == KeyState::Just_Down)
		{
			button = KeyState::Down;
		}
		else if (button == KeyState::Just_Up)
		{
			button = KeyState::Up;
		}
	}

	m_mouseDelta = int2(0);
	m_scrollDelta = 0.f;
}

bool Engine::InputManager::IsKeyDown( const int key ) const
{
	return m_keys[key] == KeyState::Just_Down || m_keys[key] == KeyState::Down;
}

bool Engine::InputManager::IsKeyUp( const int key ) const
{
	return m_keys[key] == KeyState::Just_Up || m_keys[key] == KeyState::Up;
}

bool Engine::InputManager::IsKeyJustDown( const int key ) const
{
	return m_keys[key] == KeyState::Just_Down;
}

bool Engine::InputManager::IsKeyJustUp( const int key ) const
{
	return m_keys[key] == KeyState::Just_Up;
}

bool Engine::InputManager::IsMouseDown( const int button ) const
{
	return m_mouseButtons[button] == KeyState::Just_Down || m_mouseButtons[button] == KeyState::Down;
}

bool Engine::InputManager::IsMouseUp( const int button ) const
{
	return m_mouseButtons[button] == KeyState::Just_Up || m_mouseButtons[button] == KeyState::Up;
}

bool Engine::InputManager::IsMouseJustDown( const int button ) const
{
	return m_mouseButtons[button] == KeyState::Just_Down;
}

bool Engine::InputManager::IsMouseJustUp( const int button ) const
{
	return m_mouseButtons[button] == KeyState::Just_Up;
}

bool Engine::InputManager::IsMouseClicked( const int button, const float clickThreshold ) const
{
	if (!IsMouseJustUp(button)) return false;

	return m_mouseButtonDownTime[button] < clickThreshold;
}
