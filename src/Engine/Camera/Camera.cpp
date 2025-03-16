#include "precomp.h"
#include "Camera.h"

#include "Input/InputManager.h"

Engine::Camera::Camera( const int2& size )
	: m_resolution{size}
	  , m_wishWidthSize{static_cast<float>(size.x)}
{
}

void Engine::Camera::Init( Engine::InputManager* input )
{
	m_inputManager = input;
}

void Engine::Camera::Update( const float deltaTime )
{
	float2 dir = {0.f};
	float zoomDir = 1;

	if (m_inputManager->IsKeyDown(GLFW_KEY_W)) dir.y -= 1.f;
	if (m_inputManager->IsKeyDown(GLFW_KEY_S)) dir.y += 1.f;
	if (m_inputManager->IsKeyDown(GLFW_KEY_A)) dir.x -= 1.f;
	if (m_inputManager->IsKeyDown(GLFW_KEY_D)) dir.x += 1.f;

	if (m_inputManager->IsMouseJustDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		m_mouseLocked = true;
		m_lockedWorldMousePos = GetWorldPosition(m_inputManager->GetMousePos());
	}
	if (m_inputManager->IsMouseDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		const float2 diff = m_lockedWorldMousePos - GetWorldPosition(m_inputManager->GetMousePos());
		SetPosition(GetPosition() + diff);
	}
	else
	{
		m_mouseLocked = false;
	}

	float evaluatedMoveSpeed = m_moveSpeed;
	if (m_inputManager->IsKeyDown(GLFW_KEY_LEFT_SHIFT)) evaluatedMoveSpeed *= 2.0f;
	SetPosition(GetPosition() + dir * evaluatedMoveSpeed * deltaTime);

	zoomDir += m_inputManager->GetScrollDelta() * deltaTime * m_scrollSensitivity;

	if (sqrLength(dir) > 0.f)
	{
		dir = normalize(dir);
	}

	SetZoomLevel(GetZoomLevel() * zoomDir);
}

float2 Engine::Camera::GetTopLeft() const
{
	const float2 resFloat = make_float2(m_resolution);
	const float zoomAmount = resFloat.x / m_wishWidthSize;

	return m_position - (resFloat * 0.5f) / zoomAmount;
}

float2 Engine::Camera::GetBottomRight() const
{
	const float2 resFloat = make_float2(m_resolution);
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
