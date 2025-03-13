#pragma once
#include "Renderable.h"




class LineSegment final : public Renderable
{
public:
	LineSegment() = default;
	LineSegment(const float2& lStart, const float2& lEnd, const uint color)
		: m_lineStart(lStart)
		, m_lineEnd(lEnd)
		, m_color(color) {}


	void Render(const Camera& camera, Surface& drawSurface) override;
	static void RenderWorldPos(const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lEnd, uint lColor);
	static void RenderLocalPos(Surface& drawSurface, const float2& lStart, const float2& lEnd, uint lColor);

private:
	float2 m_lineStart{0.f};
	float2 m_lineEnd{0.f};

	uint m_color{ 0xffffffff };
};

