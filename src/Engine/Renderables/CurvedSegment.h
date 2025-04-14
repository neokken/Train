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
		               float hardness, const uint color, const uint drawSteps = 10u );
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

		void RenderBezierPoints( const Camera& camera, Surface& drawSurface ) const;

		/**
		 * Get position along normalized curved track 
		 * @param t how far along the path from 0 to 1
		 * @return 
		 */
		float2 GetPositionOnSegment( float t ) const;

	private:
		float2 m_lineStart{0.f};
		float2 m_lineEnd{0.f};
		float2 m_startMidPoint{0.f};
		float2 m_endMidPoint{0.f};
		int m_segments{10};
		std::vector<float> m_segmentLengths;
		float m_length{0.f};

		uint m_color{0xffffffff};
		static void DrawCircle( const Camera& camera, Surface& targetSurface, int segmentCount, const float2& center,
		                        float circleSize, uint color );
	};
}
