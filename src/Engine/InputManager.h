#pragma once
namespace Engine {
	enum class KeyState : std::uint8_t
	{
		Up,
		Just_Up,
		Just_Down,
		Down
	};

	class InputManager
	{
	public:
		InputManager() = default;

		void UpdateMousePosition( const int2& mousePos );

		void UpdateScrollDelta( const float scrollDelta ) { m_scrollDelta = scrollDelta; }

		void HandleKeyDown( const int key ) { m_keys[key] = KeyState::Just_Down; }
		void HandleKeyUp( const int key ) { m_keys[key] = KeyState::Just_Up; }

		void HandleMouseDown( const int button ) { m_mouseButtons[button] = KeyState::Just_Down; }
		void HandleMouseUp( const int button ) { m_mouseButtons[button] = KeyState::Just_Up; }

		void Update( float deltaTime );

		[[nodiscard]] bool IsKeyDown( int key ) const;
		[[nodiscard]] bool IsKeyUp( int key ) const;
		[[nodiscard]] bool IsKeyJustDown( int key ) const;
		[[nodiscard]] bool IsKeyJustUp( int key ) const;

		[[nodiscard]] bool IsMouseDown( const int button ) const { return m_mouseButtons[button] == KeyState::Just_Down || m_mouseButtons[button] == KeyState::Down; }
		[[nodiscard]] bool IsMouseUp( const int button ) const { return m_mouseButtons[button] == KeyState::Just_Up || m_mouseButtons[button] == KeyState::Up; }
		[[nodiscard]] bool IsMouseJustDown( const int button ) const { return m_mouseButtons[button] == KeyState::Just_Down; }
		[[nodiscard]] bool IsMouseJustUp( const int button ) const { return m_mouseButtons[button] == KeyState::Just_Up; }

		[[nodiscard]] int2 GetMousePos() const { return m_mousePosition; }
		[[nodiscard]] int2 GetMouseDelta() const { return m_mouseDelta; }
		[[nodiscard]] float GetScrollDelta() const { return m_scrollDelta; }

	private:
		int2 m_mousePosition{ 0 };
		int2 m_mouseDelta{ 0 };

		float m_scrollDelta{ 0.f };

		KeyState m_keys[GLFW_KEY_LAST]{ KeyState::Up };
		KeyState m_mouseButtons[GLFW_MOUSE_BUTTON_LAST]{ KeyState::Up };
	};
}