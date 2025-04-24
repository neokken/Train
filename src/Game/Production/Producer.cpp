#include "precomp.h"
#include "Producer.h"

#include "Camera/Camera.h"
#include "Renderables/LineSegment.h"

Game::Producer::Producer( Engine::Transform transform, ProducerStats stats ) : GameObject::GameObject(transform)
{
	m_stats = stats;
	m_ticksLeftUntilNextProduction = m_stats.productionRate;
}

void Game::Producer::Update( float deltaTime )
{
}

void Game::Producer::Render( const Engine::Camera& _camera, Surface& renderTarget )
{
	auto& camera = _camera;
	auto& target = renderTarget;

	float2 base = m_transform.position;

	Engine::LineSegment::RenderWorldPos(camera, target, base, base + float2{20.0f, 0.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, target, base + float2{20.0f, 0.0f}, base + float2{20.0f, -10.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, target, base + float2{20.0f, -10.0f}, base + float2{0.0f, -10.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, target, base + float2{0.0f, -10.0f}, base, 0x00ff00);

	float2 pos = camera.GetCameraPosition(m_transform.position + float2(8.0f, -5.0f));
	std::string value = fmt::format("{} / {}", m_produce, m_stats.produceLimit);
	renderTarget.Print(value.c_str(), static_cast<int>(pos.x), static_cast<int>(pos.y), 0xffffff);
}

void Game::Producer::Simulate()
{
	m_ticksLeftUntilNextProduction--;
	if (m_ticksLeftUntilNextProduction > 1) return;

	m_ticksLeftUntilNextProduction = m_stats.productionRate;
	m_produce += m_stats.producePerProduction;
	m_produce = clamp(m_produce, 0u, m_stats.produceLimit);
}
