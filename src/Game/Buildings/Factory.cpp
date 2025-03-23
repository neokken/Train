#include "precomp.h"
#include "Factory.h"

#include "Renderables/LineSegment.h"
#include "World/World.h"

Game::Factory::Factory( const Engine::Transform& transform ): Building(transform)
{
}

void Game::Factory::Start()
{
	RegisterNode(m_transform.position + float2(GRID_SIZE * 2.0f, 0.0f));
}

void Game::Factory::Update( float /*deltaTime*/ )
{
}

void Game::Factory::Render( const Engine::Camera& _camera, Surface& renderTarget )
{
	auto& camera = _camera;
	auto& target = renderTarget;

	float2 base = m_transform.position;

	Engine::LineSegment::RenderWorldPos(camera, target, base, base + float2{200.0f, -50.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, target, base + float2{200.0f, -50.0f}, base + float2{200.0f, -100.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, target, base + float2{200.0f, -100.0f}, base + float2{0.0f, -100.0f}, 0x00ff00);
	Engine::LineSegment::RenderWorldPos(camera, target, base + float2{0.0f, -100.0f}, base, 0x00ff00);
}

void Game::Factory::ImGuiDebugViewer()
{
	ImGui::Text("Factory");
}
