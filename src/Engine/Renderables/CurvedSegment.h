#pragma once
#include "Renderable.h"

namespace Engine
{
	enum class CurveSetupMode
	{
		SimpleBezier, // Simple curve that puts the midpoints along a tangental line based on hardness
		LongestBend, // SimpleBezier but uses the longest length which creates less sharp bends
		ClampedLongest,
		Circular,
	};

	struct CurveData
	{
		float2 lStart;
		float2 lStartDir;
		float2 lEnd;
		float2 lEndDir;
		float hardness = 0.5f;
		CurveSetupMode setupMode = CurveSetupMode::LongestBend;
		uint baseSegments = 5;
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

		void Render( const Camera& camera ) override;
		void RenderBezierPoints( const Camera& camera ) const;

		/**
		 * Render a curve statically from data slightly more expensive than keeping an object
		 */
		static void RenderWorldPos( const Camera& camera, CurveData curve, uint color );

		/**
		* Render a track made of arrows statically
		* @return the length of the drawn track
		*/
		static float RenderArrowsWorldPos( const Camera& camera, const CurveData& curve, uint color, float width );

		/**
		* Render a full track
		* @return the length of the drawn track
		*/
		static float RenderTrackWorldPos( const Camera& camera, Surface& drawSurface, const CurveData& curve, uint trackColor, uint spokeColor, float trackSize, float spokeSize, float trackWidth = 2.f, float spokeWidth = 2.f, float spokeDistance = 20, float wobblyness = 0.2f );

		/**
		* Render 2 lines following a curve statically 
		* @return the length of the drawn track
		*/
		static float RenderTrackLinesWorldPos( const Camera& camera, CurveData curve, uint color, float width );

		/**
		* Render track spokes, should be rendered together with a track
		* @param spokeLength length of spoke horizontally so this should be around the same length as trackwidth
		* @param spokesDistance amount of spokes rendered should probably be based on length returned from rendertrack
		* @param segments base amount of segments to use setting this too low can cause wrong length calculation
		*/
		static void RenderTrackSpokesWorldPos( const Camera& camera, Surface& drawSurface, CurveData curve, uint color, float spokeLength, float spokeWidth, float spokesDistance, float wobblyness = 5.f );

		/**
		 * Get the total real length of a curved segment
		 * @return the length in units
		 */
		static float GetSegmentLength( CurveData curve );
		/**
		 * Render a curve statically from data slightly more expensive than keeping an object
		 * @return
		 * Length of the drawn segment in units
		 */
		static float RenderWorldPosAndGetLength( const Camera& camera, CurveData curve, uint color );
		/**
		 * @param t how far along the path from 0 to 1
		 * @return
		 * position along normalized curved track
		 */
		static float2 GetPositionOnCurvedSegment( const float t, const CurveData& curve );

		///**
		// * @param t how far along the path from 0 to 1
		// * @return
		// * normalized vector direction
		// */
		static float2 GetDirectionOnCurvedSegment( const float t, const CurveData& curve );

		[[nodiscard]] static float2 GetClosestPoint( const CurveData& curve, const float2& position, int samples = 10, float tolerance = 1e-4f );

		/**
		 * Check if a curve is valid, i.e if it doesn't bend too much for its size
		 * @param strictness How strict should the checking be, higher values are more lenient
		 * @param camera | optionally draw the part where the test failed
		 * @param screen -|
		 * @return true if valid
		 */
		static bool CheckCurveValidity( CurveData curve, float strictness, const Camera* camera = nullptr, Surface* screen = nullptr );

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
		static void DrawCircle( const Camera& camera, int segmentCount, const float2& center,
		                        float circleSize, uint color );
	};
}
