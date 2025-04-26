#pragma once

#include "GameObject/GameObject.h"

namespace Game
{
	class Building : public Engine::GameObject
	{
	public:
		Building( Engine::Transform transform );
		void Update( float deltaTime ) override;
		void Render( const Engine::Camera& camera ) override;

	private:
		float m_time = 0.f;
	};
}
