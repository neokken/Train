#pragma once
#include "Renderable.h"

namespace Engine
{
	class LineSegment final : public Engine::Renderable
	{
	public:
		LineSegment() = default;
		LineSegment( const float2& lStart, const float2& lEnd, const uint color );

		void Render( const Camera& camera, Surface& drawSurface ) override;
		static void RenderWorldPos( const Camera& camera, Surface& drawSurface, const float2& lStart,
		                            const float2& lEnd, uint lColor );
		static void RenderLocalPos( Surface& drawSurface, const float2& lStart, const float2& lEnd, uint lColor );

	private:
		float2 m_lineStart{0.f};
		float2 m_lineEnd{0.f};

		uint m_color{0xffffffff};
	};
}
