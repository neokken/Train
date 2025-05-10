#pragma once
#include "Renderable.h"

namespace Engine
{
	class Circle final : public Engine::Renderable
	{
	public:
		Circle() = default;
		Circle( const float2& center, float size, uint color, int segmentCount = 12 );

		void Render( const Camera& camera ) override;
		static void RenderWorldPos( const Camera& camera, const float2& center, float size, uint color, float width = 0.f, int segmentCount = 12 );

	private:
		float2 m_center;
		float m_size = 0.0f;
		uint m_color{0xffffffff};
		int m_segmentCount;
	};
}
