#pragma once
#include "Renderable.h"

namespace Engine
{
	enum class CurveSetupMode
	{
		SimpleBezier, // Simple curve that puts the midpoints along a tangental line based on hardness
		LongestBend, // SimpleBezier but uses the longest length which creates less sharp bends
		ClampedLongest,
	};

	class CurvedSegment :
		public Engine::Renderable
	{
	public:
		CurvedSegment() = default;
		CurvedSegment( const float2& lStart, const float2& lEnd, const float2& lStartDir, const float2& lEndDir,
		               float hardness, const uint color, const uint drawSteps = 10u, CurveSetupMode setupMode = CurveSetupMode::LongestBend );
		/**
		 * Set up the mid points for the curve
		 * @param lStart line start
		 * @param lEnd line end
		 * @param lStartDir direction of start point
		 * @param lEndDir  direction of end point
		 * @param hardness hardness of the curve, 0 means a straight line 1 is a semi hard curve at the middle, values above 1 can cause bulging
		 * @param setupMode
		 */
		void SetupPoints( const float2& lStart, const float2& lEnd, const float2& lStartDir, const float2& lEndDir,
		                  float hardness, CurveSetupMode setupMode = CurveSetupMode::LongestBend );

		void Render( const Camera& camera, Surface& drawSurface ) override;
		void RenderBezierPoints( const Camera& camera, Surface& drawSurface ) const;

		/**
		 * Render a curve statically from data slightly more expensive than keeping an object
		 */
		static void RenderWorldPos( const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint color, uint segments = 10, CurveSetupMode setupMode = CurveSetupMode::LongestBend );

		/**
		 * Get the total real length of a curved segment
		 * @return the length in units
		 */
		static float GetSegmentLength( const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint segments = 10, CurveSetupMode setupMode = CurveSetupMode::LongestBend );
		/**
		 * Render a curve statically from data slightly more expensive than keeping an object
		 * @return
		 * Length of the drawn segment in units
		 */
		static float RenderWorldPosAndGetLength( const Camera& camera, Surface& drawSurface, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint color, uint segments = 10, CurveSetupMode setupMode = CurveSetupMode::LongestBend );
		/**
		 * @param t how far along the path from 0 to 1
		 * @return
		 * position along normalized curved track
		 */
		static float2 GetPositionOnCurvedSegment( const float t, const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, uint segments = 10, CurveSetupMode setupMode = CurveSetupMode::LongestBend );
		/**
		 * Get position along normalized curved track 
		 * @param t how far along the path from 0 to 1
		 * @return 
		 */
		float2 GetPositionOnSegment( float t ) const;

	private:
		static void CalculateMidPoints( const float2& lStart, const float2& lStartDir, const float2& lEnd, const float2& lEndDir, float hardness, float2& outStartMidPoint, float2& outEndMidPoint, CurveSetupMode setupMode = CurveSetupMode::LongestBend );
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
