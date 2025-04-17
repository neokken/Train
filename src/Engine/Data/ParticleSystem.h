#pragma once
#include "Camera/Camera.h"

namespace Engine
{
	struct Particle
	{
		float2 position;
		float2 velocity;
		float2 worldForce;
		float radius;
		float duration;
		float lifeTime;
		uint startColor;
		uint endColor;
	};

	class ParticleSystem
	{
	public:
		ParticleSystem( const int _amountOfParticlesInPool );
		~ParticleSystem();
		//All parameters prefixed by rng are randomized in range [parr.x, parr.y]
		void SpawnParticles( float2 position, float spawnRadius = 4, int2 amount = int2(1, 2), float2 rngForce = float2(0.01, 0.02), float2 rngLifetime = float2(100, 700), float2 rngRadius = float2(1, 3), uint startColor = 0xffffff, uint endColor = 0x000000, float2 worldForce = float2(0, 0) );
		void Update( float deltaTime );
		void Render( const Camera& camera, Surface& screen ) const;
		void Clear();

	private:
		const int m_amountOfParticles;
		Particle* m_particles;
		int m_currentAliveIterator = 0; // Particles in the array above this iterator are considered "dead"
	};
}
