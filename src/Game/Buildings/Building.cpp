#include "precomp.h"
#include "Building.h"
#include "Renderables/LineSegment.h"

#include "World/World.h"

Game::Building::Building( Engine::Transform transform ) : GameObject::GameObject(transform)
{
}

void Game::Building::Update( float /*deltaTime*/ )
{
}

void Game::Building::Render( const Engine::Camera& /*_camera*/, Surface& /*renderTarget*/ )
{
}

void Game::Building::ImGuiDebugViewer()
{
	ImGui::Text("<Building>");
}

void Game::Building::RegisterNode( float2 position )
{
	m_associatedNodes.emplace_back(
		m_world->CreateNode(position + float2(GRID_SIZE * 2.0f, 0.0f))
	);
}
