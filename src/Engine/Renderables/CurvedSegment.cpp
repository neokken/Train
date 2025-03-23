#include "precomp.h"
#include "CurvedSegment.h"

#include "LineSegment.h"
#include "Camera/Camera.h"

Engine::CurvedSegment::CurvedSegment( const float2& lStart, const float2& lEnd, const float2& lStartDir,
                                      const float2& lEndDir, const float hardness, const uint color,
                                      const uint drawSteps )
	: m_segments(drawSteps)
	  , m_color(color)
{
	SetupPoints(lStart, lEnd, lStartDir, lEndDir, hardness);
}

void Engine::CurvedSegment::SetupPoints( const float2& lStart, const float2& lEnd, const float2& lStartDir,
                                         const float2& lEndDir, const float hardness )
{
	m_lineStart = lStart;
	m_lineEnd = lEnd;

	assert(length(lStartDir) > 0);
	assert(length(lEndDir) > 0);

	const float2 sMidPointOffset = (lEnd - lStart) * normalize(lStartDir);
	m_startMidPoint = lStart + sMidPointOffset * hardness;

	const float2 eMidPointOffset = (lStart - lEnd) * normalize(lEndDir);
	m_endMidPoint = lEnd + eMidPointOffset * hardness;

	float2 lastPoint = m_lineStart;
	float segmentLength = 0;
	m_segmentLengths.clear();
	float t = 0;
	for (int i = 0; i < m_segments; ++i)
	{
		float2 linear_SsM = lerp(m_lineStart, m_startMidPoint, t);
		float2 linear_sMeM = lerp(m_startMidPoint, m_endMidPoint, t);
		float2 linear_eME = lerp(m_endMidPoint, m_lineEnd, t);
		float2 square_SM = lerp(linear_SsM, linear_sMeM, t);
		float2 square_ME = lerp(linear_sMeM, linear_eME, t);

		const float2 cubic = lerp(square_SM, square_ME, t);
		segmentLength += length(lastPoint - cubic);
		m_segmentLengths.push_back(segmentLength);
		lastPoint = cubic;
		t += 1.f / static_cast<float>(m_segments);
	}
	m_length = segmentLength;
}

void Engine::CurvedSegment::Render( const Camera& camera, Surface& drawSurface )
{
	float2 lastPoint = m_lineStart;
	float t = 0;
	for (int i = 0; i < m_segments; ++i)
	{
		float2 linear_SsM = lerp(m_lineStart, m_startMidPoint, t);
		float2 linear_sMeM = lerp(m_startMidPoint, m_endMidPoint, t);
		float2 linear_eME = lerp(m_endMidPoint, m_lineEnd, t);
		float2 square_SM = lerp(linear_SsM, linear_sMeM, t);
		float2 square_ME = lerp(linear_sMeM, linear_eME, t);

		const float2 cubic = lerp(square_SM, square_ME, t);

		Engine::LineSegment::RenderWorldPos(camera, drawSurface, lastPoint, cubic, m_color);
		lastPoint = cubic;
		t += 1.f / static_cast<float>(m_segments);
	}
}

void Engine::CurvedSegment::RenderBezierPoints( const Camera& camera, Surface& drawSurface ) const
{
	DrawCircle(camera, drawSurface, 10, m_lineStart, 10.f, 0x80ff80);
	DrawCircle(camera, drawSurface, 10, m_lineEnd, 10.f, 0xff8080);
	DrawCircle(camera, drawSurface, 4, m_startMidPoint, 8.f, 0x8000ff);
	DrawCircle(camera, drawSurface, 4, m_endMidPoint, 8.f, 0xff0080);
}

float2 Engine::CurvedSegment::GetPositionOnSegment( const float t ) const
{
	const float tLength = t * m_length;
	//This can probably be more optimized
	for (int i = 0; i < m_segments; ++i)
	{
		if (tLength <= m_segmentLengths[i])
		{
			// We are in between segment i-1 and i
			const float part = (tLength - m_segmentLengths[i - 1]) / (m_segmentLengths[i] - m_segmentLengths[i - 1]);

			float a = m_segmentLengths[i - 1] / m_length;

			float2 linear_SsM = lerp(m_lineStart, m_startMidPoint, a);
			float2 linear_sMeM = lerp(m_startMidPoint, m_endMidPoint, a);
			float2 linear_eME = lerp(m_endMidPoint, m_lineEnd, a);
			float2 square_SM = lerp(linear_SsM, linear_sMeM, a);
			float2 square_ME = lerp(linear_sMeM, linear_eME, a);
			const float2 segmentA = lerp(square_SM, square_ME, a);

			a = m_segmentLengths[i] / m_length;

			linear_SsM = lerp(m_lineStart, m_startMidPoint, a);
			linear_sMeM = lerp(m_startMidPoint, m_endMidPoint, a);
			linear_eME = lerp(m_endMidPoint, m_lineEnd, a);
			square_SM = lerp(linear_SsM, linear_sMeM, a);
			square_ME = lerp(linear_sMeM, linear_eME, a);
			const float2 segmentB = lerp(square_SM, square_ME, a);

			return lerp(segmentA, segmentB, part);

		}
	}
	Logger::Error("t was not found on CurvedSegment");
	return float2(0, 0);
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
