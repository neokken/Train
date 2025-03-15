#pragma once

namespace Engine
{
	class Camera;

	class Renderable
	{
	public:
		Renderable() = default;
		virtual ~Renderable() = default;
		virtual void Render( const Camera& camera, Surface& drawSurface ) = 0;
	};
}
