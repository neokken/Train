#include "precomp.h"
#include "InputManager.h"

#include "UIManager.h"

void Engine::InputManager::UpdateMousePosition( const int2& mousePos )
{
	const int2 actualMousePos = Engine::UIManager::GetMainWindowCursorPos(mousePos);
	m_mouseDelta = m_mousePosition - actualMousePos;
	m_mousePosition = actualMousePos;
}

void Engine::InputManager::Update( float )
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
