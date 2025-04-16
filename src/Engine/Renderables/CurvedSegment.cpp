#include "precomp.h"
#include "CurvedSegment.h"

#include "Circle.h"
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
		const float2 cubic = CubicBezier(m_lineStart, m_startMidPoint, m_endMidPoint, m_lineEnd, t);

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
		const float2 cubic = CubicBezier(m_lineStart, m_startMidPoint, m_endMidPoint, m_lineEnd, t);

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
		const float2 cubic = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, t);

		Engine::LineSegment::RenderWorldPos(camera, drawSurface, lastPoint, cubic, color);
		lastPoint = cubic;
		t += 1.f / static_cast<float>(segments);
	}
}

float Engine::CurvedSegment::RenderArrowsWorldPos( const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint color, float width, uint segments, CurveSetupMode setupMode )
{
	float2 startMidPoint, endMidPoint;
	CalculateMidPoints(lStart, lStartDir, lEnd, lEndDir, hardness, startMidPoint, endMidPoint, setupMode);

	float2 lastPoint = lStart;
	float2 lastDir = lStartDir;
	float t = 1.f / static_cast<float>(segments);
	float segmentLength = 0;
	for (uint i = 0; i < segments; ++i)
	{
		const float2 cubic = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, t);

		float2 offset = cubic - lastPoint;
		float l = length(offset);
		segmentLength += l;
		float2 dir = offset / l;

		float2 lastP = lastPoint + float2(lastDir.y, -lastDir.x) * width;

		Engine::LineSegment::RenderWorldPos(camera, drawSurface, lastP, cubic, color);
		lastP = lastPoint + float2(-lastDir.y, lastDir.x) * width;
		Engine::LineSegment::RenderWorldPos(camera, drawSurface, lastP, cubic, color);
		lastPoint = cubic;
		lastDir = dir;
		t += 1.f / static_cast<float>(segments);
	}
	return segmentLength;
}

float Engine::CurvedSegment::RenderTrackWorldPos( const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint trackColor, uint spokeColor, float trackSize, float spokeSize, float trackWidth, float spokeWidth, float spokeDistance, float wobblyness, uint segments, CurveSetupMode setupMode )
{
	RenderTrackSpokesWorldPos(camera, drawSurface, lStart, lStartDir, lEnd, lEndDir, hardness, spokeColor, spokeSize, spokeWidth, spokeDistance, segments, wobblyness, setupMode);
	RenderTrackLinesWorldPos(camera, drawSurface, lStart, lStartDir, lEnd, lEndDir, hardness, trackColor, trackSize, segments, setupMode);
	return RenderTrackLinesWorldPos(camera, drawSurface, lStart, lStartDir, lEnd, lEndDir, hardness, trackColor, trackSize - trackWidth, segments, setupMode);
}

float Engine::CurvedSegment::RenderTrackLinesWorldPos( const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint color, float width, uint segments, CurveSetupMode setupMode )
{
	float2 startMidPoint, endMidPoint;
	CalculateMidPoints(lStart, lStartDir, lEnd, lEndDir, hardness, startMidPoint, endMidPoint, setupMode);

	float2 lastPoint = lStart;
	float2 lastDir = lStartDir;
	float t = 1.f / static_cast<float>(segments);
	float segmentLength = 0;
	for (uint i = 0; i < segments; ++i)
	{
		const float2 cubic = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, t);

		float2 offset = cubic - lastPoint;
		float l = length(offset);
		segmentLength += l;
		float2 dir = offset / l;

		float2 lastP = lastPoint + float2(lastDir.y, -lastDir.x) * width;
		float2 P = cubic + float2(dir.y, -dir.x) * width;
		Engine::LineSegment::RenderWorldPos(camera, drawSurface, lastP, P, color);
		lastP = lastPoint + float2(-lastDir.y, lastDir.x) * width;
		P = cubic + float2(-dir.y, dir.x) * width;
		Engine::LineSegment::RenderWorldPos(camera, drawSurface, lastP, P, color);
		lastPoint = cubic;
		lastDir = dir;
		t += 1.f / static_cast<float>(segments);
	}
	return segmentLength;
}

void Engine::CurvedSegment::RenderTrackSpokesWorldPos( const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint color, float spokeLength, float spokeWidth, float spokesDistance, uint segments, float wobblyness, CurveSetupMode setupMode )
{
	float2 startMidPoint, endMidPoint;
	CalculateMidPoints(lStart, lStartDir, lEnd, lEndDir, hardness, startMidPoint, endMidPoint, setupMode);

	float2 lastPoint = lStart;
	float t = 1.f / static_cast<float>(segments);
	float spokes = 0.f;
	for (uint i = 0; i < segments; ++i)
	{
		const float2 cubic = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, t);

		float2 offset = cubic - lastPoint;
		float l = length(offset);
		float2 dir = offset / l;

		spokes += l / spokesDistance;
		int spokesInSegment = static_cast<int>(round(spokes));
		for (int spokeI = 0; spokeI < spokesInSegment; ++spokeI)
		{
			spokes -= 1;
			float2 point = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, t - ((1 / static_cast<float>(segments)) / static_cast<float>(spokesInSegment)) * static_cast<float>(spokeI));
			if (spokeWidth <= 0)
			{
				float2 P = point + float2(dir.y, -dir.x) * spokeLength;
				float2 P2 = point + float2(-dir.y, dir.x) * spokeLength;
				if (wobblyness > 0)
				{
					uint seed = InitSeed(i);
					P += dir * (RandomFloat(seed) - 0.5f) * 2.f * wobblyness;
					P2 += dir * (RandomFloat(seed) - 0.5f) * 2.f * wobblyness;
				}
				Engine::LineSegment::RenderWorldPos(camera, drawSurface, P, P2, color);
			}
			else
			{
				float2 d = dir;
				if (wobblyness > 0)
				{
					uint seed = InitSeed(i);
					d = normalize(dir + float2((RandomFloat(seed) - 0.5f) * 2.f * wobblyness, (RandomFloat(seed) - 0.5f) * 2.f * wobblyness));
				}
				drawSurface.LineRectangle(camera.GetCameraPosition(point), d, float2(spokeLength * 2, spokeWidth * 2) * camera.GetZoomLevel(), color);
			}
		}
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
		const float2 cubic = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, t);

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
		const float2 cubic = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, t);

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
		const float2 cubic = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, time);
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
			const float2 segmentA = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, a);

			a = segmentLengths[i] / segmentLength;
			const float2 segmentB = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, a);

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
			const float2 segmentA = CubicBezier(m_lineStart, m_startMidPoint, m_endMidPoint, m_lineEnd, a);

			a = m_segmentLengths[i] / m_length;
			const float2 segmentB = CubicBezier(m_lineStart, m_startMidPoint, m_endMidPoint, m_lineEnd, a);

			return lerp(segmentA, segmentB, part);
		}
	}
	Logger::Error("t of {} was not found on CurvedSegment", t);
	return float2(0, 0);
}

bool Engine::CurvedSegment::CheckCurveValidity( const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, const float hardness, uint segments, CurveSetupMode setupMode, float strictness, const Camera* camera, Surface* screen )
{
	float2 startMidPoint, endMidPoint;
	CalculateMidPoints(lStart, lStartDir, lEnd, lEndDir, hardness, startMidPoint, endMidPoint, setupMode);

	float2 lastDir = float2(0);
	float2 lastPoint = lStart;
	float time = 1.f / static_cast<float>(segments);

	for (uint i = 0; i < segments; ++i)
	{
		const float2 cubic = CubicBezier(lStart, startMidPoint, endMidPoint, lEnd, time);

		float2 dir;
		if (i == 0)
		{
			dir = normalize(cubic - lStart);
			lastDir = lStartDir;
		}
		else
		{
			dir = normalize(cubic - lastPoint);
		}

		float l = length(lastPoint - cubic);
		float diff = acos(dot(dir, lastDir));
		diff = diff / l;
		if (diff > strictness)
		{
			if (camera != nullptr && screen != nullptr)
			{
				Engine::Circle::RenderWorldPos(*camera, *screen, cubic, 10.f, 0xff0000);
				Engine::Circle::RenderWorldPos(*camera, *screen, lastPoint, 10.f, 0xff0000);
				Engine::LineSegment::RenderWorldPos(*camera, *screen, lastPoint + float2(1, 1), cubic + float2(1, 1), 0xff0000);
			}
			return false;
		}

		lastDir = dir;
		lastPoint = cubic;
		time += 1.f / static_cast<float>(segments);
	}
	return true;
}

float2 Engine::CurvedSegment::GetClosestPoint( const float2& position, int samples, float tolerance ) const
{
	float left = 0.f;
	float right = 1.f;

	for (int i = 0; i < samples; i++)
	{
		const float diff = right - left;
		if (diff < tolerance)
		{
			break;
		}

		const float rLeft = right - diff / golden_ratio;
		const float rRight = left + diff / golden_ratio;

		const float sqrLengthLeft = sqrLength(this->GetPositionOnSegment(rLeft) - position);
		const float sqrLengthRight = sqrLength(this->GetPositionOnSegment(rRight) - position);

		if (sqrLengthLeft < sqrLengthRight)
		{
			right = rRight;
		}
		else
		{
			left = rLeft;
		}
	}

	return this->GetPositionOnSegment((left + right) / 2);
}

void Engine::CurvedSegment::CalculateMidPoints( const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, const float hardness, float2& outStartMidPoint, float2& outEndMidPoint, const CurveSetupMode setupMode )
{
	assert(length(lStartDir) > 0);
	assert(length(lEndDir) > 0);

	switch (setupMode)
	{
	case CurveSetupMode::SimpleBezier:
		{
			const float2 sMidPointOffset = length(lEnd - lStart) * normalize(lStartDir);
			outStartMidPoint = lStart + sMidPointOffset * hardness;

			const float2 eMidPointOffset = length(lStart - lEnd) * normalize(lEndDir);
			outEndMidPoint = lEnd + eMidPointOffset * hardness;
			break;
		}
	case CurveSetupMode::LongestBend:
		{
			const float2 sMidPointOffset = length(lEnd - lStart) * normalize(lStartDir);
			float sOffsetDistance = length(sMidPointOffset * hardness);

			const float2 eMidPointOffset = length(lStart - lEnd) * normalize(lEndDir);
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
			const float2 sMidPointOffset = length(lEnd - lStart) * normalize(lStartDir);
			float sOffsetDistance = length(sMidPointOffset * hardness);

			const float2 eMidPointOffset = length(lStart - lEnd) * normalize(lEndDir);
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
