#include "precomp.h"
#include "CurvedSegment.h"

#include "LineSegment.h"
#include "Camera/Camera.h"

Engine::CurvedSegment::CurvedSegment( const float2& lStart, const float2& lEnd, const float2& lStartDir,
                                      const float2& lEndDir, const float hardness, const uint color,
                                      const uint drawSteps, const CurveSetupMode setupMode )
	: m_segments(drawSteps)
	  , m_color(color)
{
	SetupPoints(lStart, lEnd, lStartDir, lEndDir, hardness, setupMode);
}

void Engine::CurvedSegment::SetupPoints( const float2& lStart, const float2& lEnd, const float2& lStartDir,
                                         const float2& lEndDir, const float hardness, const CurveSetupMode setupMode )
{
	m_lineStart = lStart;
	m_lineEnd = lEnd;

	assert(length(lStartDir) > 0);
	assert(length(lEndDir) > 0);

	CalculateMidPoints(lStart, lStartDir, lEnd, lEndDir, hardness, m_startMidPoint, m_endMidPoint, setupMode);

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
	for (int i = 0; i <= m_segments; ++i)
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

void Engine::CurvedSegment::RenderWorldPos( const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, const float hardness, const uint color, const uint segments, const CurveSetupMode setupMode )
{
	float2 startMidPoint, endMidPoint;
	CalculateMidPoints(lStart, lStartDir, lEnd, lEndDir, hardness, startMidPoint, endMidPoint, setupMode);

	float2 lastPoint = lStart;
	float t = 0;
	for (uint i = 0; i <= segments; ++i)
	{
		float2 linear_SsM = lerp(lStart, startMidPoint, t);
		float2 linear_sMeM = lerp(startMidPoint, endMidPoint, t);
		float2 linear_eME = lerp(endMidPoint, lEnd, t);
		float2 square_SM = lerp(linear_SsM, linear_sMeM, t);
		float2 square_ME = lerp(linear_sMeM, linear_eME, t);

		const float2 cubic = lerp(square_SM, square_ME, t);

		Engine::LineSegment::RenderWorldPos(camera, drawSurface, lastPoint, cubic, color);
		lastPoint = cubic;
		t += 1.f / static_cast<float>(segments);
	}

}

float Engine::CurvedSegment::GetSegmentLength( const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint segments, CurveSetupMode setupMode )
{
	float2 startMidPoint, endMidPoint;
	CalculateMidPoints(lStart, lStartDir, lEnd, lEndDir, hardness, startMidPoint, endMidPoint, setupMode);

	float2 lastPoint = lStart;
	float segmentLength = 0;
	float t = 0;
	for (uint i = 0; i < segments; ++i)
	{
		float2 linear_SsM = lerp(lStart, startMidPoint, t);
		float2 linear_sMeM = lerp(startMidPoint, endMidPoint, t);
		float2 linear_eME = lerp(endMidPoint, lEnd, t);
		float2 square_SM = lerp(linear_SsM, linear_sMeM, t);
		float2 square_ME = lerp(linear_sMeM, linear_eME, t);

		const float2 cubic = lerp(square_SM, square_ME, t);
		segmentLength += length(lastPoint - cubic);
		lastPoint = cubic;
		t += 1.f / static_cast<float>(segments);
	}
	return segmentLength;
}

float Engine::CurvedSegment::RenderWorldPosAndGetLength( const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint color, uint segments, CurveSetupMode setupMode )
{
	float2 startMidPoint, endMidPoint;
	CalculateMidPoints(lStart, lStartDir, lEnd, lEndDir, hardness, startMidPoint, endMidPoint, setupMode);

	float2 lastPoint = lStart;
	float segmentLength = 0;

	float t = 0;
	for (uint i = 0; i <= segments; ++i)
	{
		float2 linear_SsM = lerp(lStart, startMidPoint, t);
		float2 linear_sMeM = lerp(startMidPoint, endMidPoint, t);
		float2 linear_eME = lerp(endMidPoint, lEnd, t);
		float2 square_SM = lerp(linear_SsM, linear_sMeM, t);
		float2 square_ME = lerp(linear_sMeM, linear_eME, t);

		const float2 cubic = lerp(square_SM, square_ME, t);
		segmentLength += length(lastPoint - cubic);

		Engine::LineSegment::RenderWorldPos(camera, drawSurface, lastPoint, cubic, color);

		lastPoint = cubic;
		t += 1.f / static_cast<float>(segments);
	}
	return segmentLength;
}

float2 Engine::CurvedSegment::GetPositionOnCurvedSegment( const float t, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint segments, CurveSetupMode setupMode )
{
	float2 startMidPoint, endMidPoint;
	CalculateMidPoints(lStart, lStartDir, lEnd, lEndDir, hardness, startMidPoint, endMidPoint, setupMode);

	//Calculate segment lengths
	std::vector<float> segmentLengths;
	float2 lastPoint = lStart;
	float segmentLength = 0;
	float time = 0;
	for (uint i = 0; i < segments; ++i)
	{
		float2 linear_SsM = lerp(lStart, startMidPoint, time);
		float2 linear_sMeM = lerp(startMidPoint, endMidPoint, time);
		float2 linear_eME = lerp(endMidPoint, lEnd, time);
		float2 square_SM = lerp(linear_SsM, linear_sMeM, time);
		float2 square_ME = lerp(linear_sMeM, linear_eME, time);

		const float2 cubic = lerp(square_SM, square_ME, time);
		segmentLength += length(lastPoint - cubic);
		segmentLengths.push_back(segmentLength);
		lastPoint = cubic;
		time += 1.f / static_cast<float>(segments);
	}

	//Find where we are
	float tLength = t * segmentLength;
	if (tLength == 0) tLength = 0.001f;

	for (uint i = 0; i < segments; ++i)
	{
		if (tLength <= segmentLengths[i])
		{
			// We are in between segment i-1 and i
			const float part = (tLength - segmentLengths[i - 1]) / (segmentLengths[i] - segmentLengths[i - 1]);

			float a = segmentLengths[i - 1] / segmentLength;

			float2 linear_SsM = lerp(lStart, startMidPoint, a);
			float2 linear_sMeM = lerp(startMidPoint, endMidPoint, a);
			float2 linear_eME = lerp(endMidPoint, lEnd, a);
			float2 square_SM = lerp(linear_SsM, linear_sMeM, a);
			float2 square_ME = lerp(linear_sMeM, linear_eME, a);
			const float2 segmentA = lerp(square_SM, square_ME, a);

			a = segmentLengths[i] / segmentLength;

			linear_SsM = lerp(lStart, startMidPoint, a);
			linear_sMeM = lerp(startMidPoint, endMidPoint, a);
			linear_eME = lerp(endMidPoint, lEnd, a);
			square_SM = lerp(linear_SsM, linear_sMeM, a);
			square_ME = lerp(linear_sMeM, linear_eME, a);
			const float2 segmentB = lerp(square_SM, square_ME, a);

			return lerp(segmentA, segmentB, part);
		}
	}
	Logger::Error("t of {} was not found on CurvedSegment", t);
	return float2(0, 0);
}

void Engine::CurvedSegment::RenderBezierPoints( const Camera& camera, Surface& drawSurface ) const
{
	DrawCircle(camera, drawSurface, 10, m_lineStart, 10.f, 0x80ff80);
	DrawCircle(camera, drawSurface, 10, m_lineEnd, 10.f, 0xff8080);
	DrawCircle(camera, drawSurface, 4, m_startMidPoint, 8.f, 0x8000ff);
	DrawCircle(camera, drawSurface, 4, m_endMidPoint, 8.f, 0xff0080);
	DrawCircle(camera, drawSurface, 20, intersectionDEBUG, 4.f, 0xff0000);
}

float2 Engine::CurvedSegment::GetPositionOnSegment( const float t ) const
{
	float tLength = t * m_length;
	if (tLength == 0) tLength = 0.001f;
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
	Logger::Error("t of {} was not found on CurvedSegment", t);
	return float2(0, 0);
}

void Engine::CurvedSegment::CalculateMidPoints( const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, const float hardness, float2& outStartMidPoint, float2& outEndMidPoint, const CurveSetupMode setupMode )
{
	assert(length(lStartDir) > 0);
	assert(length(lEndDir) > 0);

	switch (setupMode)
	{
	case CurveSetupMode::SimpleBezier:
		{
			const float2 sMidPointOffset = (lEnd - lStart) * normalize(lStartDir);
			outStartMidPoint = lStart + sMidPointOffset * hardness;

			const float2 eMidPointOffset = (lStart - lEnd) * normalize(lEndDir);
			outEndMidPoint = lEnd + eMidPointOffset * hardness;
			break;
		}
	case CurveSetupMode::LongestBend:
		{
			const float2 sMidPointOffset = (lEnd - lStart) * normalize(lStartDir);
			float sOffsetDistance = length(sMidPointOffset * hardness);

			const float2 eMidPointOffset = (lStart - lEnd) * normalize(lEndDir);
			float eOffsetDistance = length(eMidPointOffset * hardness);

			float halfLength = max(sOffsetDistance, eOffsetDistance);

			if (length(sMidPointOffset) > 0) outStartMidPoint = lStart + normalize(sMidPointOffset) * halfLength;
			else outStartMidPoint = lStart;
			if (length(eMidPointOffset) > 0) outEndMidPoint = lEnd + normalize(eMidPointOffset) * halfLength;
			else outEndMidPoint = lEnd;
			break;
		}
	case CurveSetupMode::ClampedLongest:
		{
			const float2 sMidPointOffset = (lEnd - lStart) * normalize(lStartDir);
			float sOffsetDistance = length(sMidPointOffset * hardness);

			const float2 eMidPointOffset = (lStart - lEnd) * normalize(lEndDir);
			float eOffsetDistance = length(eMidPointOffset * hardness);

			float halfLength = max(sOffsetDistance, eOffsetDistance);

			if (length(sMidPointOffset) > 0) outStartMidPoint = lStart + normalize(sMidPointOffset) * min(halfLength, length(sMidPointOffset));
			else outStartMidPoint = lStart;
			if (length(eMidPointOffset) > 0) outEndMidPoint = lEnd + normalize(eMidPointOffset) * min(halfLength, length(eMidPointOffset));
			else outEndMidPoint = lEnd;
			break;
		}
	}
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
