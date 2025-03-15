#pragma once

#include "Data/Transform.h"

namespace Engine
{
	class Camera;
	class World;

	class GameObject
	{
	public:
		GameObject() = default;
		GameObject( Transform transform );
		virtual ~GameObject() = default;

		void Init( World* world );
		virtual void Update( float deltaTime ) = 0;
		virtual void Render( const Camera& camera, Surface& target ) = 0;

		void Destroy();
		bool MarkedForDestroy() const;

	protected:
		Transform m_transform;
		bool m_destroy = false;

		Engine::World* m_world;
	};
}
