#include "precomp.h"
#include "Camera.h"


float2 Camera::GetTopLeft() const
{
	const float2 resFloat = make_float2(m_resolution);
	const float zoomAmount = resFloat.x / m_wishWidthSize;

	return  m_position - (resFloat * 0.5f) / zoomAmount;
}

float2 Camera::GetBottomRight() const
{
	const float2 resFloat = make_float2(m_resolution);
	const float zoomAmount = resFloat.x / m_wishWidthSize;

	return m_position + (resFloat * 0.5f) / zoomAmount;
}

float Camera::GetZoomLevel() const
{
	return static_cast<float>(m_resolution.x) / m_wishWidthSize;
}

float2 Camera::GetCameraPosition(const float2& worldPosition) const
{
	return (worldPosition - GetTopLeft()) * GetZoomLevel();
}

float2 Camera::GetWorldPosition(const float2& localPosition) const
{
	
	return localPosition / GetZoomLevel() + GetTopLeft();
}
