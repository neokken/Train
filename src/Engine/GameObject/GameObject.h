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

		// TODO: Remove virtual
		virtual void Init( World* world );
		virtual void Start();
		virtual void Update( float deltaTime ) = 0;
		virtual void Render( const Camera& camera, Surface& target ) = 0;

		void Destroy();
		[[nodiscard]] bool MarkedForDestroy() const;

		virtual void ImGuiDebugViewer();

	protected:
		Transform m_transform{};

		Engine::World* m_world{nullptr};
		bool m_destroy{false};
	};
}
