#include "precomp.h"
#include "Building.h"
#include "Renderables/LineSegment.h"

#include "World/World.h"

Game::Building::Building( Engine::Transform transform ) : GameObject::GameObject(transform)
{
}

void Game::Building::Update( float deltaTime )
{
	m_time += deltaTime;
}

void Game::Building::Render( const Engine::Camera& _camera )
{
	auto& camera = _camera;

	float2 base = m_transform.position;

	Engine::LineSegment::RenderWorldPos(camera, base, base + float2{20.0f, 0.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, base + float2{20.0f, 0.0f}, base + float2{20.0f, -10.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, base + float2{20.0f, -10.0f}, base + float2{0.0f, -10.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, base + float2{0.0f, -10.0f}, base, 0x00ff00);
}
