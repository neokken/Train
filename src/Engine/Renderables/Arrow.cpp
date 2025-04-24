#include "precomp.h"
#include "Arrow.h"

#include "LineSegment.h"

Engine::Arrow::Arrow( const float2& base, const float2& dir, const float length, const uint color )
{
	m_base = base;
	m_dir = dir;
	m_length = length;

	m_color = color;
}

void Engine::Arrow::Render( const Camera& camera )
{
	const float2 normDir = normalize(m_dir);
	const float2 tip = m_base + normDir * m_length;

	const float arrowHeadLength = m_length * 0.25f;
	const float2 perp = float2(-normDir.y, normDir.x);

	const float2 headLeft = tip - normDir * arrowHeadLength + perp * (arrowHeadLength * 0.5f);
	const float2 headRight = tip - normDir * arrowHeadLength - perp * (arrowHeadLength * 0.5f);

	LineSegment::RenderWorldPos(camera, m_base, tip, m_color);

	LineSegment::RenderWorldPos(camera, tip, headLeft, m_color);
	LineSegment::RenderWorldPos(camera, tip, headRight, m_color);
}

void Engine::Arrow::RenderWorldPos( const Camera& camera, const float2& base, const float2& dir, const float length, const uint color )
{
	const float2 normDir = normalize(dir);
	const float2 tip = base + normDir * length;

	const float arrowHeadLength = length * 0.25f;
	const float2 perp = float2(-normDir.y, normDir.x);

	const float2 headLeft = tip - normDir * arrowHeadLength + perp * (arrowHeadLength * 0.5f);
	const float2 headRight = tip - normDir * arrowHeadLength - perp * (arrowHeadLength * 0.5f);

	LineSegment::RenderWorldPos(camera, base, tip, color);

	LineSegment::RenderWorldPos(camera, tip, headLeft, color);
	LineSegment::RenderWorldPos(camera, tip, headRight, color);
}
