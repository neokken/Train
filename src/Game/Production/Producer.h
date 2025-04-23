#pragma once

#include "GameObject/GameObject.h"
#include "ProduceType.h"

namespace Game
{
	class Producer : public Engine::GameObject
	{
	public:
		Producer( Engine::Transform transform, ProduceType type );
		void Update( float deltaTime ) override;
		void Render( const Engine::Camera& camera, Surface& target ) override;

	private:
		float m_time = 0.f;
		ProduceType m_type = ProduceType::Unknown;
	};
}
