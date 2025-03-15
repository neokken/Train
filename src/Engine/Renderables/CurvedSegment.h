#pragma once
#include "Renderable.h"

namespace Engine
{
	class CurvedSegment :
		public Engine::Renderable
	{
	public:
		CurvedSegment() = default;
		CurvedSegment( const float2& lStart, const float2& lEnd, const float2& lStartDir, const float2& lEndDir,
		               float hardness, const uint color );
		/**
		 * Set up the mid points for the curve
		 * @param lStart line start
		 * @param lEnd line end
		 * @param lStartDir direction of start point
		 * @param lEndDir  direction of end point
		 * @param hardness hardness of the curve, 0 means a straight line 1 is a semi hard curve at the middle, values above 1 can cause bulging
		 */
		void SetupPoints( const float2& lStart, const float2& lEnd, const float2& lStartDir, const float2& lEndDir,
		                  float hardness );
		void Render( const Camera& camera, Surface& drawSurface ) override;
		void RenderWorldPos( const Camera& camera, Surface& drawSurface );
		void RenderLocalPos( Surface& drawSurface );

		void RenderBezierPoints( const Camera& camera, Surface& drawSurface );

	private:
		float2 m_lineStart{0.f};
		float2 m_lineEnd{0.f};
		float2 m_startMidPoint{0.f};
		float2 m_endMidPoint{0.f};
		const float m_STEPSIZE{1.f / 10.f};
		uint m_color{0xffffffff};
		static void DrawCircle( const Camera& camera, Surface& targetSurface, int segmentCount, const float2& center,
		                        float circleSize, uint color );
	};
}
