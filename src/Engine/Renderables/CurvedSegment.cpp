#include "precomp.h"
#include "CurvedSegment.h"

#include "LineSegment.h"
#include "Camera/Camera.h"

Engine::CurvedSegment::CurvedSegment( const float2& lStart, const float2& lEnd, const float2& lStartDir,
                                      const float2& lEndDir, const float hardness, const uint color,
                                      const uint drawSteps )
	: m_color(color)
	, m_STEPSIZE(1.f/static_cast<float>(drawSteps))
{
	SetupPoints(lStart, lEnd, lStartDir, lEndDir, hardness);
}

void Engine::CurvedSegment::SetupPoints( const float2& lStart, const float2& lEnd, const float2& lStartDir,
                                         const float2& lEndDir, const float hardness )
{
	m_lineStart = lStart;
	m_lineEnd = lEnd;

	const float2 sMidPointOffset = (lEnd - lStart) * lStartDir;
	m_startMidPoint = lStart + sMidPointOffset * hardness;

	const float2 eMidPointOffset = (lStart - lEnd) * lEndDir;
	m_endMidPoint = lEnd + eMidPointOffset * hardness;
}

void Engine::CurvedSegment::Render( const Camera& camera, Surface& drawSurface )
{
	RenderWorldPos(camera, drawSurface);
}

void Engine::CurvedSegment::RenderWorldPos( const Camera& camera, Surface& drawSurface )
{
	float2 lastPoint = m_lineStart;
	float t = m_STEPSIZE;
	while (t <= 1.0001f)
	{
		float2 linear_SsM = lerp(m_lineStart, m_startMidPoint, t);
		float2 linear_sMeM = lerp(m_startMidPoint, m_endMidPoint, t);
		float2 linear_eME = lerp(m_endMidPoint, m_lineEnd, t);
		float2 square_SM = lerp(linear_SsM, linear_sMeM, t);
		float2 square_ME = lerp(linear_sMeM, linear_eME, t);

		const float2 cubic = lerp(square_SM, square_ME, t);

		Engine::LineSegment::RenderWorldPos(camera, drawSurface, lastPoint, cubic, m_color);
		lastPoint = cubic;
		t += m_STEPSIZE;
	}
}

void Engine::CurvedSegment::RenderLocalPos( Surface& drawSurface )
{
}

void Engine::CurvedSegment::RenderBezierPoints( const Camera& camera, Surface& drawSurface )
{
	DrawCircle(camera, drawSurface, 10, m_lineStart, 10.f, 0x80ff80);
	DrawCircle(camera, drawSurface, 10, m_lineEnd, 10.f, 0xff8080);
	DrawCircle(camera, drawSurface, 4, m_startMidPoint, 8.f, 0x8000ff);
	DrawCircle(camera, drawSurface, 4, m_endMidPoint, 8.f, 0xff0080);
}

void Engine::CurvedSegment::DrawCircle( const Camera& camera, Surface& targetSurface, const int segmentCount,
                                        const float2& center, const float circleSize, uint
                                        color )
{
	const float segmentCountF = static_cast<float>(segmentCount);

	for (int i = 0; i < segmentCount; i++)
	{
		const float iF = static_cast<float>(i);

		const float angle1 = (2.0f * PI * iF) / segmentCountF;
		const float angle2 = (2.0f * PI * (iF + 1.f)) / segmentCountF;

		float2 p1 = center + float2{circleSize * cos(angle1), circleSize * sin(angle1)};
		float2 p2 = center + float2{circleSize * cos(angle2), circleSize * sin(angle2)};

		Engine::LineSegment::RenderWorldPos(camera, targetSurface, p1, p2, color);
	}
}
