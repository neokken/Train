#include "precomp.h"
#include "Engine/Data/ParticleSystem.h"

Engine::ParticleSystem::ParticleSystem( const int _amountOfParticlesInPool ) :
	m_amountOfParticles(_amountOfParticlesInPool)
{
	m_particles = new Particle[_amountOfParticlesInPool];
	m_currentAliveIterator = 0;
}

Engine::ParticleSystem::~ParticleSystem()
{
	delete m_particles;
}

void Engine::ParticleSystem::SpawnParticles( float2 position, float spawnRad, int2 _amount, float2 _force, float2 _lifetime, float2 _radius, uint startColor, uint endColor, float2 worldForce )
{
	int amount = rand() % (_amount.y - _amount.x + 1) + _amount.x;

	if (amount <= 0) return;

	if (m_currentAliveIterator + amount >= m_amountOfParticles)
	{
		// Make sure we do not spawn more than available
		amount = m_amountOfParticles - m_currentAliveIterator;
	}
	if (m_currentAliveIterator + amount >= m_amountOfParticles) return;

	for (int i = m_currentAliveIterator; i < m_currentAliveIterator + amount; i++) // Respawn dead particles
	{
		float force = Rand(_force.y - _force.x) + _force.x;

		float lifetime = Rand(_lifetime.y - _lifetime.x) + _lifetime.x;
		float radius = Rand(_radius.y - _radius.x) + _radius.x;

		Particle* particle = &m_particles[i];
		particle->position = position + float2((RandomFloat() - 0.5f), (RandomFloat() - 0.5f)) * spawnRad;
		particle->startColor = startColor;
		particle->endColor = endColor;
		particle->duration = lifetime;
		particle->lifeTime = lifetime;
		particle->radius = radius;
		//Get random direction
		float angle = Rand(2 * PI);
		float2 velocity;
		velocity.x = cos(angle);
		velocity.y = sin(angle);
		particle->velocity = velocity * force;
		particle->worldForce = worldForce;
	}
	m_currentAliveIterator += amount;
}

void Engine::ParticleSystem::Update( float deltaTime )
{
	for (int i = 0; i < m_currentAliveIterator; i++)
	{
		Particle particle = m_particles[i];
		particle.lifeTime -= deltaTime;
		//Kill if lifetime 0
		if (particle.lifeTime <= 0)
		{
			//Swap with last
			m_particles[i] = m_particles[m_currentAliveIterator - 1];
			m_particles[m_currentAliveIterator] = particle;
			m_currentAliveIterator--;
		}
		else
		{
			particle.velocity += particle.worldForce * deltaTime;
			particle.position += particle.velocity * deltaTime;
			m_particles[i] = particle;
		}
	}
}

void Engine::ParticleSystem::Render( const Camera& camera, Surface& screen ) const
{
	for (int i = 0; i < m_currentAliveIterator; i++)
	{
		Particle* particle = &m_particles[i];

		uint color;
		if (particle->startColor == particle->endColor) color = particle->startColor;
		else color = BlendColors(particle->endColor, particle->startColor, particle->lifeTime / particle->duration);

		float rad = particle->radius * (particle->lifeTime / particle->duration);

		float2 p1 = camera.GetCameraPosition(particle->position - (rad / 2));
		int2 ip1 = int2(static_cast<int>(p1.x), static_cast<int>(p1.y));
		float2 p2 = camera.GetCameraPosition(particle->position + (rad / 2));
		int2 ip2 = int2(static_cast<int>(p2.x), static_cast<int>(p2.y));
		screen.Bar(ip1.x, ip1.y, ip2.x, ip2.y, color);
	}
}

void Engine::ParticleSystem::Clear()
{
	m_currentAliveIterator = 0;
}
