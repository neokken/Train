#pragma once

#include "Renderable.h"

namespace Engine
{
	class Arrow final : public Engine::Renderable
	{
	public:
		Arrow() = default;
		Arrow( const float2& base, const float2& dir, float length, uint color );

		void Render( const Camera& camera, Surface& drawSurface ) override;
		static void RenderWorldPos( const Camera& camera, Surface& drawSurface, const float2& base, const float2& dir, float length, uint color );

	private:
		float2 m_base;
		float2 m_dir;
		float m_length;

		uint m_color{0xffffffff};
	};
}
