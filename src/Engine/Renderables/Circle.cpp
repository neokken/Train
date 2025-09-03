#include "precomp.h"
#include "Circle.h"
#include "LineSegment.h"

Engine::Circle::Circle( const float2& center, float size, uint color, int segmentCount ) :
	m_center(center), m_size(size), m_color(color), m_segmentCount(segmentCount)
{
}

void Engine::Circle::Render( const Camera& camera )
{
	RenderWorldPos(camera, m_center, m_size, m_color, 0.f, m_segmentCount);
}

void Engine::Circle::RenderWorldPos( const Camera& camera, const float2& center, float size, uint color, float width, int segmentCount )
{
	const float segmentCountF = static_cast<float>(segmentCount);

	for (int i = 0; i < segmentCount; i++)
	{
		const float iF = static_cast<float>(i);

		const float angle1 = (2.0f * PI * iF) / segmentCountF;
		const float angle2 = (2.0f * PI * (iF + 1.f)) / segmentCountF;

		float2 p1 = center + float2{size * cos(angle1), size * sin(angle1)};
		float2 p2 = center + float2{size * cos(angle2), size * sin(angle2)};
		float2 dir = normalize(p1 - p2);
		Engine::LineSegment::RenderWorldPos(camera, p1 + dir * width * 0.145f, p2 - dir * width * 0.145f, color, HeightLayer::Default, width);
	}
}
