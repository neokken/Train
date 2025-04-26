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
		explicit GameObject( const Transform& transform );
		virtual ~GameObject() = default;

		virtual void Init( World* world );
		virtual void Update( float deltaTime ) = 0;
		virtual void Render( const Camera& camera ) = 0;

		void Destroy();
		[[nodiscard]] bool MarkedForDestroy() const;

		virtual void ImGuiDebugViewer();

	protected:
		Transform m_transform{};

		Engine::World* m_world{nullptr};
		bool m_destroy{false};
	};
}
