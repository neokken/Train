#include "precomp.h"
#include "LineSegment.h"

#include "Camera/Camera.h"
#include "Helpers/Renderer.h"

Engine::LineSegment::LineSegment( const float2& lStart, const float2& lEnd, const uint color )
{
	m_lineStart = lStart;
	m_lineEnd = lEnd;
	m_color = color;
}

void Engine::LineSegment::Render( const Camera& camera, Surface& drawSurface )
{
	RenderWorldPos(camera, drawSurface, m_lineStart, m_lineEnd, m_color);
}

void Engine::LineSegment::RenderWorldPos( const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lEnd, uint lColor )
{
	float2 localPosS = camera.GetCameraPosition(lStart);
	float2 localPosE = camera.GetCameraPosition(lEnd);
	Renderer::GetRenderer().DrawLine({float3(localPosS, 0.f), float3(localPosE, 0.f), RGB8ToRGB32(lColor), 1.f});
	//drawSurface.Line(localPosS.x, localPosS.y, localPosE.x, localPosE.y, lColor);
}

void Engine::LineSegment::RenderLocalPos( Surface& drawSurface, const float2& lStart, const float2& lEnd, uint lColor )
{
	drawSurface.Line(lStart.x, lStart.y, lEnd.x, lEnd.y, lColor);
}
