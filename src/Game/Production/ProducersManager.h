#pragma once

#include "ProduceType.h"

namespace Engine
{
	class World;
}

class TrackManager;

namespace Game
{
	class Producer;

	class ProducersManager
	{
	public:
		ProducersManager() = default;
		void Init( Engine::World* world, TrackManager* trackManager );

		void Simulate() const;

	private:
		void CreateProducer( const ProduceType type, uint stationSize );

	private:
		Engine::World* m_world;
		TrackManager* m_trackManager;

		std::vector<Producer*> m_producers;
	};
}
