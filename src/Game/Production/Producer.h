#pragma once

#include "GameObject/GameObject.h"
#include "ProducerStats.h"

namespace Game
{
	class Producer : public Engine::GameObject
	{
	public:
		Producer( Engine::Transform transform, ProducerStats stats );
		void Update( float deltaTime ) override;
		void Render( const Engine::Camera& camera, Surface& target ) override;

		void Simulate();

	private:
		ProducerStats m_stats;

		uint m_produce = 0;
		uint m_ticksLeftUntilNextProduction = 0;
	};
}
