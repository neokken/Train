#include "precomp.h"
#include "ProducersManager.h"

#include "Engine/World/World.h"
#include "Game/Production/Producer.h"

void Game::ProducersManager::Init( Engine::World* world, TrackManager* trackManager )
{
	m_world = world;
	m_trackManager = trackManager;

	uint count = (RandomUInt() % 25) + 1;
	for (uint i = 0; i < count; i++)
	{
		CreateProducer(ProduceType::Wood, (RandomUInt() % 5) + 1);
	}
}

void Game::ProducersManager::Simulate() const
{
	for (Producer* producer : m_producers)
	{
		producer->Simulate();
	}
}

void Game::ProducersManager::CreateProducer( const ProduceType type, uint stationSize )
{
	float2 position = SnapToGrid(float2(Rand(500.0f) - 250.0f, Rand(500.0f) - 250.0f), 10);

	ProducerStats stats = {};
	stats.stationSize = stationSize;
	stats.produceLimit = 500;
	stats.productionRate = 20;
	stats.producePerProduction = 1;
	stats.type = type;

	m_producers.push_back(new Producer({.position = position}, stats));
	m_world->AddObject(
		m_producers.back()
	);

	for (uint i = 1; i <= stationSize; i++)
	{
		m_trackManager->BuildTrackPart(
			position - float2(5.0f * static_cast<float>(i), 0.0f),
			TrackDirection::N,
			TrackSegmentID::Invalid,
			position - float2(5.0f * static_cast<float>(i), 10.0f),
			TrackDirection::N,
			TrackSegmentID::Invalid
		);
	}
}
