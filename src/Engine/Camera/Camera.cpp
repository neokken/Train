#include "precomp.h"
#include "Camera.h"

#include "InputManager.h"

void Engine::Camera::Init( Engine::InputManager* input )
{
	m_inputManager = input;
}

void Engine::Camera::Update( float deltaTime )
{
	float2 dir = { 0.f };
	float zoomDir = 1;


	if (m_inputManager->IsKeyDown( GLFW_KEY_W ))
		dir.y -= 1.f;
	if (m_inputManager->IsKeyDown( GLFW_KEY_S ))
		dir.y += 1.f;
	if (m_inputManager->IsKeyDown( GLFW_KEY_A ))
		dir.x -= 1.f;
	if (m_inputManager->IsKeyDown( GLFW_KEY_D ))
		dir.x += 1.f;

	zoomDir += m_inputManager->GetScrollDelta() * deltaTime * m_scrollSensitivity;

	if (sqrLength( dir ) > 0.f)
		dir = normalize( dir );

	float evaluatedMoveSpeed = m_moveSpeed;
	if (m_inputManager->IsKeyDown( GLFW_KEY_LEFT_SHIFT )) evaluatedMoveSpeed *= 2.0f;

	SetPosition( GetPosition() + dir * evaluatedMoveSpeed * deltaTime );
	SetZoomLevel( GetZoomLevel() * zoomDir );
}

float2 Engine::Camera::GetTopLeft() const
{
	const float2 resFloat = make_float2( m_resolution );
	const float zoomAmount = resFloat.x / m_wishWidthSize;

	return  m_position - (resFloat * 0.5f) / zoomAmount;
}

float2 Engine::Camera::GetBottomRight() const
{
	const float2 resFloat = make_float2( m_resolution );
	const float zoomAmount = resFloat.x / m_wishWidthSize;

	return m_position + (resFloat * 0.5f) / zoomAmount;
}

float Engine::Camera::GetZoomLevel() const
{
	return static_cast<float>(m_resolution.x) / m_wishWidthSize;
}

float2 Engine::Camera::GetCameraPosition( const float2& worldPosition ) const
{
	return (worldPosition - GetTopLeft()) * GetZoomLevel();
}

float2 Engine::Camera::GetWorldPosition( const float2& localPosition ) const
{

	return localPosition / GetZoomLevel() + GetTopLeft();
}
