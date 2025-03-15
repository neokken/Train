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

void Game::Building::Render( const Engine::Camera& _camera, Surface& renderTarget )
{
	auto& camera = _camera;
	auto& target = renderTarget;

	float2 base = m_transform.position;

	Engine::LineSegment::RenderWorldPos(camera, target, base, base + float2{200.0f, 0.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, target, base + float2{200.0f, 0.0f}, base + float2{200.0f, -100.0f},
	                                    0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, target, base + float2{200.0f, -100.0f}, base + float2{0.0f, -100.0f},
	                                    0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, target, base + float2{0.0f, -100.0f}, base, 0x00ff00);
}
