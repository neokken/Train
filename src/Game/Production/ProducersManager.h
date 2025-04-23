#pragma once

#include "ProduceType.h"

namespace Engine
{
	class World;
}

class TrackManager;

namespace Game
{
	class ProducersManager
	{
	public:
		ProducersManager() = default;
		void Init( Engine::World* world, TrackManager* trackManager );

	private:
		void CreateProducer( const ProduceType type, uint stationSize ) const;

	private:
		Engine::World* m_world;
		TrackManager* m_trackManager;
	};
}
