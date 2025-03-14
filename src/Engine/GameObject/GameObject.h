#pragma once

#include "Data/Transform.h"

namespace Engine {
	class GameObject {
	public:
		GameObject() = default;
		virtual ~GameObject() = default;

		virtual void Update( float deltaTime ) = 0;

		void Destroy();
		bool MarkedForDestroy() const;
	private:
		Transform m_transform;

		bool m_destroy = false;
	};
}