#pragma once

namespace Engine
{
	class InputManager;

	class Camera
	{
	public:
		Camera() = delete;

		explicit Camera( const int2& size );

		void SetPosition( const float2& newPos ) { m_position = newPos; }
		void SetResolution( const int2& res ) { m_resolution = res; }
		void SetHorizontalWishSize( const float width ) { m_wishWidthSize = width; }
		void SetZoomLevel( const float zoom ) { m_wishWidthSize = static_cast<float>(m_resolution.x) / zoom; }

		void Update( float deltaTime );

		[[nodiscard]] const float2& GetPosition() const { return m_position; }

		[[nodiscard]] float2 GetTopLeft() const;
		[[nodiscard]] float2 GetBottomRight() const;
		[[nodiscard]] float GetZoomLevel() const;

		[[nodiscard]] float2 GetCameraPosition( const float2& worldPosition ) const;
		[[nodiscard]] float2 GetWorldPosition( const float2& localPosition ) const;

	private:
		float2 m_position{0.f, 0.f};

		int2 m_resolution;
		float m_wishWidthSize;

		float m_scrollSensitivity = 10.0f;
		float m_scrollMouseImpact = 2.f;
		float m_moveSpeed = 150.0f;

		float2 m_lockedWorldMousePos{};
		bool m_mouseLocked{false};
	};
}
