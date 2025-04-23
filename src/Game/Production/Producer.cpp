#include "precomp.h"
#include "Producer.h"
#include "Renderables/LineSegment.h"

Game::Producer::Producer( Engine::Transform transform, ProduceType type ) : GameObject::GameObject(transform)
{
	m_type = type;
}

void Game::Producer::Update( float deltaTime )
{
	m_time += deltaTime;
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
}
